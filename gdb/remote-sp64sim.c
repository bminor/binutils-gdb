/* Generic remote debugging interface for simulators.
   Copyright 1993 Free Software Foundation, Inc.
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
#include "simif.h"

/* Naming conventions:

   sim_xxx are internal objects that describe top level interfaces to the
   simulator.

   simif_xxx are external counterparts to the sim_xxx objects that must be
   provided by the simulator (simif for SIMulator InterFace, duh...).

   A complete list of them is:

   --- Fetch one register and store the raw value in BUF.

int simif_fetch_register (int regno, char *buf);

   --- Store VAL in one register.

int simif_store_register (int regno, char *val);

   --- Complete terminate the simulator.  This includes freeing all memory,
   closing all open files, and releasing all mmap'd memory.

int simif_kill (void);

   --- Load program PROG into the simulator.

int simif_load (bfd *abfd, char *prog);

   --- Set the arguments and environment for the program loaded into the
   simulator.  ARGV and ENV are NULL terminated lists of pointers.

int simif_set_args (char **argv, char **env);

   --- Initialize the simulator.  This function is called when the simulator
   is selected from the command line. ARGS is passed from the command line
   and can be used to select whatever run time options the simulator provides.
   ARGS is the raw character string and must be parsed by the simulator.

int simif_open (char *args);

   --- Start running the program, or resume it after a breakpoint.
   FIXME: What are A and B?

int simif_resume (int a, int b);

   --- Fetch the reason why the program stopped running (breakpoint, signal,
   etc.)

WAITTYPE simif_stop_signal (void);

   --- Write some data into the program's memory.
   Result is 0 for success, nonzero for failure.

int simif_write (CORE_ADDR memaddr, char *myaddr, int len);

   --- Read some data from the program's memory.
   Result is 0 for success, nonzero for failure.

int simif_read (CORE_ADDR memaddr, char *myaddr, int len);
*/

/* Forward data declarations */
extern struct target_ops sim_ops;

int sim_verbose = 0;	/* available to the simulator to use */

static int program_loaded = 0;

static void dump_mem ();

static void
sim_fetch_register (regno)
int regno;
{
  if (regno == -1) 
    {
      if (sim_verbose)
	printf_filtered ("sim_fetch_register: %d\n", regno);
      /* FIXME: Where did the 16 come from and what does it need to be? */
      for (regno = 0; regno < 16; regno++)
	sim_fetch_register (regno);
    }
  else
    {
      char buf[MAX_REGISTER_RAW_SIZE];

      simif_fetch_register (regno, buf);
      supply_register (regno, buf);
      if (sim_verbose)
	{
	  printf_filtered ("sim_fetch_register: %d", regno);
	  dump_mem (buf, sizeof (REGISTER_TYPE));
	}
    }
}

static void
sim_store_register (regno)
int regno;
{
  if (regno  == -1) 
    {
      if (sim_verbose)
	printf_filtered ("sim_store_register: %d\n", regno);
      /* FIXME: 16? */
      for (regno = 0; regno < 16; regno++)
	sim_store_register (regno);
    }
  else
    {
      char value[sizeof (REGISTER_TYPE)];

      read_register_gen (regno, value);
      SWAP_TARGET_AND_HOST (value, sizeof (REGISTER_TYPE));
      simif_store_register (regno, value);
      if (sim_verbose)
	{
	  printf_filtered ("sim_store_register: %d", regno);
	  dump_mem (value, sizeof (REGISTER_TYPE));
	}
    }
}

static void
sim_kill (arg,from_tty)
char	*arg;
int	from_tty;
{
  if (sim_verbose)
    printf_filtered ("sim_kill: arg \"%s\"\n", arg);

  simif_kill ();	/* close fd's, remove mappings */
  inferior_pid = 0;
}

/* Load program PROG into the sim. */

static void
sim_load (prog, fromtty)
     char *prog;
     int fromtty;
{
  bfd	*abfd;

  if (sim_verbose)
    printf_filtered ("sim_load: prog \"%s\"\n", prog);

  inferior_pid = 0;  
  program_loaded = 0;
  abfd = bfd_openr (prog, (char *) 0);

  if (!abfd) 
    error ("Unable to open file %s.", prog);

  if (bfd_check_format (abfd, bfd_object) ==0) 
    error ("File is not an object file.");

  if (simif_load (abfd, prog) != 0)
    return;

  program_loaded = 1;

  simif_set_pc (abfd->start_address);
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */

static void
sim_create_inferior (exec_file, args, env)
     char *exec_file;
     char *args;
     char **env;
{
  int len,entry_pt;
  char *arg_buf,**argv;

  if (! program_loaded)
    error ("No program loaded.");

  if (sim_verbose)
    printf_filtered ("sim_create_inferior: exec_file \"%s\", args \"%s\"\n",
      exec_file, args);

  if (exec_file == 0 || exec_bfd == 0)
   error ("No exec file specified.");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

  sim_kill (NULL, NULL);	 
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
  simif_set_args (argv, env);

  inferior_pid = 42;
  insert_breakpoints ();	/* Needed to get correct instruction in cache */
  proceed (entry_pt, -1, 0);
}

/* Called when selecting the simulator. EG: (gdb) target sim name.  */

static void
sim_open (args, from_tty)
     char *args;
     int from_tty;
{
  if (sim_verbose)
    printf_filtered ("sim_open: args \"%s\"\n", args);

  if (simif_open (args) != 0)
    {
      error ("Unable to initialize simulator (insufficient memory?).");
      return;
    }

  push_target (&sim_ops);
  target_fetch_registers (-1);

  printf_filtered ("Connected to the simulator.\n");
}

/* Close out all files and local state before this target loses control. */

static void
sim_close (quitting)
     int quitting;
{
  if (sim_verbose)
    printf_filtered ("sim_close: quitting %d\n", quitting);

  program_loaded = 0;

  /* FIXME: Need to call simif_close() to close all files and
     delete all mappings. */
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */

static void
sim_detach (args,from_tty)
     char *args;
     int from_tty;
{
  if (sim_verbose)
    printf_filtered ("sim_detach: args \"%s\"\n", args);

  pop_target ();		/* calls sim_close to do the real work */
  if (from_tty)
    printf_filtered ("Ending simulator %s debugging\n", target_shortname);
}
 
/* Tell the remote machine to resume.  */
/* FIXME: What are A and B?  */

static void
sim_resume (a,b)
{
  if (sim_verbose)
    printf_filtered ("sim_resume: %d/%d\n", a, b);

  simif_resume (a, b);
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
sim_wait (status)
     WAITTYPE *status;
{
  if (sim_verbose)
    printf_filtered ("sim_wait: ");
#if 1
  *status = simif_stop_signal ();
#else
  WSETSTOP (*status, simif_stop_signal ());
#endif
  if (sim_verbose)
    printf_filtered ("status %d\n", *status);
  return 0;
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
sim_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static int
sim_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;			/* ignored */
{
  if (sim_verbose)
    {
      printf_filtered ("sim_xfer_inferior_memory: myaddr 0x%x, memaddr 0x%x, len %d, write %d\n",
		       myaddr, memaddr, len, write);
      if (sim_verbose && write)
	dump_mem(myaddr, len);
    }

  if (! program_loaded)
    error ("No program loaded.");

  if (write)
    {
      len = simif_write (memaddr, myaddr, len);
    }
  else 
    {
      len = simif_read (memaddr, myaddr, len);
      if (sim_verbose && len  > 0)
	dump_mem(myaddr, len);
    } 
  return len;
}

static void
sim_files_info ()
{
  char *file = "nothing";

  if (exec_bfd)
    file = bfd_get_filename (exec_bfd);

  if (sim_verbose)
    printf_filtered ("sim_files_info: file \"%s\"\n", file);

  if (exec_bfd)
    printf_filtered ("\tAttached to %s running program %s\n",
      target_shortname, file);
}

/* Clear the sims notion of what the break points are */
static void
sim_mourn () 
{ 
  if (sim_verbose)
    printf_filtered ("sim_mourn:\n");

  remove_breakpoints ();
  generic_mourn_inferior ();
}

/* Define the target subroutine names */

struct target_ops sim_ops = 
{
  "sim", "Simulator",
  "Use the Simulator",
  sim_open, sim_close, 
  0, sim_detach, sim_resume, sim_wait, /* attach */
  sim_fetch_register, sim_store_register,
  sim_prepare_to_store,
  sim_xfer_inferior_memory, 
  sim_files_info,
  0, 0,				/* Breakpoints */
  0, 0, 0, 0, 0,		/* Terminal handling */
  sim_kill,			/* FIXME, kill */
  sim_load, 
  0,				/* lookup_symbol */
  sim_create_inferior,	/* create_inferior */ 
  sim_mourn,			/* mourn_inferior FIXME */
  0,				/* can_run */
  0,				/* notice_signals */
  process_stratum, 0,		/* next */
  1, 1, 1, 1, 1,		/* all mem, mem, stack, regs, exec */
  0, 0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

static void
sim_snoop ()
{
  sim_verbose = ! sim_verbose;  
  if (sim_verbose)
    printf_filtered ("Snoop enabled\n");
  else
    printf_filtered ("Snoop disabled\n");

}

/***********************************************************************/

void
_initialize_remote_sim ()
{
  add_target (&sim_ops);
  add_com ("snoop", class_obscure, sim_snoop,
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
