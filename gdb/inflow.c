/* Low level interface to ptrace, for GDB when running under Unix.
   Copyright (C) 1986, 1987, 1989, 1991 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "command.h"
#include "signals.h"
#include "terminal.h"
#include "target.h"

#ifdef USG
#include <sys/types.h>
#endif

/* Some USG-esque systems (some of which are BSD-esque enough so that USG
   is not defined) want this header, and it won't do any harm.  */
#include <fcntl.h>

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>

extern struct target_ops child_ops;

/* Nonzero if we are debugging an attached outside process
   rather than an inferior.  */

int attach_flag;


/* Record terminal status separately for debugger and inferior.  */

static TERMINAL sg_inferior;
static TERMINAL sg_ours;

static int tflags_inferior;
static int tflags_ours;

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
static struct tchars tc_inferior;
static struct tchars tc_ours;
#endif

#ifdef TIOCGLTC
static struct ltchars ltc_inferior;
static struct ltchars ltc_ours;
#endif

#ifdef TIOCLGET
static int lmode_inferior;
static int lmode_ours;
#endif

#ifdef TIOCGPGRP
static int pgrp_inferior;
static int pgrp_ours;
#else
static void (*sigint_ours) ();
static void (*sigquit_ours) ();
#endif /* TIOCGPGRP */

/* Copy of inferior_io_terminal when inferior was last started.  */
static char *inferior_thisrun_terminal;

static void terminal_ours_1 ();

/* Nonzero if our terminal settings are in effect.
   Zero if the inferior's settings are in effect.  */
static int terminal_is_ours;

/* Initialize the terminal settings we record for the inferior,
   before we actually run the inferior.  */

void
terminal_init_inferior ()
{
  sg_inferior = sg_ours;
  tflags_inferior = tflags_ours;

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
  tc_inferior = tc_ours;
#endif

#ifdef TIOCGLTC
  ltc_inferior = ltc_ours;
#endif

#ifdef TIOCLGET
  lmode_inferior = lmode_ours;
#endif

#ifdef TIOCGPGRP
  pgrp_inferior = inferior_pid;
#endif /* TIOCGPGRP */

  terminal_is_ours = 1;
}

/* Put the inferior's terminal settings into effect.
   This is preparation for starting or resuming the inferior.  */

void
terminal_inferior ()
{
  if (terminal_is_ours && inferior_thisrun_terminal == 0)
    {
      fcntl (0, F_SETFL, tflags_inferior);
      fcntl (0, F_SETFL, tflags_inferior);
      ioctl (0, TIOCSETN, &sg_inferior);

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
      ioctl (0, TIOCSETC, &tc_inferior);
#endif
#ifdef TIOCGLTC
      ioctl (0, TIOCSLTC, &ltc_inferior);
#endif
#ifdef TIOCLGET
      ioctl (0, TIOCLSET, &lmode_inferior);
#endif

#ifdef TIOCGPGRP
      ioctl (0, TIOCSPGRP, &pgrp_inferior);
#else
      sigint_ours = (void (*) ()) signal (SIGINT, SIG_IGN);
      sigquit_ours = (void (*) ()) signal (SIGQUIT, SIG_IGN);
#endif /* TIOCGPGRP */
    }
  terminal_is_ours = 0;
}

/* Put some of our terminal settings into effect,
   enough to get proper results from our output,
   but do not change into or out of RAW mode
   so that no input is discarded.

   After doing this, either terminal_ours or terminal_inferior
   should be called to get back to a normal state of affairs.  */

void
terminal_ours_for_output ()
{
  terminal_ours_1 (1);
}

/* Put our terminal settings into effect.
   First record the inferior's terminal settings
   so they can be restored properly later.  */

void
terminal_ours ()
{
  terminal_ours_1 (0);
}

static void
terminal_ours_1 (output_only)
     int output_only;
{
#ifdef TIOCGPGRP
  /* Ignore this signal since it will happen when we try to set the pgrp.  */
  void (*osigttou) ();
#endif /* TIOCGPGRP */

  /* The check for inferior_thisrun_terminal had been commented out
     when the call to ioctl (TIOCNOTTY) was commented out.
     Checking inferior_thisrun_terminal is necessary so that
     if GDB is running in the background, it won't block trying
     to do the ioctl()'s below.  */
  if (inferior_thisrun_terminal != 0)
    return;

  if (!terminal_is_ours)
    {
      terminal_is_ours = 1;

#ifdef TIOCGPGRP
      osigttou = (void (*) ()) signal (SIGTTOU, SIG_IGN);

      ioctl (0, TIOCGPGRP, &pgrp_inferior);
      ioctl (0, TIOCSPGRP, &pgrp_ours);

      signal (SIGTTOU, osigttou);
#else
      signal (SIGINT, sigint_ours);
      signal (SIGQUIT, sigquit_ours);
#endif /* TIOCGPGRP */

      tflags_inferior = fcntl (0, F_GETFL, 0);
      ioctl (0, TIOCGETP, &sg_inferior);

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
      ioctl (0, TIOCGETC, &tc_inferior);
#endif
#ifdef TIOCGLTC
      ioctl (0, TIOCGLTC, &ltc_inferior);
#endif
#ifdef TIOCLGET
      ioctl (0, TIOCLGET, &lmode_inferior);
#endif
    }

#ifdef HAVE_TERMIO
  sg_ours.c_lflag |= ICANON;
  if (output_only && !(sg_inferior.c_lflag & ICANON))
    sg_ours.c_lflag &= ~ICANON;
#else /* not HAVE_TERMIO */
  sg_ours.sg_flags &= ~RAW & ~CBREAK;
  if (output_only)
    sg_ours.sg_flags |= (RAW | CBREAK) & sg_inferior.sg_flags;
#endif /* not HAVE_TERMIO */

  fcntl (0, F_SETFL, tflags_ours);
  fcntl (0, F_SETFL, tflags_ours);
  ioctl (0, TIOCSETN, &sg_ours);

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
  ioctl (0, TIOCSETC, &tc_ours);
#endif
#ifdef TIOCGLTC
  ioctl (0, TIOCSLTC, &ltc_ours);
#endif
#ifdef TIOCLGET
  ioctl (0, TIOCLSET, &lmode_ours);
#endif

#ifdef HAVE_TERMIO
  sg_ours.c_lflag |= ICANON;
#else /* not HAVE_TERMIO */
  sg_ours.sg_flags &= ~RAW & ~CBREAK;
#endif /* not HAVE_TERMIO */
}

/* ARGSUSED */
void
term_info (arg, from_tty)
     char *arg;
     int from_tty;
{
  target_terminal_info (arg, from_tty);
}

void
child_terminal_info (args, from_tty)
     char *args;
     int from_tty;
{
  register int i;

  printf_filtered ("Inferior's terminal status (currently saved by GDB):\n");

#ifdef HAVE_TERMIO

  printf_filtered ("fcntl flags = 0x%x, c_iflag = 0x%x, c_oflag = 0x%x,\n",
	  tflags_inferior, sg_inferior.c_iflag, sg_inferior.c_oflag);
  printf_filtered ("c_cflag = 0x%x, c_lflag = 0x%x, c_line = 0x%x.\n",
	  sg_inferior.c_cflag, sg_inferior.c_lflag, sg_inferior.c_line);
  printf_filtered ("c_cc: ");
  for (i = 0; (i < NCC); i += 1)
    printf_filtered ("0x%x ", sg_inferior.c_cc[i]);
  printf_filtered ("\n");

#else /* not HAVE_TERMIO */

  printf_filtered ("fcntl flags = 0x%x, sgttyb.sg_flags = 0x%x, owner pid = %d.\n",
	  tflags_inferior, sg_inferior.sg_flags, pgrp_inferior);

#endif /* not HAVE_TERMIO */

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
  printf_filtered ("tchars: ");
  for (i = 0; i < (int)sizeof (struct tchars); i++)
    printf_filtered ("0x%x ", ((char *)&tc_inferior)[i]);
  printf_filtered ("\n");
#endif

#ifdef TIOCGLTC
  printf_filtered ("ltchars: ");
  for (i = 0; i < (int)sizeof (struct ltchars); i++)
    printf_filtered ("0x%x ", ((char *)&ltc_inferior)[i]);
  printf_filtered ("\n");
#endif
  
#ifdef TIOCLGET
  printf_filtered ("lmode:  0x%x\n", lmode_inferior);
#endif
}

/* NEW_TTY is called in new child processes under Unix, which will
   become debugger target processes.
   If the TTYNAME argument is non-null, we switch to that tty for further
   input and output.  In either case, we remember the setup.  */

void
new_tty (ttyname)
     char *ttyname;
{
  register int tty;

  /* Save the name for later, for determining whether we and the child
     are sharing a tty.  */
  inferior_thisrun_terminal = ttyname;
  if (ttyname == 0)
    return;

#ifdef TIOCNOTTY
  /* Disconnect the child process from our controlling terminal.  */
  tty = open("/dev/tty", O_RDWR);
  if (tty > 0)
    {
      ioctl(tty, TIOCNOTTY, 0);
      close(tty);
    }
#endif

  /* Now open the specified new terminal.  */

  tty = open(ttyname, O_RDWR);
  if (tty == -1)
    {
      print_sys_errmsg (ttyname, errno);
      _exit(1);
    }

  /* Avoid use of dup2; doesn't exist on all systems.  */
  if (tty != 0)
    { close (0); dup (tty); }
  if (tty != 1)
    { close (1); dup (tty); }
  if (tty != 2)
    { close (2); dup (tty); }
  if (tty > 2)
    close(tty);
}

/* Kill the inferior process.  Make us have no inferior.  */

/* ARGSUSED */
static void
kill_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  if (inferior_pid == 0)
    error ("The program is not being run.");
  if (!query ("Kill the inferior process? "))
    error ("Not confirmed.");
  target_kill (arg, from_tty);

  /* Killing off the inferior can leave us with a core file.  If so,
     print the state we are left in.  */
  if (target_has_stack) {
    printf_filtered ("In %s,\n", current_target->to_longname);
    if (selected_frame == NULL)
      fputs_filtered ("No selected stack frame.\n", stdout);
    else
      print_sel_frame (0);
  }
}

/* The inferior process has died.  Long live the inferior!  */

void
generic_mourn_inferior ()
{
  inferior_pid = 0;
  attach_flag = 0;
  mark_breakpoints_out ();
  registers_changed ();

#ifdef CLEAR_DEFERRED_STORES
  /* Delete any pending stores to the inferior... */
  CLEAR_DEFERRED_STORES;
#endif

  reopen_exec_file ();
  if (target_has_stack) {
    set_current_frame ( create_new_frame (read_register (FP_REGNUM),
					  read_pc ()));
    select_frame (get_current_frame (), 0);
  } else {
    set_current_frame (0);
    select_frame ((FRAME) 0, -1);
  }
  /* It is confusing to the user for ignore counts to stick around
     from previous runs of the inferior.  So clear them.  */
  breakpoint_clear_ignore_counts ();
}

void
child_mourn_inferior ()
{
  unpush_target (&child_ops);
  generic_mourn_inferior ();
}

#if 0 
/* This function is just for testing, and on some systems (Sony NewsOS
   3.2) <sys/user.h> also includes <sys/time.h> which leads to errors
   (since on this system at least sys/time.h is not protected against
   multiple inclusion).  */
/* ARGSUSED */
static void
try_writing_regs_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register int i;
  register int value;

  if (inferior_pid == 0)
    error ("There is no inferior process now.");

  /* A Sun 3/50 or 3/60 (at least) running SunOS 4.0.3 will have a
     kernel panic if we try to write past the end of the user area.
     Presumably Sun will fix this bug (it has been reported), but it
     is tacky to crash the system, so at least on SunOS4 we need to
     stop writing when we hit the end of the user area.  */
  for (i = 0; i < sizeof (struct user); i += 2)
    {
      QUIT;
      errno = 0;
      value = call_ptrace (3, inferior_pid, i, 0);
      call_ptrace (6, inferior_pid, i, value);
      if (errno == 0)
	{
	  printf (" Succeeded with address 0x%x; value 0x%x (%d).\n",
		  i, value, value);
	}
      else if ((i & 0377) == 0)
	printf (" Failed at 0x%x.\n", i);
    }
}
#endif

void
_initialize_inflow ()
{
  add_info ("terminal", term_info,
	   "Print inferior's saved terminal status.");

#if 0
  add_com ("try-writing-regs", class_obscure, try_writing_regs_command,
	   "Try writing all locations in inferior's system block.\n\
Report which ones can be written.");
#endif

  add_com ("kill", class_run, kill_command,
	   "Kill execution of program being debugged.");

  inferior_pid = 0;

  ioctl (0, TIOCGETP, &sg_ours);
  fcntl (0, F_GETFL, tflags_ours);

#if defined(TIOCGETC) && !defined(TIOCGETC_BROKEN)
  ioctl (0, TIOCGETC, &tc_ours);
#endif
#ifdef TIOCGLTC
  ioctl (0, TIOCGLTC, &ltc_ours);
#endif
#ifdef TIOCLGET
  ioctl (0, TIOCLGET, &lmode_ours);
#endif

#ifdef TIOCGPGRP
  ioctl (0, TIOCGPGRP, &pgrp_ours);
#endif /* TIOCGPGRP */

  terminal_is_ours = 1;
}

