/* Generic remote debugging interface for simulators.
   Copyright 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.
   Steve Chamberlain (sac@cygnus.com) and Doug Evans (dje@cygnus.com).

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
#include "remote-sim.h"
#include "remote-utils.h"

/* Naming convention:

   sim_* are the interface to the simulator (see remote-sim.h).

   gdbsim_* are stuff which is internal to gdb.  */

/* Forward data declarations */
extern struct target_ops gdbsim_ops;

static int program_loaded = 0;

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

static void
gdbsim_fetch_register (regno)
int regno;
{
  if (regno == -1) 
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	gdbsim_fetch_register (regno);
    }
  else
    {
      char buf[MAX_REGISTER_RAW_SIZE];

      sim_fetch_register (regno, buf);
      supply_register (regno, buf);
      if (sr_get_debug ())
	{
	  printf_filtered ("gdbsim_fetch_register: %d", regno);
	  /* FIXME: We could print something more intelligible.  */
	  dump_mem (buf, REGISTER_RAW_SIZE (regno));
	}
    }
}

static void
gdbsim_store_register (regno)
int regno;
{
  if (regno  == -1) 
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	gdbsim_store_register (regno);
    }
  else
    {
      /* FIXME: Until read_register() returns LONGEST, we have this.  */
      char value[MAX_REGISTER_RAW_SIZE];
      read_register_gen (regno, value);
      sim_store_register (regno, value);
      if (sr_get_debug ())
	{
	  printf_filtered ("gdbsim_store_register: %d", regno);
	  /* FIXME: We could print something more intelligible.  */
	  dump_mem (value, REGISTER_RAW_SIZE (regno));
	}
    }
}

static void
gdbsim_kill ()
{
  if (sr_get_debug ())
    printf_filtered ("gdbsim_kill\n");

  sim_kill ();	/* close fd's, remove mappings */
  inferior_pid = 0;
}

/* Load an executable file into the target process.  This is expected to
   not only bring new code into the target process, but also to update
   GDB's symbol tables to match.  */

static void
gdbsim_load (prog, fromtty)
     char *prog;
     int fromtty;
{
  bfd	*abfd;

  if (sr_get_debug ())
    printf_filtered ("gdbsim_load: prog \"%s\"\n", prog);

  inferior_pid = 0;  
  program_loaded = 0;
  abfd = bfd_openr (prog, gnutarget);

  if (!abfd) 
    error ("Unable to open file %s.", prog);

  if (bfd_check_format (abfd, bfd_object) == 0)
    error ("File is not an object file.");

  if (sim_load (abfd, prog) != 0)
    return;

  program_loaded = 1;

  sim_set_pc (abfd->start_address);
}

/*
 * This is a utility routine that sim_load() can call to do the work.
 * The result is 0 for success, non-zero for failure.
 *
 * Eg: int sim_load (bfd *abfd, char *prog) { return sim_load_standard (abfd); }
 */

sim_load_standard (abfd)
     bfd *abfd;
{
  asection *s;

  s = abfd->sections;
  while (s != (asection *)NULL) 
  {
    if (s->flags & SEC_LOAD) 
    {
      int i;
      int delta = 4096;
      char *buffer = xmalloc (delta);
      printf_filtered ("%s\t: 0x%4x .. 0x%4x  ",
		      s->name, s->vma, s->vma + s->_raw_size);
      for (i = 0; i < s->_raw_size; i+= delta) 
      {
	int sub_delta = delta;
	if (sub_delta > s->_raw_size - i)
	  sub_delta = s->_raw_size - i ;

	bfd_get_section_contents (abfd, s, buffer, i, sub_delta);
	sim_write (s->vma + i, buffer, sub_delta);
	printf_filtered ("*");
	fflush (stdout);
      }
      printf_filtered ("\n");
      free (buffer);
    }
    s = s->next;
  }

  return 0;
}

/* Start an inferior process and set inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().
   On VxWorks and various standalone systems, we ignore exec_file.  */
/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */

static void
gdbsim_create_inferior (exec_file, args, env)
     char *exec_file;
     char *args;
     char **env;
{
  int len,entry_pt;
  char *arg_buf,**argv;

  if (! program_loaded)
    error ("No program loaded.");

  if (sr_get_debug ())
    printf_filtered ("gdbsim_create_inferior: exec_file \"%s\", args \"%s\"\n",
      exec_file, args);

  if (exec_file == 0 || exec_bfd == 0)
   error ("No exec file specified.");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

  gdbsim_kill (NULL, NULL);	 
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
  /* FIXME: remote-sim.h says targets that don't support this return
     non-zero.  Perhaps distinguish between "not supported" and other errors?
     Or maybe that can be the only error.  */
  if (sim_set_args (argv, env) != 0)
    return;

  inferior_pid = 42;
  insert_breakpoints ();	/* Needed to get correct instruction in cache */
  proceed (entry_pt, -1, 0);
}

/* The open routine takes the rest of the parameters from the command,
   and (if successful) pushes a new target onto the stack.
   Targets should supply this routine, if only to provide an error message.  */
/* Called when selecting the simulator. EG: (gdb) target sim name.  */

static void
gdbsim_open (args, from_tty)
     char *args;
     int from_tty;
{
  if (sr_get_debug ())
    printf_filtered ("gdbsim_open: args \"%s\"\n", args);

  if (sim_open (args) != 0)
    {
      /* FIXME: This is totally bogus.  sim_open should have a way to
	 tell us what the error was, so we can tell the user.  */
      error ("Unable to initialize simulator (insufficient memory?).");
      return;
    }

  push_target (&gdbsim_ops);
  target_fetch_registers (-1);

  printf_filtered ("Connected to the simulator.\n");
}

/* Does whatever cleanup is required for a target that we are no longer
   going to be calling.  Argument says whether we are quitting gdb and
   should not get hung in case of errors, or whether we want a clean
   termination even if it takes a while.  This routine is automatically
   always called just before a routine is popped off the target stack.
   Closing file descriptors and freeing memory are typical things it should
   do.  */
/* Close out all files and local state before this target loses control. */

static void
gdbsim_close (quitting)
     int quitting;
{
  if (sr_get_debug ())
    printf_filtered ("gdbsim_close: quitting %d\n", quitting);

  program_loaded = 0;

  /* FIXME: Need to call sim_close() to close all files and
     delete all mappings. */
}

/* Takes a program previously attached to and detaches it.
   The program may resume execution (some targets do, some don't) and will
   no longer stop on signals, etc.  We better not have left any breakpoints
   in the program or it'll die when it hits one.  ARGS is arguments
   typed by the user (e.g. a signal to send the process).  FROM_TTY
   says whether to be verbose or not.  */
/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else with your gdb.  */

static void
gdbsim_detach (args,from_tty)
     char *args;
     int from_tty;
{
  if (sr_get_debug ())
    printf_filtered ("gdbsim_detach: args \"%s\"\n", args);

  pop_target ();		/* calls gdbsim_close to do the real work */
  if (from_tty)
    printf_filtered ("Ending simulator %s debugging\n", target_shortname);
}
 
/* Resume execution of the target process.  STEP says whether to single-step
   or to run free; SIGGNAL is the signal value (e.g. SIGINT) to be given
   to the target, or zero for no signal.  */

static void
gdbsim_resume (pid, step, siggnal)
     int pid, step, siggnal;
{
  if (sr_get_debug ())
    printf_filtered ("gdbsim_resume: step %d, signal %d\n", step, siggnal);

  sim_resume (step, siggnal);
}

/* Wait for inferior process to do something.  Return pid of child,
   or -1 in case of error; store status through argument pointer STATUS,
   just as `wait' would.  */

static int
gdbsim_wait (pid, status)
     int pid;
     WAITTYPE *status;
{
  if (sr_get_debug ())
    printf_filtered ("gdbsim_wait: ");
  WSETSTOP (*status, sim_stop_signal ());
  if (sr_get_debug ())
    printf_filtered ("status %d\n", *status);
  return inferior_pid;
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
gdbsim_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static int
gdbsim_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;			/* ignored */
{
  if (! program_loaded)
    error ("No program loaded.");

  if (sr_get_debug ())
    {
      printf_filtered ("gdbsim_xfer_inferior_memory: myaddr 0x%x, memaddr 0x%x, len %d, write %d\n",
		       myaddr, memaddr, len, write);
      if (sr_get_debug () && write)
	dump_mem(myaddr, len);
    }

  if (write)
    {
      len = sim_write (memaddr, myaddr, len);
    }
  else 
    {
      len = sim_read (memaddr, myaddr, len);
      if (sr_get_debug () && len > 0)
	dump_mem(myaddr, len);
    } 
  return len;
}

static void
gdbsim_files_info (target)
     struct target_ops *target;
{
  char *file = "nothing";

  if (exec_bfd)
    file = bfd_get_filename (exec_bfd);

  if (sr_get_debug ())
    printf_filtered ("gdbsim_files_info: file \"%s\"\n", file);

  if (exec_bfd)
    {
      printf_filtered ("\tAttached to %s running program %s\n",
		       target_shortname, file);
      sim_info ();
    }
}

/* Clear the sims notion of what the break points are.  */

static void
gdbsim_mourn_inferior () 
{ 
  if (sr_get_debug ())
    printf_filtered ("gdbsim_mourn_inferior:\n");

  remove_breakpoints ();
  generic_mourn_inferior ();
}

/* Define the target subroutine names */

struct target_ops gdbsim_ops = 
{
  "sim", "simulator",
  "Use the simulator",
  gdbsim_open, gdbsim_close, 
  0, gdbsim_detach, gdbsim_resume, gdbsim_wait, /* attach */
  gdbsim_fetch_register, gdbsim_store_register,
  gdbsim_prepare_to_store,
  gdbsim_xfer_inferior_memory, 
  gdbsim_files_info,
  0, 0,				/* Breakpoints */
  0, 0, 0, 0, 0,		/* Terminal handling */
  gdbsim_kill,			/* kill */
  gdbsim_load, 
  0,				/* lookup_symbol */
  gdbsim_create_inferior,		/* create_inferior */ 
  gdbsim_mourn_inferior,		/* mourn_inferior */
  0,				/* can_run */
  0,				/* notice_signals */
  process_stratum, 0,		/* next */
  1, 1, 1, 1, 1,		/* all mem, mem, stack, regs, exec */
  0, 0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

void
_initialize_remote_sim ()
{
  add_target (&gdbsim_ops);
}
