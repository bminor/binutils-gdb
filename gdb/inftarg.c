/* Target-vector operations for controlling Unix child processes, for GDB.
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
#include "command.h"
#include <signal.h>

static void
child_prepare_to_store PARAMS ((void));

#ifndef CHILD_WAIT
static int
child_wait PARAMS ((int, struct target_waitstatus *));
#endif /* CHILD_WAIT */

static void
child_open PARAMS ((char *, int));

static void
child_files_info PARAMS ((struct target_ops *));

static void
child_detach PARAMS ((char *, int));

static void
child_attach PARAMS ((char *, int));

static void
ptrace_me PARAMS ((void));

static void
ptrace_him PARAMS ((int));

static void
child_create_inferior PARAMS ((char *, char *, char **));

static void
child_mourn_inferior PARAMS ((void));

static int
child_can_run PARAMS ((void));

extern char **environ;

/* Forward declaration */
extern struct target_ops child_ops;

/* Convert host signal to our signals.  */
enum target_signal
target_signal_from_host (hostsig)
     int hostsig;
{
  /* A switch statement would make sense but would require special kludges
     to deal with the cases where more than one signal has the same number.  */

  if (hostsig == 0) return TARGET_SIGNAL_0;

#if defined (SIGHUP)
  if (hostsig == SIGHUP) return TARGET_SIGNAL_HUP;
#endif
#if defined (SIGINT)
  if (hostsig == SIGINT) return TARGET_SIGNAL_INT;
#endif
#if defined (SIGQUIT)
  if (hostsig == SIGQUIT) return TARGET_SIGNAL_QUIT;
#endif
#if defined (SIGILL)
  if (hostsig == SIGILL) return TARGET_SIGNAL_ILL;
#endif
#if defined (SIGTRAP)
  if (hostsig == SIGTRAP) return TARGET_SIGNAL_TRAP;
#endif
#if defined (SIGABRT)
  if (hostsig == SIGABRT) return TARGET_SIGNAL_ABRT;
#endif
#if defined (SIGEMT)
  if (hostsig == SIGEMT) return TARGET_SIGNAL_EMT;
#endif
#if defined (SIGFPE)
  if (hostsig == SIGFPE) return TARGET_SIGNAL_FPE;
#endif
#if defined (SIGKILL)
  if (hostsig == SIGKILL) return TARGET_SIGNAL_KILL;
#endif
#if defined (SIGBUS)
  if (hostsig == SIGBUS) return TARGET_SIGNAL_BUS;
#endif
#if defined (SIGSEGV)
  if (hostsig == SIGSEGV) return TARGET_SIGNAL_SEGV;
#endif
#if defined (SIGSYS)
  if (hostsig == SIGSYS) return TARGET_SIGNAL_SYS;
#endif
#if defined (SIGPIPE)
  if (hostsig == SIGPIPE) return TARGET_SIGNAL_PIPE;
#endif
#if defined (SIGALRM)
  if (hostsig == SIGALRM) return TARGET_SIGNAL_ALRM;
#endif
#if defined (SIGTERM)
  if (hostsig == SIGTERM) return TARGET_SIGNAL_TERM;
#endif
#if defined (SIGUSR1)
  if (hostsig == SIGUSR1) return TARGET_SIGNAL_USR1;
#endif
#if defined (SIGUSR2)
  if (hostsig == SIGUSR2) return TARGET_SIGNAL_USR2;
#endif
#if defined (SIGCLD)
  if (hostsig == SIGCLD) return TARGET_SIGNAL_CHLD;
#endif
#if defined (SIGCHLD)
  if (hostsig == SIGCHLD) return TARGET_SIGNAL_CHLD;
#endif
#if defined (SIGPWR)
  if (hostsig == SIGPWR) return TARGET_SIGNAL_PWR;
#endif
#if defined (SIGWINCH)
  if (hostsig == SIGWINCH) return TARGET_SIGNAL_WINCH;
#endif
#if defined (SIGURG)
  if (hostsig == SIGURG) return TARGET_SIGNAL_URG;
#endif
#if defined (SIGIO)
  if (hostsig == SIGIO) return TARGET_SIGNAL_IO;
#endif
#if defined (SIGPOLL)
  if (hostsig == SIGPOLL) return TARGET_SIGNAL_POLL;
#endif
#if defined (SIGSTOP)
  if (hostsig == SIGSTOP) return TARGET_SIGNAL_STOP;
#endif
#if defined (SIGTSTP)
  if (hostsig == SIGTSTP) return TARGET_SIGNAL_TSTP;
#endif
#if defined (SIGCONT)
  if (hostsig == SIGCONT) return TARGET_SIGNAL_CONT;
#endif
#if defined (SIGTTIN)
  if (hostsig == SIGTTIN) return TARGET_SIGNAL_TTIN;
#endif
#if defined (SIGTTOU)
  if (hostsig == SIGTTOU) return TARGET_SIGNAL_TTOU;
#endif
#if defined (SIGVTALRM)
  if (hostsig == SIGVTALRM) return TARGET_SIGNAL_VTALRM;
#endif
#if defined (SIGPROF)
  if (hostsig == SIGPROF) return TARGET_SIGNAL_PROF;
#endif
#if defined (SIGXCPU)
  if (hostsig == SIGXCPU) return TARGET_SIGNAL_XCPU;
#endif
#if defined (SIGXFSZ)
  if (hostsig == SIGXFSZ) return TARGET_SIGNAL_XFSZ;
#endif
#if defined (SIGWIND)
  if (hostsig == SIGWIND) return TARGET_SIGNAL_WIND;
#endif
#if defined (SIGPHONE)
  if (hostsig == SIGPHONE) return TARGET_SIGNAL_PHONE;
#endif
#if defined (SIGLOST)
  if (hostsig == SIGLOST) return TARGET_SIGNAL_LOST;
#endif
#if defined (SIGWAITING)
  if (hostsig == SIGWAITING) return TARGET_SIGNAL_WAITING;
#endif
#if defined (SIGLWP)
  if (hostsig == SIGLWP) return TARGET_SIGNAL_LWP;
#endif
#if defined (SIGDANGER)
  if (hostsig == SIGDANGER) return TARGET_SIGNAL_DANGER;
#endif
#if defined (SIGGRANT)
  if (hostsig == SIGGRANT) return TARGET_SIGNAL_GRANT;
#endif
#if defined (SIGRETRACT)
  if (hostsig == SIGRETRACT) return TARGET_SIGNAL_RETRACT;
#endif
#if defined (SIGMSG)
  if (hostsig == SIGMSG) return TARGET_SIGNAL_MSG;
#endif
#if defined (SIGSOUND)
  if (hostsig == SIGSOUND) return TARGET_SIGNAL_SOUND;
#endif
#if defined (SIGSAK)
  if (hostsig == SIGSAK) return TARGET_SIGNAL_SAK;
#endif
  return TARGET_SIGNAL_UNKNOWN;
}

int
target_signal_to_host (oursig)
     enum target_signal oursig;
{
  switch (oursig)
    {
    case TARGET_SIGNAL_0: return 0;

#if defined (SIGHUP)
    case TARGET_SIGNAL_HUP: return SIGHUP;
#endif
#if defined (SIGINT)
    case TARGET_SIGNAL_INT: return SIGINT;
#endif
#if defined (SIGQUIT)
    case TARGET_SIGNAL_QUIT: return SIGQUIT;
#endif
#if defined (SIGILL)
    case TARGET_SIGNAL_ILL: return SIGILL;
#endif
#if defined (SIGTRAP)
    case TARGET_SIGNAL_TRAP: return SIGTRAP;
#endif
#if defined (SIGABRT)
    case TARGET_SIGNAL_ABRT: return SIGABRT;
#endif
#if defined (SIGEMT)
    case TARGET_SIGNAL_EMT: return SIGEMT;
#endif
#if defined (SIGFPE)
    case TARGET_SIGNAL_FPE: return SIGFPE;
#endif
#if defined (SIGKILL)
    case TARGET_SIGNAL_KILL: return SIGKILL;
#endif
#if defined (SIGBUS)
    case TARGET_SIGNAL_BUS: return SIGBUS;
#endif
#if defined (SIGSEGV)
    case TARGET_SIGNAL_SEGV: return SIGSEGV;
#endif
#if defined (SIGSYS)
    case TARGET_SIGNAL_SYS: return SIGSYS;
#endif
#if defined (SIGPIPE)
    case TARGET_SIGNAL_PIPE: return SIGPIPE;
#endif
#if defined (SIGALRM)
    case TARGET_SIGNAL_ALRM: return SIGALRM;
#endif
#if defined (SIGTERM)
    case TARGET_SIGNAL_TERM: return SIGTERM;
#endif
#if defined (SIGUSR1)
    case TARGET_SIGNAL_USR1: return SIGUSR1;
#endif
#if defined (SIGUSR2)
    case TARGET_SIGNAL_USR2: return SIGUSR2;
#endif
#if defined (SIGCHLD) || defined (SIGCLD)
    case TARGET_SIGNAL_CHLD: 
#if defined (SIGCHLD)
      return SIGCHLD;
#else
      return SIGCLD;
#endif
#endif /* SIGCLD or SIGCHLD */
#if defined (SIGPWR)
    case TARGET_SIGNAL_PWR: return SIGPWR;
#endif
#if defined (SIGWINCH)
    case TARGET_SIGNAL_WINCH: return SIGWINCH;
#endif
#if defined (SIGURG)
    case TARGET_SIGNAL_URG: return SIGURG;
#endif
#if defined (SIGIO)
    case TARGET_SIGNAL_IO: return SIGIO;
#endif
#if defined (SIGPOLL)
    case TARGET_SIGNAL_POLL: return SIGPOLL;
#endif
#if defined (SIGSTOP)
    case TARGET_SIGNAL_STOP: return SIGSTOP;
#endif
#if defined (SIGTSTP)
    case TARGET_SIGNAL_TSTP: return SIGTSTP;
#endif
#if defined (SIGCONT)
    case TARGET_SIGNAL_CONT: return SIGCONT;
#endif
#if defined (SIGTTIN)
    case TARGET_SIGNAL_TTIN: return SIGTTIN;
#endif
#if defined (SIGTTOU)
    case TARGET_SIGNAL_TTOU: return SIGTTOU;
#endif
#if defined (SIGVTALRM)
    case TARGET_SIGNAL_VTALRM: return SIGVTALRM;
#endif
#if defined (SIGPROF)
    case TARGET_SIGNAL_PROF: return SIGPROF;
#endif
#if defined (SIGXCPU)
    case TARGET_SIGNAL_XCPU: return SIGXCPU;
#endif
#if defined (SIGXFSZ)
    case TARGET_SIGNAL_XFSZ: return SIGXFSZ;
#endif
#if defined (SIGWIND)
    case TARGET_SIGNAL_WIND: return SIGWIND;
#endif
#if defined (SIGPHONE)
    case TARGET_SIGNAL_PHONE: return SIGPHONE;
#endif
#if defined (SIGLOST)
    case TARGET_SIGNAL_LOST: return SIGLOST;
#endif
#if defined (SIGWAITING)
    case TARGET_SIGNAL_WAITING: return SIGWAITING;
#endif
#if defined (SIGLWP)
    case TARGET_SIGNAL_LWP: return SIGLWP;
#endif
#if defined (SIGDANGER)
    case TARGET_SIGNAL_DANGER: return SIGDANGER;
#endif
#if defined (SIGGRANT)
    case TARGET_SIGNAL_GRANT: return SIGGRANT;
#endif
#if defined (SIGRETRACT)
    case TARGET_SIGNAL_RETRACT: return SIGRETRACT;
#endif
#if defined (SIGMSG)
    case TARGET_SIGNAL_MSG: return SIGMSG;
#endif
#if defined (SIGSOUND)
    case TARGET_SIGNAL_SOUND: return SIGSOUND;
#endif
#if defined (SIGSAK)
    case TARGET_SIGNAL_SAK: return SIGSAK;
#endif
    default:
      /* The user might be trying to do "signal SIGSAK" where this system
	 doesn't have SIGSAK.  */
      warning ("Signal %s does not exist on this system.\n",
	       target_signal_to_name (oursig));
      return 0;
    }
}

/* Helper function for child_wait and the Lynx derivatives of child_wait.
   HOSTSTATUS is the waitstatus from wait() or the equivalent; store our
   translation of that in OURSTATUS.  */
void
store_waitstatus (ourstatus, hoststatus)
     struct target_waitstatus *ourstatus;
     int hoststatus;
{
  if (WIFEXITED (hoststatus))
    {
      ourstatus->kind = TARGET_WAITKIND_EXITED;
      ourstatus->value.integer = WEXITSTATUS (hoststatus);
    }
  else if (!WIFSTOPPED (hoststatus))
    {
      ourstatus->kind = TARGET_WAITKIND_SIGNALLED;
      ourstatus->value.sig = target_signal_from_host (WTERMSIG (hoststatus));
    }
  else
    {
      ourstatus->kind = TARGET_WAITKIND_STOPPED;
      ourstatus->value.sig = target_signal_from_host (WSTOPSIG (hoststatus));
    }
}

#ifndef CHILD_WAIT

/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer OURSTATUS.  */

static int
child_wait (pid, ourstatus)
     int pid;
     struct target_waitstatus *ourstatus;
{
  int save_errno;
  int status;

  do {
    if (attach_flag)
      set_sigint_trap();	/* Causes SIGINT to be passed on to the
				   attached process. */
    pid = wait (&status);
    save_errno = errno;

    if (attach_flag)
      clear_sigint_trap();

    if (pid == -1)
      {
	if (save_errno == EINTR)
	  continue;
	fprintf_unfiltered (gdb_stderr, "Child process unexpectedly missing: %s.\n",
		 safe_strerror (save_errno));
	/* Claim it exited with unknown signal.  */
	ourstatus->kind = TARGET_WAITKIND_SIGNALLED;
	ourstatus->value.sig = TARGET_SIGNAL_UNKNOWN;
        return -1;
      }
  } while (pid != inferior_pid); /* Some other child died or stopped */
  store_waitstatus (ourstatus, status);
  return pid;
}
#endif /* CHILD_WAIT */

/* Attach to process PID, then initialize for debugging it.  */

static void
child_attach (args, from_tty)
     char *args;
     int from_tty;
{
  if (!args)
    error_no_arg ("process-id to attach");

#ifndef ATTACH_DETACH
  error ("Can't attach to a process on this machine.");
#else
  {
    char *exec_file;
    int pid;

    pid = atoi (args);

    if (pid == getpid())		/* Trying to masturbate? */
      error ("I refuse to debug myself!");

    if (from_tty)
      {
	exec_file = (char *) get_exec_file (0);

	if (exec_file)
	  printf_unfiltered ("Attaching to program `%s', %s\n", exec_file,
		  target_pid_to_str (pid));
	else
	  printf_unfiltered ("Attaching to %s\n", target_pid_to_str (pid));

	gdb_flush (gdb_stdout);
      }

    attach (pid);
    inferior_pid = pid;
    push_target (&child_ops);
  }
#endif  /* ATTACH_DETACH */
}


/* Take a program previously attached to and detaches it.
   The program resumes execution and will no longer stop
   on signals, etc.  We'd better not have left any breakpoints
   in the program or it'll die when it hits one.  For this
   to work, it may be necessary for the process to have been
   previously attached.  It *might* work if the program was
   started via the normal ptrace (PTRACE_TRACEME).  */

static void
child_detach (args, from_tty)
     char *args;
     int from_tty;
{
#ifdef ATTACH_DETACH
  {
    int siggnal = 0;

    if (from_tty)
      {
	char *exec_file = get_exec_file (0);
	if (exec_file == 0)
	  exec_file = "";
	printf_unfiltered ("Detaching from program: %s %s\n", exec_file,
		target_pid_to_str (inferior_pid));
	gdb_flush (gdb_stdout);
      }
    if (args)
      siggnal = atoi (args);

    detach (siggnal);
    inferior_pid = 0;
    unpush_target (&child_ops);
  }
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
  printf_unfiltered ("\tUsing the running image of %s %s.\n",
	  attach_flag? "attached": "child", target_pid_to_str (inferior_pid));
}

/* ARGSUSED */
static void
child_open (arg, from_tty)
     char *arg;
     int from_tty;
{
  error ("Use the \"run\" command to start a Unix child process.");
}

/* Stub function which causes the inferior that runs it, to be ptrace-able
   by its parent process.  */

static void
ptrace_me ()
{
  /* "Trace me, Dr. Memory!" */
  call_ptrace (0, 0, (PTRACE_ARG3_TYPE) 0, 0);
}

/* Stub function which causes the GDB that runs it, to start ptrace-ing
   the child process.  */

static void
ptrace_him (pid)
     int pid;
{
  push_target (&child_ops);

#ifdef START_INFERIOR_TRAPS_EXPECTED
  startup_inferior (START_INFERIOR_TRAPS_EXPECTED);
#else
  /* One trap to exec the shell, one to exec the program being debugged.  */
  startup_inferior (2);
#endif
}

/* Start an inferior Unix child process and sets inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().  */

static void
child_create_inferior (exec_file, allargs, env)
     char *exec_file;
     char *allargs;
     char **env;
{
  fork_inferior (exec_file, allargs, env, ptrace_me, ptrace_him);
  /* We are at the first instruction we care about.  */
  /* Pedal to the metal... */
  proceed ((CORE_ADDR) -1, TARGET_SIGNAL_0, 0);
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
  0, 				/* to_notice_signals */
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
