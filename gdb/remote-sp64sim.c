/* Remote debugging interface for SPARC64 Simulator.
   Copyright 1992 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Hacked from Steve Chamberlain's Z8000 work
   by Doug Evans. (dje@cygnus.com).

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "inferior.h"
#include "wait.h"
#include "value.h"
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include "terminal.h"
#include "target.h"
#include "gdbcore.h"
#include "sp64sim.h"

/* Naming conventions:

   simif_xxx are internal objects that describe top level interfaces to the
   simulator (simif for SIMulator InterFace).

   sim_xxx are external counterparts to the simif_xxx objects that must be
   provided by the simulator.  */

/* Forward data declarations */
extern struct target_ops simif_ops;

int simif_verbose = 0;	/* available to the simulator to use */

static int program_loaded = 0;

static void dump_mem ();

static void
simif_fetch_register (regno)
int regno;
{
  if (regno == -1) 
    {
      if (simif_verbose)
	printf_filtered ("simif_fetch_register: %d\n", regno);
      for (regno = 0; regno < 16; regno++)
	simif_fetch_register (regno);
    }
  else
    {
      char buf[MAX_REGISTER_RAW_SIZE];

      sim_fetch_register (regno, buf);
      supply_register (regno, buf);
      if (simif_verbose)
	{
	  printf_filtered ("simif_fetch_register: %d", regno);
	  dump_mem (buf, sizeof (REGISTER_TYPE));
	}
    }
}

static void
simif_store_register (regno)
int regno;
{
  if (regno  == -1) 
    {
      if (simif_verbose)
	printf_filtered ("simif_store_register: %d\n", regno);
      for (regno = 0; regno < 16; regno++)
	simif_store_register (regno);
    }
  else 
    {
      char value[sizeof (REGISTER_TYPE)];

      read_register_gen (regno, value);
      SWAP_TARGET_AND_HOST (value, sizeof (REGISTER_TYPE));
      sim_store_register (regno, value);
      if (simif_verbose)
	{
	  printf_filtered ("simif_store_register: %d", regno);
	  dump_mem (value, sizeof (REGISTER_TYPE));
	}
    }
}

static void
simif_kill (arg,from_tty)
char	*arg;
int	from_tty;
{
  if (simif_verbose)
    printf_filtered ("simif_kill: arg \"%s\"\n", arg);

  sim_kill ();	/* close fd's, remove mappings */
  inferior_pid = 0;
}

/* Download a file specified in 'args', to the sim. */

static void
simif_load (args, fromtty)
     char *args;
     int fromtty;
{
  bfd	*abfd;

  if (simif_verbose)
    printf_filtered ("simif_load: args \"%s\"\n", args);

  inferior_pid = 0;  
  program_loaded = 0;
  /* FIXME: a.out should be a config parm and/or an arg.  */
  abfd = bfd_openr (args,"a.out-sunos-big");

  if (!abfd) 
    error ("Unable to open file %s.", args);

  if (bfd_check_format (abfd, bfd_object) ==0) 
    error ("File is not an object file.");

  if (sim_load (abfd, args) != 0)
    return;

  program_loaded = 1;

  /* It is sim_load()'s job to set this.  */
  /*sim_set_pc (abfd->start_address); - can't do 'cus we use RMTVaddr */
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */

static void
simif_create_inferior (exec_file, args, env)
     char *exec_file;
     char *args;
     char **env;
{
  int len,entry_pt;
  char *arg_buf,**argv;

  if (! program_loaded)
    error ("No program loaded.");

  if (simif_verbose)
    printf_filtered ("simif_create_inferior: exec_file \"%s\", args \"%s\"\n",
      exec_file, args);

  if (exec_file == 0 || exec_bfd == 0)
   error ("No exec file specified.");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

  simif_kill (NULL, NULL);	 
  remove_breakpoints ();
  init_wait_for_inferior ();

  len = 5 + strlen (exec_file) + 1 + strlen (args) + 1 + /*slop*/ 10;
  arg_buf = (char *) alloca (len);
  arg_buf[0] = '\0';
  strcat (arg_buf, exec_file);
  strcat (arg_buf, " ");
  strcat (arg_buf, args);
  argv = buildargv (arg_buf);
  make_cleanup (freeargv, (char *) argv);
  sim_set_args (argv, env);

  inferior_pid = 42;
  insert_breakpoints ();	/* Needed to get correct instruction in cache */
  proceed (entry_pt, -1, 0);
}

/* Called when selecting the simulator. EG: (gdb) target sim name.
   NAME unused at present. */

static void
simif_open (name, from_tty)
     char *name;
     int from_tty;
{
  if (simif_verbose)
    printf_filtered ("simif_open: name \"%s\"\n", name);

  if (sim_init (name) != 0)
    {
      error ("Unable to initialize simulator (insufficient memory?).");
      return;
    }

  push_target (&simif_ops);
  target_fetch_registers (-1);

  printf_filtered ("Connected to the simulator.\n");
}

/* Close out all files and local state before this target loses control. */

static void
simif_close (quitting)
     int quitting;
{
  if (simif_verbose)
    printf_filtered ("simif_close: quitting %d\n", quitting);

  program_loaded = 0;

  /* FIXME: Need to call sim_close() to close all files and
     delete all mappings. */
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */

static void
simif_detach (args,from_tty)
     char *args;
     int from_tty;
{
  if (simif_verbose)
    printf_filtered ("simif_detach: args \"%s\"\n", args);

  pop_target ();		/* calls simif_close to do the real work */
  if (from_tty)
    printf_filtered ("Ending simulator %s debugging\n", target_shortname);
}
 
/* Tell the remote machine to resume.  */
/* FIXME: What are A and B?  */

static void
simif_resume (a,b)
{
  if (simif_verbose)
    printf_filtered ("simif_resume: %d/%d\n", a, b);

  sim_resume (a, b);
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
simif_wait (status)
     WAITTYPE *status;
{
  if (simif_verbose)
    printf_filtered ("simif_wait: ");
#if 1
  *status = sim_stop_signal ();
#else
  WSETSTOP (*status, sim_stop_signal ());
#endif
  if (simif_verbose)
    printf_filtered ("status %d\n", *status);
  return 0;
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
simif_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static int
simif_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;			/* ignored */
{
  if (simif_verbose)
  {
    printf_filtered ("simif_xfer_inferior_memory: myaddr 0x%x, memaddr 0x%x, len %d, write %d\n",
      myaddr, memaddr, len, write);
    if (simif_verbose && write)
      dump_mem(myaddr, len);
  }

  if (! program_loaded)
    error ("No program loaded.");

  if (write)
  {
    len = sim_write (memaddr, myaddr, len);
  }
  else 
  {
    len = sim_read (memaddr, myaddr, len);
    if (simif_verbose && len  > 0)
      dump_mem(myaddr, len);
  } 
  return len;
}

static void
simif_files_info ()
{
  char *file = "nothing";

  if (exec_bfd)
    file = bfd_get_filename (exec_bfd);

  if (simif_verbose)
    printf_filtered ("simif_files_info: file \"%s\"\n", file);

  if (exec_bfd)
    printf_filtered ("\tAttached to %s running program %s\n",
      target_shortname, file);
}

/* Clear the sims notion of what the break points are */
static void
simif_mourn () 
{ 
  if (simif_verbose)
    printf_filtered ("simif_mourn:\n");

  remove_breakpoints ();
  generic_mourn_inferior ();
}

/* Define the target subroutine names */

struct target_ops simif_ops = 
{
  "sim", "SPARC64 Simulator",
  "Use the SPARC64 Simulator",
  simif_open, simif_close, 
  0, simif_detach, simif_resume, simif_wait, /* attach */
  simif_fetch_register, simif_store_register,
  simif_prepare_to_store,
  simif_xfer_inferior_memory, 
  simif_files_info,
  0, 0,				/* Breakpoints */
  0, 0, 0, 0, 0,		/* Terminal handling */
  simif_kill,			/* FIXME, kill */
  simif_load, 
  0,				/* lookup_symbol */
  simif_create_inferior,	/* create_inferior */ 
  simif_mourn,			/* mourn_inferior FIXME */
  0,				/* can_run */
  0,				/* notice_signals */
  process_stratum, 0,		/* next */
  1, 1, 1, 1, 1,		/* all mem, mem, stack, regs, exec */
  0, 0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

static void
simif_snoop ()
{
  simif_verbose = ! simif_verbose;  
  if (simif_verbose)
    printf_filtered ("Snoop enabled\n");
  else
    printf_filtered ("Snoop disabled\n");

}

/***********************************************************************/

void
_initialize_remote_sim ()
{
  add_target (&simif_ops);
  add_com ("snoop", class_obscure, simif_snoop,
	   "Show what commands are going to the simulator");
}

static void
dump_mem (buf, len)
     char *buf;
     int len;
{
  if (len <= 8)
  {
    if (len == 8 || len == 4)
    {
      long l[2];
      memcpy (l, buf, len);
      printf_filtered ("\t0x%x", l[0]);
      printf_filtered (len == 8 ? " 0x%x\n" : "\n", l[1]);
    }
    else
    {
      int i;
      printf_filtered ("\t");
      for (i = 0; i < len; i++)
	printf_filtered ("0x%x ", buf[i]);
      printf_filtered ("\n");
    }
  }
}
