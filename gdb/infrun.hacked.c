/* Start and stop the inferior process, for GDB.
   Copyright (C) 1986, 1987, 1988, 1989 Free Software Foundation, Inc.

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

/* Notes on the algorithm used in wait_for_inferior to determine if we
   just did a subroutine call when stepping.  We have the following
   information at that point:

                  Current and previous (just before this step) pc.
		  Current and previous sp.
		  Current and previous start of current function.

   If the start's of the functions don't match, then

   	a) We did a subroutine call.

   In this case, the pc will be at the beginning of a function.

	b) We did a subroutine return.

   Otherwise.

	c) We did a longjmp.

   If we did a longjump, we were doing "nexti", since a next would
   have attempted to skip over the assembly language routine in which
   the longjmp is coded and would have simply been the equivalent of a
   continue.  I consider this ok behaivior.  We'd like one of two
   things to happen if we are doing a nexti through the longjmp()
   routine: 1) It behaves as a stepi, or 2) It acts like a continue as
   above.  Given that this is a special case, and that anybody who
   thinks that the concept of sub calls is meaningful in the context
   of a longjmp, I'll take either one.  Let's see what happens.  

   Acts like a subroutine return.  I can handle that with no problem
   at all.

   -->So: If the current and previous beginnings of the current
   function don't match, *and* the pc is at the start of a function,
   we've done a subroutine call.  If the pc is not at the start of a
   function, we *didn't* do a subroutine call.  

   -->If the beginnings of the current and previous function do match,
   either: 

   	a) We just did a recursive call.

	   In this case, we would be at the very beginning of a
	   function and 1) it will have a prologue (don't jump to
	   before prologue, or 2) (we assume here that it doesn't have
	   a prologue) there will have been a change in the stack
	   pointer over the last instruction.  (Ie. it's got to put
	   the saved pc somewhere.  The stack is the usual place.  In
	   a recursive call a register is only an option if there's a
	   prologue to do something with it.  This is even true on
	   register window machines; the prologue sets up the new
	   window.  It might not be true on a register window machine
	   where the call instruction moved the register window
	   itself.  Hmmm.  One would hope that the stack pointer would
	   also change.  If it doesn't, somebody send me a note, and
	   I'll work out a more general theory.
	   bug-gdb@prep.ai.mit.edu).  This is true (albeit slipperly
	   so) on all machines I'm aware of:

	      m68k:	Call changes stack pointer.  Regular jumps don't.

	      sparc:	Recursive calls must have frames and therefor,
	                prologues.

	      vax:	All calls have frames and hence change the
	                stack pointer.

	b) We did a return from a recursive call.  I don't see that we
	   have either the ability or the need to distinguish this
	   from an ordinary jump.  The stack frame will be printed
	   when and if the frame pointer changes; if we are in a
	   function without a frame pointer, it's the users own
	   lookout.

	c) We did a jump within a function.  We assume that this is
	   true if we didn't do a recursive call.

	d) We are in no-man's land ("I see no symbols here").  We
	   don't worry about this; it will make calls look like simple
	   jumps (and the stack frames will be printed when the frame
	   pointer moves), which is a reasonably non-violent response.

#if 0
    We skip this; it causes more problems than it's worth.
#ifdef SUN4_COMPILER_FEATURE
    We do a special ifdef for the sun 4, forcing it to single step
  into calls which don't have prologues.  This means that we can't
  nexti over leaf nodes, we can probably next over them (since they
  won't have debugging symbols, usually), and we can next out of
  functions returning structures (with a "call .stret4" at the end).
#endif
#endif
*/
   

   
   

#include "defs.h"
#include <string.h>
#include "symtab.h"
#include "frame.h"
#include "inferior.h"
#include "breakpoint.h"
#include "wait.h"
#include "gdbcore.h"
#include "signame.h"
#include "command.h"
#include "terminal.h"		/* For #ifdef TIOCGPGRP and new_tty */
#include "target.h"

#include <signal.h>

/* unistd.h is needed to #define X_OK */
#ifdef USG
#include <unistd.h>
#else
#include <sys/file.h>
#endif

#ifdef SET_STACK_LIMIT_HUGE
extern int original_stack_limit;
#endif /* SET_STACK_LIMIT_HUGE */

/* Required by <sys/user.h>.  */
#include <sys/types.h>
/* Required by <sys/user.h>, at least on system V.  */
#include <sys/dir.h>
/* Needed by IN_SIGTRAMP on some machines (e.g. vax).  */
#include <sys/param.h>
/* Needed by IN_SIGTRAMP on some machines (e.g. vax).  */
#include <sys/user.h>

extern int errno;
extern char *getenv ();

extern struct target_ops child_ops;	/* In inftarg.c */

/* Copy of inferior_io_terminal when inferior was last started.  */

extern char *inferior_thisrun_terminal;


/* Sigtramp is a routine that the kernel calls (which then calls the
   signal handler).  On most machines it is a library routine that
   is linked into the executable.

   This macro, given a program counter value and the name of the
   function in which that PC resides (which can be null if the
   name is not known), returns nonzero if the PC and name show
   that we are in sigtramp.

   On most machines just see if the name is sigtramp (and if we have
   no name, assume we are not in sigtramp).  */
#if !defined (IN_SIGTRAMP)
#define IN_SIGTRAMP(pc, name) \
  name && STREQ ("_sigtramp", name)
#endif

/* Tables of how to react to signals; the user sets them.  */

static char signal_stop[NSIG];
static char signal_print[NSIG];
static char signal_program[NSIG];

/* Nonzero if breakpoints are now inserted in the inferior.  */
/* Nonstatic for initialization during xxx_create_inferior. FIXME. */

/*static*/ int breakpoints_inserted;

/* Function inferior was in as of last step command.  */

static struct symbol *step_start_function;

/* Nonzero => address for special breakpoint for resuming stepping.  */

static CORE_ADDR step_resume_break_address;

/* Pointer to orig contents of the byte where the special breakpoint is.  */

static char step_resume_break_shadow[BREAKPOINT_MAX];

/* Nonzero means the special breakpoint is a duplicate
   so it has not itself been inserted.  */

static int step_resume_break_duplicate;

/* Nonzero if we are expecting a trace trap and should proceed from it.  */

static int trap_expected;

/* Nonzero if the next time we try to continue the inferior, it will
   step one instruction and generate a spurious trace trap.
   This is used to compensate for a bug in HP-UX.  */

static int trap_expected_after_continue;

/* Nonzero means expecting a trace trap
   and should stop the inferior and return silently when it happens.  */

int stop_after_trap;

/* Nonzero means expecting a trap and caller will handle it themselves.
   It is used after attach, due to attaching to a process;
   when running in the shell before the child program has been exec'd;
   and when running some kinds of remote stuff (FIXME?).  */

int stop_soon_quietly;

/* Nonzero if pc has been changed by the debugger
   since the inferior stopped.  */

int pc_changed;

/* Nonzero if proceed is being used for a "finish" command or a similar
   situation when stop_registers should be saved.  */

int proceed_to_finish;

/* Save register contents here when about to pop a stack dummy frame,
   if-and-only-if proceed_to_finish is set.
   Thus this contains the return value from the called function (assuming
   values are returned in a register).  */

char stop_registers[REGISTER_BYTES];

/* Nonzero if program stopped due to error trying to insert breakpoints.  */

static int breakpoints_failed;

/* Nonzero after stop if current stack frame should be printed.  */

static int stop_print_frame;

#ifdef NO_SINGLE_STEP
extern int one_stepped;		/* From machine dependent code */
extern void single_step ();	/* Same. */
#endif /* NO_SINGLE_STEP */

static void insert_step_breakpoint ();
static void remove_step_breakpoint ();
/*static*/ void wait_for_inferior ();
void init_wait_for_inferior ();
static void normal_stop ();


/* Clear out all variables saying what to do when inferior is continued.
   First do this, then set the ones you want, then call `proceed'.  */

void
clear_proceed_status ()
{
  trap_expected = 0;
  step_range_start = 0;
  step_range_end = 0;
  step_frame_address = 0;
  step_over_calls = -1;
  step_resume_break_address = 0;
  stop_after_trap = 0;
  stop_soon_quietly = 0;
  proceed_to_finish = 0;
  breakpoint_proceeded = 1;	/* We're about to proceed... */

  /* Discard any remaining commands or status from previous stop.  */
  bpstat_clear (&stop_bpstat);
}

/* Basic routine for continuing the program in various fashions.

   ADDR is the address to resume at, or -1 for resume where stopped.
   SIGGNAL is the signal to give it, or 0 for none,
     or -1 for act according to how it stopped.
   STEP is nonzero if should trap after one instruction.
     -1 means return after that and print nothing.
     You should probably set various step_... variables
     before calling here, if you are stepping.

   You should call clear_proceed_status before calling proceed.  */

void
proceed (addr, siggnal, step)
     CORE_ADDR addr;
     int siggnal;
     int step;
{
  int oneproc = 0;

  if (step > 0)
    step_start_function = find_pc_function (read_pc ());
  if (step < 0)
    stop_after_trap = 1;

  if (addr == -1)
    {
      /* If there is a breakpoint at the address we will resume at,
	 step one instruction before inserting breakpoints
	 so that we do not stop right away.  */

      if (!pc_changed && breakpoint_here_p (read_pc ()))
	oneproc = 1;
    }
  else
    {
      write_register (PC_REGNUM, addr);
#ifdef NPC_REGNUM
      write_register (NPC_REGNUM, addr + 4);
#ifdef NNPC_REGNUM
      write_register (NNPC_REGNUM, addr + 8);
#endif
#endif
    }

  if (trap_expected_after_continue)
    {
      /* If (step == 0), a trap will be automatically generated after
	 the first instruction is executed.  Force step one
	 instruction to clear this condition.  This should not occur
	 if step is nonzero, but it is harmless in that case.  */
      oneproc = 1;
      trap_expected_after_continue = 0;
    }

  if (oneproc)
    /* We will get a trace trap after one instruction.
       Continue it automatically and insert breakpoints then.  */
    trap_expected = 1;
  else
    {
      int temp = insert_breakpoints ();
      if (temp)
	{
	  print_sys_errmsg ("ptrace", temp);
	  error ("Cannot insert breakpoints.\n\
The same program may be running in another process.");
	}
      breakpoints_inserted = 1;
    }

  /* Install inferior's terminal modes.  */
  target_terminal_inferior ();

  if (siggnal >= 0)
    stop_signal = siggnal;
  /* If this signal should not be seen by program,
     give it zero.  Used for debugging signals.  */
  else if (stop_signal < NSIG && !signal_program[stop_signal])
    stop_signal= 0;

  /* Handle any optimized stores to the inferior NOW...  */
#ifdef DO_DEFERRED_STORES
  DO_DEFERRED_STORES;
#endif

  /* Resume inferior.  */
  target_resume (oneproc || step || bpstat_should_step (), stop_signal);

  /* Wait for it to stop (if not standalone)
     and in any case decode why it stopped, and act accordingly.  */

  wait_for_inferior ();
  normal_stop ();
}

#if 0
/* This might be useful (not sure), but isn't currently used.  See also
   write_pc().  */
/* Writing the inferior pc as a register calls this function
   to inform infrun that the pc has been set in the debugger.  */

void
writing_pc (val)
     CORE_ADDR val;
{
  stop_pc = val;
  pc_changed = 1;
}
#endif

/* Record the pc and sp of the program the last time it stopped.
   These are just used internally by wait_for_inferior, but need
   to be preserved over calls to it and cleared when the inferior
   is started.  */
static CORE_ADDR prev_pc;
static CORE_ADDR prev_sp;
static CORE_ADDR prev_func_start;
static char *prev_func_name;

/* Start an inferior Unix child process and sets inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().  */

#ifndef SHELL_FILE
#define SHELL_FILE "/bin/sh"
#endif

void
child_create_inferior (exec_file, allargs, env)
     char *exec_file;
     char *allargs;
     char **env;
{
  int pid;
  char *shell_command;
  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;
  char *shell_file;
  static char default_shell_file[] = SHELL_FILE;
  int len;
  int pending_execs;
  /* Set debug_fork then attach to the child while it sleeps, to debug. */
  static int debug_fork = 0;
  /* This is set to the result of setpgrp, which if vforked, will be visible
     to you in the parent process.  It's only used by humans for debugging.  */
  static int debug_setpgrp = 657473;

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
      debug_setpgrp = setpgrp (getpid (), getpid ());
      if (0 != debug_setpgrp)
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

      /* Tell the terminal handling subsystem what tty we plan to run on;
	 it will now switch to that one if non-null.  */

      new_tty (inferior_io_terminal);

      /* Changing the signal handlers for the inferior after
	 a vfork can also change them for the superior, so we don't mess
	 with signals here.  See comments in
	 initialize_signals for how we get the right signal handlers
	 for the inferior.  */

      call_ptrace (0, 0, 0, 0);		/* "Trace me, Dr. Memory!" */
      execle (shell_file, shell_file, "-c", shell_command, (char *)0, env);

      fprintf (stderr, "Cannot exec %s: %s.\n", shell_file,
	       errno < sys_nerr ? sys_errlist[errno] : "unknown error");
      gdb_flush (stderr);
      _exit (0177);
    }

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
	  target_resume (0, stop_signal);
	}
      else
	{
	  /* We handle SIGTRAP, however; it means child did an exec.  */
	  if (0 == --pending_execs)
	    break;
	  target_resume (0, 0);		/* Just make it go on */
	}
    }
  stop_soon_quietly = 0;

  /* Should this perhaps just be a "proceed" call?  FIXME */
  insert_step_breakpoint ();
  breakpoints_failed = insert_breakpoints ();
  if (!breakpoints_failed)
    {
      breakpoints_inserted = 1;
      target_terminal_inferior();
      /* Start the child program going on its first instruction, single-
	 stepping if we need to.  */
      target_resume (bpstat_should_step (), 0);
      wait_for_inferior ();
      normal_stop ();
    }
}

/* Start remote-debugging of a machine over a serial link.  */

void
start_remote ()
{
  init_wait_for_inferior ();
  clear_proceed_status ();
  stop_soon_quietly = 1;
  trap_expected = 0;
}

/* Initialize static vars when a new inferior begins.  */

void
init_wait_for_inferior ()
{
  /* These are meaningless until the first time through wait_for_inferior.  */
  prev_pc = 0;
  prev_sp = 0;
  prev_func_start = 0;
  prev_func_name = NULL;

  trap_expected_after_continue = 0;
  breakpoints_inserted = 0;
  mark_breakpoints_out ();
}


/* Attach to process PID, then initialize for debugging it
   and wait for the trace-trap that results from attaching.  */

void
child_open (args, from_tty)
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

  if (target_has_execution)
    {
      if (query ("A program is being debugged already.  Kill it? "))
	target_kill ((char *)0, from_tty);
      else
	error ("Inferior not killed.");
    }

  exec_file = (char *) get_exec_file (1);

  if (from_tty)
    {
      printf ("Attaching program: %s pid %d\n",
	      exec_file, pid);
      gdb_flush (stdout);
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
  normal_stop ();
#endif  /* ATTACH_DETACH */
}

/* Wait for control to return from inferior to debugger.
   If inferior gets a signal, we may decide to start it up again
   instead of returning.  That is why there is a loop in this function.
   When this function actually returns it means the inferior
   should be left stopped and GDB should read more commands.  */

void
wait_for_inferior ()
{
  WAITTYPE w;
  int another_trap;
  int random_signal;
  CORE_ADDR stop_sp;
  CORE_ADDR stop_func_start;
  char *stop_func_name;
  CORE_ADDR prologue_pc;
  int stop_step_resume_break;
  struct symtab_and_line sal;
  int remove_breakpoints_on_following_step = 0;

#if 0
  /* This no longer works now that read_register is lazy;
     it might try to ptrace when the process is not stopped.  */
  prev_pc = read_pc ();
  (void) find_pc_partial_function (prev_pc, &prev_func_name,
				   &prev_func_start);
  prev_func_start += FUNCTION_START_OFFSET;
  prev_sp = read_register (SP_REGNUM);
#endif /* 0 */

  while (1)
    {
      /* Clean up saved state that will become invalid.  */
      pc_changed = 0;
      flush_cached_frames ();
      registers_changed ();

      target_wait (&w);

      /* See if the process still exists; clean up if it doesn't.  */
      if (WIFEXITED (w))
	{
	  target_terminal_ours_for_output ();
	  if (WEXITSTATUS (w))
	    printf ("\nProgram exited with code 0%o.\n", 
		     (unsigned int)WEXITSTATUS (w));
	  else
	    if (!batch_mode())
	      printf ("\nProgram exited normally.\n");
	  gdb_flush (stdout);
	  target_mourn_inferior ();
#ifdef NO_SINGLE_STEP
	  one_stepped = 0;
#endif
	  stop_print_frame = 0;
	  break;
	}
      else if (!WIFSTOPPED (w))
	{
	  target_kill ((char *)0, 0);
	  stop_print_frame = 0;
	  stop_signal = WTERMSIG (w);
	  target_terminal_ours_for_output ();
	  printf ("\nProgram terminated with signal %d, %s\n",
		  stop_signal,
		  stop_signal < NSIG
		  ? sys_siglist[stop_signal]
		  : "(undocumented)");
	  printf ("The inferior process no longer exists.\n");
	  gdb_flush (stdout);
#ifdef NO_SINGLE_STEP
	  one_stepped = 0;
#endif
	  break;
	}
      
#ifdef NO_SINGLE_STEP
      if (one_stepped)
	single_step (0);	/* This actually cleans up the ss */
#endif /* NO_SINGLE_STEP */
      
      stop_pc = read_pc ();
      set_current_frame ( create_new_frame (read_register (FP_REGNUM),
					    read_pc ()));
      
      stop_frame_address = FRAME_FP (get_current_frame ());
      stop_sp = read_register (SP_REGNUM);
      stop_func_start = 0;
      stop_func_name = 0;
      /* Don't care about return value; stop_func_start and stop_func_name
	 will both be 0 if it doesn't work.  */
      (void) find_pc_partial_function (stop_pc, &stop_func_name,
				       &stop_func_start);
      stop_func_start += FUNCTION_START_OFFSET;
      another_trap = 0;
      bpstat_clear (&stop_bpstat);
      stop_step = 0;
      stop_stack_dummy = 0;
      stop_print_frame = 1;
      stop_step_resume_break = 0;
      random_signal = 0;
      stopped_by_random_signal = 0;
      breakpoints_failed = 0;
      
      /* Look at the cause of the stop, and decide what to do.
	 The alternatives are:
	 1) break; to really stop and return to the debugger,
	 2) drop through to start up again
	 (set another_trap to 1 to single step once)
	 3) set random_signal to 1, and the decision between 1 and 2
	 will be made according to the signal handling tables.  */
      
      stop_signal = WSTOPSIG (w);
      
      /* First, distinguish signals caused by the debugger from signals
	 that have to do with the program's own actions.
	 Note that breakpoint insns may cause SIGTRAP or SIGILL
	 or SIGEMT, depending on the operating system version.
	 Here we detect when a SIGILL or SIGEMT is really a breakpoint
	 and change it to SIGTRAP.  */
      
      if (stop_signal == SIGTRAP
	  || (breakpoints_inserted &&
	      (stop_signal == SIGILL
	       || stop_signal == SIGEMT))
	  || stop_soon_quietly)
	{
	  if (stop_signal == SIGTRAP && stop_after_trap)
	    {
	      stop_print_frame = 0;
	      break;
	    }
	  if (stop_soon_quietly)
	    break;

	  /* Don't even think about breakpoints
	     if just proceeded over a breakpoint.

	     However, if we are trying to proceed over a breakpoint
	     and end up in sigtramp, then step_resume_break_address
	     will be set and we should check whether we've hit the
	     step breakpoint.  */
	  if (stop_signal == SIGTRAP && trap_expected
	      && step_resume_break_address == NULL)
	    bpstat_clear (&stop_bpstat);
	  else
	    {
	      /* See if there is a breakpoint at the current PC.  */
#if DECR_PC_AFTER_BREAK
	      /* Notice the case of stepping through a jump
		 that leads just after a breakpoint.
		 Don't confuse that with hitting the breakpoint.
		 What we check for is that 1) stepping is going on
		 and 2) the pc before the last insn does not match
		 the address of the breakpoint before the current pc.  */
	      if (!(prev_pc != stop_pc - DECR_PC_AFTER_BREAK
		    && step_range_end && !step_resume_break_address))
#endif /* DECR_PC_AFTER_BREAK not zero */
		{
		  /* See if we stopped at the special breakpoint for
		     stepping over a subroutine call.  */
		  if (stop_pc - DECR_PC_AFTER_BREAK
		      == step_resume_break_address)
		    {
		      stop_step_resume_break = 1;
		      if (DECR_PC_AFTER_BREAK)
			{
			  stop_pc -= DECR_PC_AFTER_BREAK;
			  write_register (PC_REGNUM, stop_pc);
			  pc_changed = 0;
			}
		    }
		  else
		    {
		      stop_bpstat =
			bpstat_stop_status (&stop_pc, stop_frame_address);
		      /* Following in case break condition called a
			 function.  */
		      stop_print_frame = 1;
		    }
		}
	    }
	  
	  if (stop_signal == SIGTRAP)
	    random_signal
	      = !(bpstat_explains_signal (stop_bpstat)
		  || trap_expected
		  || stop_step_resume_break
		  || PC_IN_CALL_DUMMY (stop_pc, stop_sp, stop_frame_address)
		  || (step_range_end && !step_resume_break_address));
	  else
	    {
	      random_signal
		= !(bpstat_explains_signal (stop_bpstat)
		    || stop_step_resume_break
		    /* End of a stack dummy.  Some systems (e.g. Sony
		       news) give another signal besides SIGTRAP,
		       so check here as well as above.  */
		    || (stop_sp INNER_THAN stop_pc
			&& stop_pc INNER_THAN stop_frame_address)
		    );
	      if (!random_signal)
		stop_signal = SIGTRAP;
	    }
	}
      else
	random_signal = 1;
      
      /* For the program's own signals, act according to
	 the signal handling tables.  */
      
      if (random_signal)
	{
	  /* Signal not for debugging purposes.  */
	  int printed = 0;
	  
	  stopped_by_random_signal = 1;
	  
	  if (stop_signal >= NSIG
	      || signal_print[stop_signal])
	    {
	      printed = 1;
	      target_terminal_ours_for_output ();
#ifdef PRINT_RANDOM_SIGNAL
	      PRINT_RANDOM_SIGNAL (stop_signal);
#else
	      printf ("\nProgram received signal %d, %s\n",
		      stop_signal,
		      stop_signal < NSIG
		      ? sys_siglist[stop_signal]
		      : "(undocumented)");
#endif /* PRINT_RANDOM_SIGNAL */
	      gdb_flush (stdout);
	    }
	  if (stop_signal >= NSIG
	      || signal_stop[stop_signal])
	    break;
	  /* If not going to stop, give terminal back
	     if we took it away.  */
	  else if (printed)
	    target_terminal_inferior ();
	}
      
      /* Handle cases caused by hitting a user breakpoint.  */
      
      if (!random_signal && bpstat_explains_signal (stop_bpstat))
      {
	  /* Does a breakpoint want us to stop?  */
	  if (bpstat_stop (stop_bpstat))
	    {
	      stop_print_frame = bpstat_should_print (stop_bpstat);
	      break;
	    }

	  /* Otherwise we continue.  Must remove breakpoints and single-step
	     to get us past the one we hit.  Possibly we also were stepping
	     and should stop for that.  So fall through and
	     test for stepping.  But, if not stepping,
	     do not stop.  */
	  else
	    {
	      remove_breakpoints ();
	      remove_step_breakpoint (); /* FIXME someday, do we need this? */
	      breakpoints_inserted = 0;
	      another_trap = 1;
	    }
	}

      /* Handle cases caused by hitting a step-resumption breakpoint.  */
      
      else if (!random_signal && stop_step_resume_break)
	{
	  /* We have hit the step-resumption breakpoint.
	     If we aren't in a recursive call that hit it again
	     before returning from the original call, remove it;
	     it has done its job getting us here.  We then resume
	     the stepping we were doing before the function call.

	     If we are in a recursive call, just proceed from this
	     breakpoint as usual, keeping it around to catch the final
	     return of interest.

	     There used to be an sp test to make sure that we don't get hung
	     up in recursive calls in functions without frame
	     pointers.  If the stack pointer isn't outside of
	     where the breakpoint was set (within a routine to be
	     stepped over), we're in the middle of a recursive
	     call. Not true for reg window machines (sparc)
	     because they must change frames to call things and
	     the stack pointer doesn't have to change if
	     the bp was set in a routine without a frame (pc can
	     be stored in some other window).

	     The removal of the sp test is to allow calls to
	     alloca.  Nasty things were happening.  Oh, well,
	     gdb can only handle one level deep of lack of
	     frame pointer. */
	  if (step_frame_address == 0
	      || (stop_frame_address == step_frame_address))
	    {
	      /* We really hit it:  not a recursive call.  */
	      remove_step_breakpoint ();
	      step_resume_break_address = 0;

	      /* If we're waiting for a trap, hitting the step_resume_break
		 doesn't count as getting it.  */
	      if (trap_expected)
		another_trap = 1;
	      /* Fall through to resume stepping... */
	    }
	  else
	    {
	      /* Otherwise, it's the recursive call case.  */
	      remove_breakpoints ();
	      remove_step_breakpoint ();
	      breakpoints_inserted = 0;
	      another_trap = 1;
	      /* Fall through to continue executing at full speed 
		 (with a possible single-step lurch over the step-resumption
		  breakpoint as we start.)  */
	    }
	}
      
      /* If this is the breakpoint at the end of a stack dummy,
	 just stop silently.  */
      if (PC_IN_CALL_DUMMY (stop_pc, stop_sp, stop_frame_address))
	  {
	    stop_print_frame = 0;
	    stop_stack_dummy = 1;
#ifdef HP_OS_BUG
	    trap_expected_after_continue = 1;
#endif
	    break;
	  }
      
      if (step_resume_break_address)
	/* Having a step-resume breakpoint overrides anything
	   else having to do with stepping commands until
	   that breakpoint is reached.  */
	;
      /* If stepping through a line, keep going if still within it.  */
      else if (!random_signal
	       && step_range_end
	       && stop_pc >= step_range_start
	       && stop_pc < step_range_end
	       /* The step range might include the start of the
		  function, so if we are at the start of the
		  step range and either the stack or frame pointers
		  just changed, we've stepped outside */
	       && !(stop_pc == step_range_start
		    && stop_frame_address
		    && (stop_sp INNER_THAN prev_sp
			|| stop_frame_address != step_frame_address)))
	{
#if 0
	  /* When "next"ing through a function,
	     This causes an extra stop at the end.
	     Is there any reason for this?
	     It's confusing to the user.  */
	  /* Don't step through the return from a function
	     unless that is the first instruction stepped through.  */
	  if (ABOUT_TO_RETURN (stop_pc))
	    {
	      stop_step = 1;
	      break;
	    }
#endif
	}
      
      /* We stepped out of the stepping range.  See if that was due
	 to a subroutine call that we should proceed to the end of.  */
      else if (!random_signal && step_range_end)
	{
	  if (stop_func_start)
	    {
	      prologue_pc = stop_func_start;
	      SKIP_PROLOGUE (prologue_pc);
	    }

	  /* Did we just take a signal?  */
	  if (IN_SIGTRAMP (stop_pc, stop_func_name)
	      && !IN_SIGTRAMP (prev_pc, prev_func_name))
	    {
	      /* This code is needed at least in the following case:
		 The user types "next" and then a signal arrives (before
		 the "next" is done).  */
	      /* We've just taken a signal; go until we are back to
		 the point where we took it and one more.  */
	      step_resume_break_address = prev_pc;
	      step_resume_break_duplicate =
		breakpoint_here_p (step_resume_break_address);
	      if (breakpoints_inserted)
		insert_step_breakpoint ();
	      /* Make sure that the stepping range gets us past
		 that instruction.  */
	      if (step_range_end == 1)
		step_range_end = (step_range_start = prev_pc) + 1;
	      remove_breakpoints_on_following_step = 1;
	    }

	  /* ==> See comments at top of file on this algorithm.  <==*/
	  
	  else if (stop_pc == stop_func_start
	      && (stop_func_start != prev_func_start
		  || prologue_pc != stop_func_start
		  || stop_sp != prev_sp))
	    {
	      /* It's a subroutine call */
	      if (step_over_calls > 0 
		  || (step_over_calls &&  find_pc_function (stop_pc) == 0))
		{
		  /* A subroutine call has happened.  */
		  /* Set a special breakpoint after the return */
		  step_resume_break_address =
		    ADDR_BITS_REMOVE
		      (SAVED_PC_AFTER_CALL (get_current_frame ()));
		  step_resume_break_duplicate
		    = breakpoint_here_p (step_resume_break_address);
		  if (breakpoints_inserted)
		    insert_step_breakpoint ();
		}
	      /* Subroutine call with source code we should not step over.
		 Do step to the first line of code in it.  */
	      else if (step_over_calls)
		{
		  SKIP_PROLOGUE (stop_func_start);
		  sal = find_pc_line (stop_func_start, 0);
		  /* Use the step_resume_break to step until
		     the end of the prologue, even if that involves jumps
		     (as it seems to on the vax under 4.2).  */
		  /* If the prologue ends in the middle of a source line,
		     continue to the end of that source line.
		     Otherwise, just go to end of prologue.  */
#ifdef PROLOGUE_FIRSTLINE_OVERLAP
		  /* no, don't either.  It skips any code that's
		     legitimately on the first line.  */
#else
		  if (sal.end && sal.pc != stop_func_start)
		    stop_func_start = sal.end;
#endif
		  
		  if (stop_func_start == stop_pc)
		    {
		      /* We are already there: stop now.  */
		      stop_step = 1;
		      break;
		    }
		  else
		    /* Put the step-breakpoint there and go until there. */
		    {
		      step_resume_break_address = stop_func_start;
		      
		      step_resume_break_duplicate
			= breakpoint_here_p (step_resume_break_address);
		      if (breakpoints_inserted)
			insert_step_breakpoint ();
		      /* Do not specify what the fp should be when we stop
			 since on some machines the prologue
			 is where the new fp value is established.  */
		      step_frame_address = 0;
		      /* And make sure stepping stops right away then.  */
		      step_range_end = step_range_start;
		    }
		}
	      else
		{
		  /* We get here only if step_over_calls is 0 and we
		     just stepped into a subroutine.  I presume
		     that step_over_calls is only 0 when we're
		     supposed to be stepping at the assembly
		     language level.*/
		  stop_step = 1;
		  break;
		}
	    }
	  /* No subroutine call; stop now.  */
	  else
	    {
	      stop_step = 1;
	      break;
	    }
	}

      else if (trap_expected
	       && IN_SIGTRAMP (stop_pc, stop_func_name)
	       && !IN_SIGTRAMP (prev_pc, prev_func_name))
	{
	  /* What has happened here is that we have just stepped the inferior
	     with a signal (because it is a signal which shouldn't make
	     us stop), thus stepping into sigtramp.

	     So we need to set a step_resume_break_address breakpoint
	     and continue until we hit it, and then step.  */
	  step_resume_break_address = prev_pc;
	  /* Always 1, I think, but it's probably easier to have
	     the step_resume_break as usual rather than trying to
	     re-use the breakpoint which is already there.  */
	  step_resume_break_duplicate =
	    breakpoint_here_p (step_resume_break_address);
	  if (breakpoints_inserted)
	    insert_step_breakpoint ();
	  remove_breakpoints_on_following_step = 1;
	  another_trap = 1;
	}

      /* Save the pc before execution, to compare with pc after stop.  */
      prev_pc = read_pc ();	/* Might have been DECR_AFTER_BREAK */
      prev_func_start = stop_func_start; /* Ok, since if DECR_PC_AFTER
					  BREAK is defined, the
					  original pc would not have
					  been at the start of a
					  function. */
      prev_func_name = stop_func_name;
      prev_sp = stop_sp;

      /* If we did not do break;, it means we should keep
	 running the inferior and not return to debugger.  */

      if (trap_expected && stop_signal != SIGTRAP)
	{
	  /* We took a signal (which we are supposed to pass through to
	     the inferior, else we'd have done a break above) and we
	     haven't yet gotten our trap.  Simply continue.  */
	  target_resume ((step_range_end && !step_resume_break_address)
		  || (trap_expected && !step_resume_break_address)
		  || bpstat_should_step (),
		  stop_signal);
	}
      else
	{
	  /* Either the trap was not expected, but we are continuing
	     anyway (the user asked that this signal be passed to the
	     child)
	       -- or --
	     The signal was SIGTRAP, e.g. it was our signal, but we
	     decided we should resume from it.

	     We're going to run this baby now!

	     Insert breakpoints now, unless we are trying
	     to one-proceed past a breakpoint.  */
	  /* If we've just finished a special step resume and we don't
	     want to hit a breakpoint, pull em out.  */
	  if (!step_resume_break_address &&
	      remove_breakpoints_on_following_step)
	    {
	      remove_breakpoints_on_following_step = 0;
	      remove_breakpoints ();
	      breakpoints_inserted = 0;
	    }
	  else if (!breakpoints_inserted &&
		   (step_resume_break_address != NULL || !another_trap))
	    {
	      insert_step_breakpoint ();
	      breakpoints_failed = insert_breakpoints ();
	      if (breakpoints_failed)
		break;
	      breakpoints_inserted = 1;
	    }

	  trap_expected = another_trap;

	  if (stop_signal == SIGTRAP)
	    stop_signal = 0;

#ifdef SHIFT_INST_REGS
	  /* I'm not sure when this following segment applies.  I do know, now,
	     that we shouldn't rewrite the regs when we were stopped by a
	     random signal from the inferior process.  */

          if (!stop_breakpoint && (stop_signal != SIGCLD) 
              && !stopped_by_random_signal)
            {
            CORE_ADDR pc_contents = read_register (PC_REGNUM);
            CORE_ADDR npc_contents = read_register (NPC_REGNUM);
            if (pc_contents != npc_contents)
              {
              write_register (NNPC_REGNUM, npc_contents);
              write_register (NPC_REGNUM, pc_contents);
	      }
            }
#endif /* SHIFT_INST_REGS */

	  target_resume ((step_range_end && !step_resume_break_address)
		  || (trap_expected && !step_resume_break_address)
		  || bpstat_should_step (),
		  stop_signal);
	}
    }
  if (target_has_execution)
    {
      /* Assuming the inferior still exists, set these up for next
	 time, just like we did above if we didn't break out of the
	 loop.  */
      prev_pc = read_pc ();
      prev_func_start = stop_func_start;
      prev_func_name = stop_func_name;
      prev_sp = stop_sp;
    }
}

/* Here to return control to GDB when the inferior stops for real.
   Print appropriate messages, remove breakpoints, give terminal our modes.

   STOP_PRINT_FRAME nonzero means print the executing frame
   (pc, function, args, file, line number and line text).
   BREAKPOINTS_FAILED nonzero means stop was due to error
   attempting to insert breakpoints.  */

static void
normal_stop ()
{
  /* Make sure that the current_frame's pc is correct.  This
     is a correction for setting up the frame info before doing
     DECR_PC_AFTER_BREAK */
  if (target_has_execution)
    (get_current_frame ())->pc = read_pc ();
  
  if (breakpoints_failed)
    {
      target_terminal_ours_for_output ();
      print_sys_errmsg ("ptrace", breakpoints_failed);
      printf ("Stopped; cannot insert breakpoints.\n\
The same program may be running in another process.\n");
    }

  if (target_has_execution)
    remove_step_breakpoint ();

  if (target_has_execution && breakpoints_inserted)
    if (remove_breakpoints ())
      {
	target_terminal_ours_for_output ();
	printf ("Cannot remove breakpoints because program is no longer writable.\n\
It must be running in another process.\n\
Further execution is probably impossible.\n");
      }

  breakpoints_inserted = 0;

  /* Delete the breakpoint we stopped at, if it wants to be deleted.
     Delete any breakpoint that is to be deleted at the next stop.  */

  breakpoint_auto_delete (stop_bpstat);

  /* If an auto-display called a function and that got a signal,
     delete that auto-display to avoid an infinite recursion.  */

  if (stopped_by_random_signal)
    disable_current_display ();

  if (step_multi && stop_step)
    return;

  target_terminal_ours ();

  if (!target_has_stack)
    return;

  /* Select innermost stack frame except on return from a stack dummy routine,
     or if the program has exited.  */
  if (!stop_stack_dummy)
    {
      select_frame (get_current_frame (), 0);

      if (stop_print_frame)
	{
	  int source_only = bpstat_print (stop_bpstat);
	  print_sel_frame
	    (source_only
	     || (stop_step
		 && step_frame_address == stop_frame_address
		 && step_start_function == find_pc_function (stop_pc)));

	  /* Display the auto-display expressions.  */
	  do_displays ();
	}
    }

  /* Save the function value return registers, if we care.
     We might be about to restore their previous contents.  */
  if (proceed_to_finish)
    read_register_bytes (0, stop_registers, REGISTER_BYTES);

  if (stop_stack_dummy)
    {
      /* Pop the empty frame that contains the stack dummy.
         POP_FRAME ends with a setting of the current frame, so we
	 can use that next. */
      POP_FRAME;
      select_frame (get_current_frame (), 0);
    }
}

static void
insert_step_breakpoint ()
{
  if (step_resume_break_address && !step_resume_break_duplicate)
    target_insert_breakpoint (step_resume_break_address,
			      step_resume_break_shadow);
}

static void
remove_step_breakpoint ()
{
  if (step_resume_break_address && !step_resume_break_duplicate)
    target_remove_breakpoint (step_resume_break_address,
			      step_resume_break_shadow);
}

static void
sig_print_header ()
{
  printf_filtered ("Signal\t\tStop\tPrint\tPass to program\tDescription\n");
}

static void
sig_print_info (number)
     int number;
{
  char *abbrev = sig_abbrev(number);
  if (abbrev == NULL)
    printf_filtered ("%d\t\t", number);
  else
    printf_filtered ("SIG%s (%d)\t", abbrev, number);
  printf_filtered ("%s\t", signal_stop[number] ? "Yes" : "No");
  printf_filtered ("%s\t", signal_print[number] ? "Yes" : "No");
  printf_filtered ("%s\t\t", signal_program[number] ? "Yes" : "No");
  printf_filtered ("%s\n", sys_siglist[number]);
}

/* Specify how various signals in the inferior should be handled.  */

static void
handle_command (args, from_tty)
     char *args;
     int from_tty;
{
  register char *p = args;
  int signum = 0;
  register int digits, wordlen;
  char *nextarg;

  if (!args)
    error_no_arg ("signal to handle");

  while (*p)
    {
      /* Find the end of the next word in the args.  */
      for (wordlen = 0;
	   p[wordlen] && p[wordlen] != ' ' && p[wordlen] != '\t';
	   wordlen++);
      /* Set nextarg to the start of the word after the one we just
	 found, and null-terminate this one.  */
      if (p[wordlen] == '\0')
	nextarg = p + wordlen;
      else
	{
	  p[wordlen] = '\0';
	  nextarg = p + wordlen + 1;
	}
      

      for (digits = 0; p[digits] >= '0' && p[digits] <= '9'; digits++);

      if (signum == 0)
	{
	  /* It is the first argument--must be the signal to operate on.  */
	  if (digits == wordlen)
	    {
	      /* Numeric.  */
	      signum = atoi (p);
	      if (signum <= 0 || signum >= NSIG)
		{
		  p[wordlen] = '\0';
		  error ("Invalid signal %s given as argument to \"handle\".", p);
		}
	    }
	  else
	    {
	      /* Symbolic.  */
	      signum = sig_number (p);
	      if (signum == -1)
		error ("No such signal \"%s\"", p);
	    }

	  if (signum == SIGTRAP || signum == SIGINT)
	    {
	      if (!query ("SIG%s is used by the debugger.\nAre you sure you want to change it? ", sig_abbrev (signum)))
		error ("Not confirmed.");
	    }
	}
      /* Else, if already got a signal number, look for flag words
	 saying what to do for it.  */
      else if (!strncmp (p, "stop", wordlen))
	{
	  signal_stop[signum] = 1;
	  signal_print[signum] = 1;
	}
      else if (wordlen >= 2 && !strncmp (p, "print", wordlen))
	signal_print[signum] = 1;
      else if (wordlen >= 2 && !strncmp (p, "pass", wordlen))
	signal_program[signum] = 1;
      else if (!strncmp (p, "ignore", wordlen))
	signal_program[signum] = 0;
      else if (wordlen >= 3 && !strncmp (p, "nostop", wordlen))
	signal_stop[signum] = 0;
      else if (wordlen >= 4 && !strncmp (p, "noprint", wordlen))
	{
	  signal_print[signum] = 0;
	  signal_stop[signum] = 0;
	}
      else if (wordlen >= 4 && !strncmp (p, "nopass", wordlen))
	signal_program[signum] = 0;
      else if (wordlen >= 3 && !strncmp (p, "noignore", wordlen))
	signal_program[signum] = 1;
      /* Not a number and not a recognized flag word => complain.  */
      else
	{
	  error ("Unrecognized flag word: \"%s\".", p);
	}

      /* Find start of next word.  */
      p = nextarg;
      while (*p == ' ' || *p == '\t') p++;
    }

  if (from_tty)
    {
      /* Show the results.  */
      sig_print_header ();
      sig_print_info (signum);
    }
}

/* Print current contents of the tables set by the handle command.  */

static void
signals_info (signum_exp)
     char *signum_exp;
{
  register int i;
  sig_print_header ();

  if (signum_exp)
    {
      /* First see if this is a symbol name.  */
      i = sig_number (signum_exp);
      if (i == -1)
	{
	  /* Nope, maybe it's an address which evaluates to a signal
	     number.  */
	  i = parse_and_eval_address (signum_exp);
	  if (i >= NSIG || i < 0)
	    error ("Signal number out of bounds.");
	}
      sig_print_info (i);
      return;
    }

  printf_filtered ("\n");
  for (i = 0; i < NSIG; i++)
    {
      QUIT;

      sig_print_info (i);
    }

  printf_filtered ("\nUse the \"handle\" command to change these tables.\n");
}

/* Save all of the information associated with the inferior<==>gdb
   connection.  INF_STATUS is a pointer to a "struct inferior_status"
   (defined in inferior.h).  */

void
save_inferior_status (inf_status, restore_stack_info)
     struct inferior_status *inf_status;
     int restore_stack_info;
{
  inf_status->pc_changed = pc_changed;
  inf_status->stop_signal = stop_signal;
  inf_status->stop_pc = stop_pc;
  inf_status->stop_frame_address = stop_frame_address;
  inf_status->stop_step = stop_step;
  inf_status->stop_stack_dummy = stop_stack_dummy;
  inf_status->stopped_by_random_signal = stopped_by_random_signal;
  inf_status->trap_expected = trap_expected;
  inf_status->step_range_start = step_range_start;
  inf_status->step_range_end = step_range_end;
  inf_status->step_frame_address = step_frame_address;
  inf_status->step_over_calls = step_over_calls;
  inf_status->step_resume_break_address = step_resume_break_address;
  inf_status->stop_after_trap = stop_after_trap;
  inf_status->stop_soon_quietly = stop_soon_quietly;
  /* Save original bpstat chain here; replace it with copy of chain. 
     If caller's caller is walking the chain, they'll be happier if we
     hand them back the original chain when restore_i_s is called.  */
  inf_status->stop_bpstat = stop_bpstat;
  stop_bpstat = bpstat_copy (stop_bpstat);
  inf_status->breakpoint_proceeded = breakpoint_proceeded;
  inf_status->restore_stack_info = restore_stack_info;
  inf_status->proceed_to_finish = proceed_to_finish;
  
  bcopy (stop_registers, inf_status->stop_registers, REGISTER_BYTES);
  
  record_selected_frame (&(inf_status->selected_frame_address),
			 &(inf_status->selected_level));
  return;
}

void
restore_inferior_status (inf_status)
     struct inferior_status *inf_status;
{
  FRAME fid;
  int level = inf_status->selected_level;

  pc_changed = inf_status->pc_changed;
  stop_signal = inf_status->stop_signal;
  stop_pc = inf_status->stop_pc;
  stop_frame_address = inf_status->stop_frame_address;
  stop_step = inf_status->stop_step;
  stop_stack_dummy = inf_status->stop_stack_dummy;
  stopped_by_random_signal = inf_status->stopped_by_random_signal;
  trap_expected = inf_status->trap_expected;
  step_range_start = inf_status->step_range_start;
  step_range_end = inf_status->step_range_end;
  step_frame_address = inf_status->step_frame_address;
  step_over_calls = inf_status->step_over_calls;
  step_resume_break_address = inf_status->step_resume_break_address;
  stop_after_trap = inf_status->stop_after_trap;
  stop_soon_quietly = inf_status->stop_soon_quietly;
  bpstat_clear (&stop_bpstat);
  stop_bpstat = inf_status->stop_bpstat;
  breakpoint_proceeded = inf_status->breakpoint_proceeded;
  proceed_to_finish = inf_status->proceed_to_finish;

  bcopy (inf_status->stop_registers, stop_registers, REGISTER_BYTES);

  /* The inferior can be gone if the user types "print exit(0)"
     (and perhaps other times).  */
  if (target_has_stack && inf_status->restore_stack_info)
    {
      fid = find_relative_frame (get_current_frame (),
				 &level);

      if (fid == 0 ||
	  FRAME_FP (fid) != inf_status->selected_frame_address ||
	  level != 0)
	{
#if 0
	  /* I'm not sure this error message is a good idea.  I have
	     only seen it occur after "Can't continue previously
	     requested operation" (we get called from do_cleanups), in
	     which case it just adds insult to injury (one confusing
	     error message after another.  Besides which, does the
	     user really care if we can't restore the previously
	     selected frame?  */
	  fprintf (stderr, "Unable to restore previously selected frame.\n");
#endif
	  select_frame (get_current_frame (), 0);
	  return;
	}
      
      select_frame (fid, inf_status->selected_level);
    }
}


void
_initialize_infrun ()
{
  register int i;

  add_info ("signals", signals_info,
	    "What debugger does when program gets various signals.\n\
Specify a signal number as argument to print info on that signal only.");

  add_com ("handle", class_run, handle_command,
	   "Specify how to handle a signal.\n\
Args are signal number followed by flags.\n\
Flags allowed are \"stop\", \"print\", \"pass\",\n\
 \"nostop\", \"noprint\" or \"nopass\".\n\
Print means print a message if this signal happens.\n\
Stop means reenter debugger if this signal happens (implies print).\n\
Pass means let program see this signal; otherwise program doesn't know.\n\
Pass and Stop may be combined.");

  for (i = 0; i < NSIG; i++)
    {
      signal_stop[i] = 1;
      signal_print[i] = 1;
      signal_program[i] = 1;
    }

  /* Signals caused by debugger's own actions
     should not be given to the program afterwards.  */
  signal_program[SIGTRAP] = 0;
  signal_program[SIGINT] = 0;

  /* Signals that are not errors should not normally enter the debugger.  */
#ifdef SIGALRM
  signal_stop[SIGALRM] = 0;
  signal_print[SIGALRM] = 0;
#endif /* SIGALRM */
#ifdef SIGVTALRM
  signal_stop[SIGVTALRM] = 0;
  signal_print[SIGVTALRM] = 0;
#endif /* SIGVTALRM */
#ifdef SIGPROF
  signal_stop[SIGPROF] = 0;
  signal_print[SIGPROF] = 0;
#endif /* SIGPROF */
#ifdef SIGCHLD
  signal_stop[SIGCHLD] = 0;
  signal_print[SIGCHLD] = 0;
#endif /* SIGCHLD */
#ifdef SIGCLD
  signal_stop[SIGCLD] = 0;
  signal_print[SIGCLD] = 0;
#endif /* SIGCLD */
#ifdef SIGIO
  signal_stop[SIGIO] = 0;
  signal_print[SIGIO] = 0;
#endif /* SIGIO */
#ifdef SIGURG
  signal_stop[SIGURG] = 0;
  signal_print[SIGURG] = 0;
#endif /* SIGURG */
}

