/* This file inplements the host independent child process statum.

   Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
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
#include "terminal.h"		/* For #ifdef TIOCGPGRP and new_tty */

#include <signal.h>

#ifdef SET_STACK_LIMIT_HUGE
#include <sys/time.h>
#include <sys/resource.h>

extern int original_stack_limit;
#endif /* SET_STACK_LIMIT_HUGE */

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

static void
child_attach PARAMS ((char *, int));

static void
child_create_inferior PARAMS ((char *, char *, char **));

static void
child_mourn_inferior PARAMS ((void));

extern char **environ;

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


/* Attach to process PID, then initialize for debugging it
   and wait for the trace-trap that results from attaching.  */

static void
child_attach (args, from_tty)
     char *args;
     int from_tty;
{
  char *exec_file;
  int pid;

  dont_repeat();

  if (!args)
    error_no_arg ("process-id to attach");

#ifndef ATTACH_DETACH
  error ("Can't attach to a process on this machine.");
#else
  pid = atoi (args);

  if (pid == getpid())		/* Trying to masturbate? */
    error ("I refuse to debug myself!");

  if (target_has_execution)
    {
      if (query ("A program is being debugged already.  Kill it? "))
	target_kill ();
      else
	error ("Inferior not killed.");
    }

  if (from_tty)
    {
      exec_file = (char *) get_exec_file (0);

      if (exec_file)
	printf ("Attaching program `%s', pid %d\n", exec_file, pid);
      else
	printf ("Attaching pid %d\n", pid);

      fflush (stdout);
    }

  attach (pid);
  inferior_pid = pid;
  push_target (&child_ops);

  mark_breakpoints_out ();
  target_terminal_init ();
  clear_proceed_status ();
  stop_soon_quietly = 1;
  /*proceed (-1, 0, -2);*/
  target_terminal_inferior ();
  wait_for_inferior ();
#ifdef SOLIB_ADD
  SOLIB_ADD ((char *)0, from_tty, (struct target_ops *)0);
#endif
  normal_stop ();
#endif  /* ATTACH_DETACH */
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

/* Start an inferior Unix child process and sets inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().  */

#ifndef SHELL_FILE
#define SHELL_FILE "/bin/sh"
#endif

static void
child_create_inferior (exec_file, allargs, env)
     char *exec_file;
     char *allargs;
     char **env;
{
  int pid;
  char *shell_command;
  char *shell_file;
  static char default_shell_file[] = SHELL_FILE;
  int len;
  int pending_execs;
  /* Set debug_fork then attach to the child while it sleeps, to debug. */
  static int debug_fork = 0;
  /* This is set to the result of setpgrp, which if vforked, will be visible
     to you in the parent process.  It's only used by humans for debugging.  */
  static int debug_setpgrp = 657473;
  char **save_our_env;

  /* If no exec file handed to us, get it from the exec-file command -- with
     a good, common error message if none is specified.  */
  if (exec_file == 0)
    exec_file = get_exec_file(1);

  /* The user might want tilde-expansion, and in general probably wants
     the program to behave the same way as if run from
     his/her favorite shell.  So we let the shell run it for us.
     FIXME, this should probably search the local environment (as
     modified by the setenv command), not the env gdb inherited.  */
  shell_file = getenv ("SHELL");
  if (shell_file == NULL)
    shell_file = default_shell_file;
  
  len = 5 + strlen (exec_file) + 1 + strlen (allargs) + 1 + /*slop*/ 10;
  /* If desired, concat something onto the front of ALLARGS.
     SHELL_COMMAND is the result.  */
#ifdef SHELL_COMMAND_CONCAT
  shell_command = (char *) alloca (strlen (SHELL_COMMAND_CONCAT) + len);
  strcpy (shell_command, SHELL_COMMAND_CONCAT);
#else
  shell_command = (char *) alloca (len);
  shell_command[0] = '\0';
#endif
  strcat (shell_command, "exec ");
  strcat (shell_command, exec_file);
  strcat (shell_command, " ");
  strcat (shell_command, allargs);

  /* exec is said to fail if the executable is open.  */
  close_exec_file ();

  /* Retain a copy of our environment variables, since the child will
     replace the value of  environ  and if we're vforked, we have to 
     restore it.  */
  save_our_env = environ;

  /* Tell the terminal handling subsystem what tty we plan to run on;
     it will just record the information for later.  */

  new_tty_prefork (inferior_io_terminal);

  /* It is generally good practice to flush any possible pending stdio
     output prior to doing a fork, to avoid the possibility of both the
     parent and child flushing the same data after the fork. */

  fflush (stdout);
  fflush (stderr);

#if defined(USG) && !defined(HAVE_VFORK)
  pid = fork ();
#else
  if (debug_fork)
    pid = fork ();
  else
    pid = vfork ();
#endif

  if (pid < 0)
    perror_with_name ("vfork");

  if (pid == 0)
    {
      if (debug_fork) 
	sleep (debug_fork);

#ifdef TIOCGPGRP
      /* Run inferior in a separate process group.  */
#ifdef NEED_POSIX_SETPGID
      debug_setpgrp = setpgid (0, 0);
#else
#if defined(USG) && !defined(SETPGRP_ARGS)
      debug_setpgrp = setpgrp ();
#else
      debug_setpgrp = setpgrp (getpid (), getpid ());
#endif /* USG */
#endif /* NEED_POSIX_SETPGID */
      if (debug_setpgrp == -1)
	 perror("setpgrp failed in child");
#endif /* TIOCGPGRP */

#ifdef SET_STACK_LIMIT_HUGE
      /* Reset the stack limit back to what it was.  */
      {
	struct rlimit rlim;

	getrlimit (RLIMIT_STACK, &rlim);
	rlim.rlim_cur = original_stack_limit;
	setrlimit (RLIMIT_STACK, &rlim);
      }
#endif /* SET_STACK_LIMIT_HUGE */

      /* Ask the tty subsystem to switch to the one we specified earlier
	 (or to share the current terminal, if none was specified).  */

      new_tty ();

      /* Changing the signal handlers for the inferior after
	 a vfork can also change them for the superior, so we don't mess
	 with signals here.  See comments in
	 initialize_signals for how we get the right signal handlers
	 for the inferior.  */

#ifdef USE_PROC_FS
      /* Use SVR4 /proc interface */
      proc_set_exec_trap ();
#else
      /* "Trace me, Dr. Memory!" */
      call_ptrace (0, 0, (PTRACE_ARG3_TYPE) 0, 0);
#endif

      /* There is no execlpe call, so we have to set the environment
	 for our child in the global variable.  If we've vforked, this
	 clobbers the parent, but environ is restored a few lines down
	 in the parent.  By the way, yes we do need to look down the
	 path to find $SHELL.  Rich Pixley says so, and I agree.  */
      environ = env;
      execlp (shell_file, shell_file, "-c", shell_command, (char *)0);

      fprintf (stderr, "Cannot exec %s: %s.\n", shell_file,
	       safe_strerror (errno));
      fflush (stderr);
      _exit (0177);
    }

  /* Restore our environment in case a vforked child clob'd it.  */
  environ = save_our_env;

  /* Now that we have a child process, make it our target.  */
  push_target (&child_ops);

#ifdef CREATE_INFERIOR_HOOK
  CREATE_INFERIOR_HOOK (pid);
#endif  

/* The process was started by the fork that created it,
   but it will have stopped one instruction after execing the shell.
   Here we must get it up to actual execution of the real program.  */

  inferior_pid = pid;		/* Needed for wait_for_inferior stuff below */

  clear_proceed_status ();

  /* We will get a trace trap after one instruction.
     Continue it automatically.  Eventually (after shell does an exec)
     it will get another trace trap.  Then insert breakpoints and continue.  */

#ifdef START_INFERIOR_TRAPS_EXPECTED
  pending_execs = START_INFERIOR_TRAPS_EXPECTED;
#else
  pending_execs = 2;
#endif

  init_wait_for_inferior ();

  /* Set up the "saved terminal modes" of the inferior
     based on what modes we are starting it with.  */
  target_terminal_init ();

  /* Install inferior's terminal modes.  */
  target_terminal_inferior ();

  while (1)
    {
      stop_soon_quietly = 1;	/* Make wait_for_inferior be quiet */
      wait_for_inferior ();
      if (stop_signal != SIGTRAP)
	{
	  /* Let shell child handle its own signals in its own way */
	  /* FIXME, what if child has exit()ed?  Must exit loop somehow */
	  resume (0, stop_signal);
	}
      else
	{
	  /* We handle SIGTRAP, however; it means child did an exec.  */
	  if (0 == --pending_execs)
	    break;
	  resume (0, 0);		/* Just make it go on */
	}
    }
  stop_soon_quietly = 0;

  /* We are now in the child process of interest, having exec'd the
     correct program, and are poised at the first instruction of the
     new program.  */
#ifdef SOLIB_CREATE_INFERIOR_HOOK
  SOLIB_CREATE_INFERIOR_HOOK (pid);
#endif

  /* Pedal to the metal.  Away we go.  */
  proceed ((CORE_ADDR) -1, 0, 0);
}

static void
child_mourn_inferior ()
{
  unpush_target (&child_ops);
  generic_mourn_inferior ();
}

static int
child_can_run ()
{
  return(1);
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
  child_can_run,		/* to_can_run */
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
