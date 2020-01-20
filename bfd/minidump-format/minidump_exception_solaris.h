/* Header file for the format of Windows minidumps.
   Copyright (C) 2020 Free Software Foundation, Inc.
   Written by Google LLC.
   Relicensed with permission, original at:
   https://source.chromium.org/chromium/chromium/src/+/master:third_party/breakpad/breakpad/src/google_breakpad/common

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* minidump_exception_solaris.h: A definition of exception codes for
 * Solaris
 *
 * (This is C99 source, please don't corrupt it with C++.)
 *
 * Author: Mark Mentovai
 * Split into its own file: Neal Sidhwaney */


#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_SOLARIS_H__
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_SOLARIS_H__

#include <stddef.h>

#include "breakpad_types.h"

/* For (MDException).exception_code.  These values come from sys/iso/signal_iso.h
 */
typedef enum {
  MD_EXCEPTION_CODE_SOL_SIGHUP = 1,      /* Hangup */
  MD_EXCEPTION_CODE_SOL_SIGINT = 2,      /* interrupt (rubout) */
  MD_EXCEPTION_CODE_SOL_SIGQUIT = 3,     /* quit (ASCII FS) */
  MD_EXCEPTION_CODE_SOL_SIGILL = 4,      /* illegal instruction (not reset when caught) */
  MD_EXCEPTION_CODE_SOL_SIGTRAP = 5,     /* trace trap (not reset when caught) */
  MD_EXCEPTION_CODE_SOL_SIGIOT = 6,      /* IOT instruction */
  MD_EXCEPTION_CODE_SOL_SIGABRT = 6,     /* used by abort, replace SIGIOT in the future */
  MD_EXCEPTION_CODE_SOL_SIGEMT = 7,      /* EMT instruction */
  MD_EXCEPTION_CODE_SOL_SIGFPE = 8,      /* floating point exception */
  MD_EXCEPTION_CODE_SOL_SIGKILL = 9,     /* kill (cannot be caught or ignored) */
  MD_EXCEPTION_CODE_SOL_SIGBUS = 10,     /* bus error */
  MD_EXCEPTION_CODE_SOL_SIGSEGV = 11,    /* segmentation violation */
  MD_EXCEPTION_CODE_SOL_SIGSYS = 12,     /* bad argument to system call */
  MD_EXCEPTION_CODE_SOL_SIGPIPE = 13,    /* write on a pipe with no one to read it */
  MD_EXCEPTION_CODE_SOL_SIGALRM = 14,    /* alarm clock */
  MD_EXCEPTION_CODE_SOL_SIGTERM = 15,    /* software termination signal from kill */
  MD_EXCEPTION_CODE_SOL_SIGUSR1 = 16,    /* user defined signal 1 */
  MD_EXCEPTION_CODE_SOL_SIGUSR2 = 17,    /* user defined signal 2 */
  MD_EXCEPTION_CODE_SOL_SIGCLD = 18,     /* child status change */
  MD_EXCEPTION_CODE_SOL_SIGCHLD = 18,    /* child status change alias (POSIX) */
  MD_EXCEPTION_CODE_SOL_SIGPWR = 19,     /* power-fail restart */
  MD_EXCEPTION_CODE_SOL_SIGWINCH = 20,   /* window size change */
  MD_EXCEPTION_CODE_SOL_SIGURG = 21,     /* urgent socket condition */
  MD_EXCEPTION_CODE_SOL_SIGPOLL = 22,    /* pollable event occurred */
  MD_EXCEPTION_CODE_SOL_SIGIO = 22,      /* socket I/O possible (SIGPOLL alias) */
  MD_EXCEPTION_CODE_SOL_SIGSTOP = 23,    /* stop (cannot be caught or ignored) */
  MD_EXCEPTION_CODE_SOL_SIGTSTP = 24,    /* user stop requested from tty */
  MD_EXCEPTION_CODE_SOL_SIGCONT = 25,    /* stopped process has been continued */
  MD_EXCEPTION_CODE_SOL_SIGTTIN = 26,    /* background tty read attempted */
  MD_EXCEPTION_CODE_SOL_SIGTTOU = 27,    /* background tty write attempted */
  MD_EXCEPTION_CODE_SOL_SIGVTALRM = 28,  /* virtual timer expired */
  MD_EXCEPTION_CODE_SOL_SIGPROF = 29,    /* profiling timer expired */
  MD_EXCEPTION_CODE_SOL_SIGXCPU = 30,    /* exceeded cpu limit */
  MD_EXCEPTION_CODE_SOL_SIGXFSZ = 31,    /* exceeded file size limit */
  MD_EXCEPTION_CODE_SOL_SIGWAITING = 32, /* reserved signal no longer used by threading code */
  MD_EXCEPTION_CODE_SOL_SIGLWP = 33,     /* reserved signal no longer used by threading code */
  MD_EXCEPTION_CODE_SOL_SIGFREEZE = 34,  /* special signal used by CPR */
  MD_EXCEPTION_CODE_SOL_SIGTHAW = 35,    /* special signal used by CPR */
  MD_EXCEPTION_CODE_SOL_SIGCANCEL = 36,  /* reserved signal for thread cancellation */
  MD_EXCEPTION_CODE_SOL_SIGLOST = 37,    /* resource lost (eg, record-lock lost) */
  MD_EXCEPTION_CODE_SOL_SIGXRES = 38,    /* resource control exceeded */
  MD_EXCEPTION_CODE_SOL_SIGJVM1 = 39,    /* reserved signal for Java Virtual Machine */
  MD_EXCEPTION_CODE_SOL_SIGJVM2 = 40     /* reserved signal for Java Virtual Machine */
} MDExceptionCodeSolaris;

#endif  /* GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_SOLARIS_H__ */
