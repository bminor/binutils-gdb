/* Subroutines for handling an "inferior" (child) process as a target
   for debugging, in GDB.
   Copyright 1990, 1991 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

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
#include "frame.h"  /* required by inferior.h */
#include "inferior.h"
#include "target.h"
#include "wait.h"
#include "gdbcore.h"
#include "ieee-float.h"		/* Required by REGISTER_CONVERT_TO_XXX */

static void
child_prepare_to_store PARAMS ((void));

static int
child_wait PARAMS ((int *));

static void
child_open PARAMS ((char *, int));

static void
child_files_info PARAMS ((struct target_ops *));

static void
child_detach PARAMS ((char *, int));

/* Forward declaration */
extern struct target_ops child_ops;

/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer STATUS.  */

static int
child_wait (status)
     int *status;
{
  int pid;

  do {
#ifdef USE_PROC_FS
    pid = proc_wait (status);
#else
    pid = wait (status);
#endif
    if (pid == -1)		/* No more children to wait for */
      {
	fprintf (stderr, "Child process unexpectedly missing.\n");
	*status = 42;	/* Claim it exited with signal 42 */
        return -1;
      }
  } while (pid != inferior_pid); /* Some other child died or stopped */
  return pid;
}


/*
 * child_detach()
 * takes a program previously attached to and detaches it.
 * The program resumes execution and will no longer stop
 * on signals, etc.  We better not have left any breakpoints
 * in the program or it'll die when it hits one.  For this
 * to work, it may be necessary for the process to have been
 * previously attached.  It *might* work if the program was
 * started via the normal ptrace (PTRACE_TRACEME).
 */

static void
child_detach (args, from_tty)
     char *args;
     int from_tty;
{
  int siggnal = 0;

#ifdef ATTACH_DETACH
  if (from_tty)
    {
      char *exec_file = get_exec_file (0);
      if (exec_file == 0)
	exec_file = "";
      printf ("Detaching program: %s pid %d\n",
	      exec_file, inferior_pid);
      fflush (stdout);
    }
  if (args)
    siggnal = atoi (args);
  
  detach (siggnal);
  inferior_pid = 0;
  unpush_target (&child_ops);		/* Pop out of handling an inferior */
#else
    error ("This version of Unix does not support detaching a process.");
#endif
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
child_prepare_to_store ()
{
#ifdef CHILD_PREPARE_TO_STORE
  CHILD_PREPARE_TO_STORE ();
#endif
}

/* Print status information about what we're accessing.  */

static void
child_files_info (ignore)
     struct target_ops *ignore;
{
  printf ("\tUsing the running image of %s process %d.\n",
	  attach_flag? "attached": "child", inferior_pid);
}

/* ARGSUSED */
static void
child_open (arg, from_tty)
     char *arg;
     int from_tty;
{
  error ("Use the \"run\" command to start a Unix child process.");
}

struct target_ops child_ops = {
  "child",			/* to_shortname */
  "Unix child process",		/* to_longname */
  "Unix child process (started by the \"run\" command).",	/* to_doc */
  child_open,			/* to_open */
  0,				/* to_close */
  child_attach,			/* to_attach */
  child_detach, 		/* to_detach */
  child_resume,			/* to_resume */
  child_wait,			/* to_wait */
  fetch_inferior_registers,	/* to_fetch_registers */
  store_inferior_registers,	/* to_store_registers */
  child_prepare_to_store,	/* to_prepare_to_store */
  child_xfer_memory,		/* to_xfer_memory */
  child_files_info,		/* to_files_info */
  memory_insert_breakpoint,	/* to_insert_breakpoint */
  memory_remove_breakpoint,	/* to_remove_breakpoint */
  terminal_init_inferior,	/* to_terminal_init */
  terminal_inferior, 		/* to_terminal_inferior */
  terminal_ours_for_output,	/* to_terminal_ours_for_output */
  terminal_ours,		/* to_terminal_ours */
  child_terminal_info,		/* to_terminal_info */
  kill_inferior,		/* to_kill */
  0,				/* to_load */
  0,				/* to_lookup_symbol */
  child_create_inferior,	/* to_create_inferior */
  child_mourn_inferior,		/* to_mourn_inferior */
  process_stratum,		/* to_stratum */
  0,				/* to_next */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  0,				/* sections */
  0,				/* sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_inftarg ()
{
  add_target (&child_ops);
}
