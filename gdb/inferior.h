/* Variables that describe the inferior process running under GDB:
   Where it is, why it stopped, and how to step it.
   Copyright (C) 1986, 1989 Free Software Foundation, Inc.

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

/* For bpstat.  */
#include "breakpoint.h"

/* For FRAME_ADDR.  */
#include "frame.h"

/*
 * Structure in which to save the status of the inferior.  Save
 * through "save_inferior_status", restore through
 * "restore_inferior_status".
 * This pair of routines should be called around any transfer of
 * control to the inferior which you don't want showing up in your
 * control variables.
 */
struct inferior_status {
  int pc_changed;
  int stop_signal;
  int stop_pc;
  FRAME_ADDR stop_frame_address;
  bpstat stop_bpstat;
  int stop_step;
  int stop_stack_dummy;
  int stopped_by_random_signal;
  int trap_expected;
  CORE_ADDR step_range_start;
  CORE_ADDR step_range_end;
  FRAME_ADDR step_frame_address;
  int step_over_calls;
  CORE_ADDR step_resume_break_address;
  int stop_after_trap;
  int stop_soon_quietly;
  FRAME_ADDR selected_frame_address;
  int selected_level;
  char stop_registers[REGISTER_BYTES];
  int breakpoint_proceeded;
  int restore_stack_info;
  int proceed_to_finish;
};

void save_inferior_status (), restore_inferior_status ();

/* File name for default use for standard in/out in the inferior.  */

extern char *inferior_io_terminal;

/* Pid of our debugged inferior, or 0 if no inferior now.  */

extern int inferior_pid;

/* Character array containing an image of the inferior programs' registers.  */

extern char registers[];

extern void clear_proceed_status ();
extern void start_inferior ();
extern void proceed ();
extern void kill_inferior ();
extern void kill_inferior_fast ();
extern void generic_mourn_inferior ();
extern void terminal_ours ();
extern void detach ();
extern void run_stack_dummy ();
extern CORE_ADDR read_pc ();
extern void write_pc ();
extern void wait_for_inferior ();
extern void init_wait_for_inferior ();
extern void close_exec_file ();
extern void reopen_exec_file ();

/* From infcmd.c */
void attach_command (
#ifdef __STDC__
		     char *arg, int from_tty
#endif
		     );
		     
/* Last signal that the inferior received (why it stopped).  */

extern int stop_signal;

/* Address at which inferior stopped.  */

extern CORE_ADDR stop_pc;

/* Stack frame when program stopped.  */

extern FRAME_ADDR stop_frame_address;

/* Chain containing status of breakpoint(s) that we have stopped at.  */

extern bpstat stop_bpstat;

/* Flag indicating that a command has proceeded the inferior past the
   current breakpoint.  */

extern int breakpoint_proceeded;

/* Nonzero if stopped due to a step command.  */

extern int stop_step;

/* Nonzero if stopped due to completion of a stack dummy routine.  */

extern int stop_stack_dummy;

/* Nonzero if program stopped due to a random (unexpected) signal in
   inferior process.  */

extern int stopped_by_random_signal;

/* Range to single step within.
   If this is nonzero, respond to a single-step signal
   by continuing to step if the pc is in this range.  */

extern CORE_ADDR step_range_start; /* Inclusive */
extern CORE_ADDR step_range_end; /* Exclusive */

/* Stack frame address as of when stepping command was issued.
   This is how we know when we step into a subroutine call,
   and how to set the frame for the breakpoint used to step out.  */

extern FRAME_ADDR step_frame_address;

/* 1 means step over all subroutine calls.
   -1 means step over calls to undebuggable functions.  */

extern int step_over_calls;

/* If stepping, nonzero means step count is > 1
   so don't print frame next time inferior stops
   if it stops due to stepping.  */

extern int step_multi;

/* Nonzero means expecting a trap and caller will handle it themselves.
   It is used after attach, due to attaching to a process;
   when running in the shell before the child program has been exec'd;
   and when running some kinds of remote stuff (FIXME?).  */

int stop_soon_quietly;

/* Nonzero if proceed is being used for a "finish" command or a similar
   situation when stop_registers should be saved.  */

extern int proceed_to_finish;

/* Save register contents here when about to pop a stack dummy frame,
   if-and-only-if proceed_to_finish is set.
   Thus this contains the return value from the called function (assuming
   values are returned in a register).  */

extern char stop_registers[REGISTER_BYTES];

/* Nonzero if pc has been changed by the debugger
   since the inferior stopped.  */

extern int pc_changed;

/* Nonzero if the child process in inferior_pid was attached rather
   than forked.  */

int attach_flag;

/* Possible values for CALL_DUMMY_LOCATION.  */
#define ON_STACK 1
#define BEFORE_TEXT_END 2
#define AFTER_TEXT_END 3

#if !defined (CALL_DUMMY_LOCATION)
#define CALL_DUMMY_LOCATION ON_STACK
#endif /* No CALL_DUMMY_LOCATION.  */

/* Are we in a call dummy?  The code below which allows DECR_PC_AFTER_BREAK
   below is for infrun.c, which may give the macro a pc without that
   subtracted out.  */
#if !defined (PC_IN_CALL_DUMMY)
#if CALL_DUMMY_LOCATION == BEFORE_TEXT_END
#define PC_IN_CALL_DUMMY(pc, sp, frame_address) \
  ((pc) >= text_end - CALL_DUMMY_LENGTH         \
   && (pc) < text_end + DECR_PC_AFTER_BREAK)
#else /* Not before text_end.  */
#if CALL_DUMMY_LOCATION == AFTER_TEXT_END
#define PC_IN_CALL_DUMMY(pc, sp, frame_address) \
  ((pc) >= text_end   \
   && (pc) < text_end + CALL_DUMMY_LENGTH + DECR_PC_AFTER_BREAK)
#else /* On stack.  */
#define PC_IN_CALL_DUMMY(pc, sp, frame_address) \
  ((sp) INNER_THAN (pc) && (pc) INNER_THAN (frame_address))
#endif /* On stack.  */
#endif /* Not before text_end.  */
#endif /* No PC_IN_CALL_DUMMY.  */
