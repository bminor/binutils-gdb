/* Copyright (C) 1990 Free Software Foundation, Inc.

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

#if !defined (BREAKPOINT_H)
#define BREAKPOINT_H 1

/* This is the maximum number of bytes a breakpoint instruction can take.
   Feel free to increase it.  It's just used in a few places to size
   arrays that should be independent of the target architecture.  */

#define	BREAKPOINT_MAX	10

extern void breakpoint_re_set ();
extern void clear_momentary_breakpoints ();
extern void set_momentary_breakpoint ();
extern void set_ignore_count ();
extern void set_default_breakpoint ();

extern void mark_breakpoints_out ();
extern void breakpoint_auto_delete ();
extern void breakpoint_clear_ignore_counts ();

/* The following are for displays, which aren't really breakpoints, but
   here is as good a place as any for them.  */

extern void disable_current_display ();
extern void do_displays ();
extern void disable_display ();
extern void clear_displays ();


/* The follow stuff is an abstract data type "bpstat" ("breakpoint status").
   This provides the ability to determine whether we have stopped at a
   breakpoint, and what we should do about it.  */

typedef struct bpstat__struct *bpstat;

/* Interface:  */

/* Clear a bpstat so that it says we are not at any breakpoint.
   Also free any storage that is part of a bpstat.  */
void bpstat_clear();

/* Return a copy of a bpstat.  Like "bs1 = bs2" but all storage that
   is part of the bpstat is copied as well.  */
bpstat bpstat_copy();

/* Get a bpstat associated with having just stopped at address *PC
   and frame address FRAME_ADDRESS.  Update *PC to point at the
   breakpoint (if we hit a breakpoint).  */
bpstat bpstat_stop_status (/* CORE_ADDR *pc; FRAME_ADDR frame_address */);

/* Nonzero if we should print the frame.  */
#define bpstat_should_print(bs) ((bs) != NULL && (bs)->print)

/* Nonzero if we should stop.  */
#define bpstat_stop(bs) ((bs) != NULL && (bs)->stop)

/* Nonzero if we hit a momentary breakpoint.  */
#define bpstat_momentary_breakpoint(bs) ((bs) != NULL && (bs)->momentary)

/* Nonzero if a signal that we got in wait() was due to circumstances
   explained by the BS.  */
/* Currently that is true iff we have hit a breakpoint.  */
#define bpstat_explains_signal(bs) ((bs) != NULL)

/* Nonzero if we should step constantly (e.g. watchpoints on machines
   without hardware support).  This isn't related to a specific bpstat,
   just to things like whether watchpoints are set.  */
int bpstat_should_step (/* void */);

/* Print a message indicating what happened.  Returns nonzero to
   say that only the source line should be printed after this (zero
   return means print the frame as well as the source line).  */
int bpstat_print (/* bpstat bs */);

/* Return the breakpoint number of the first breakpoint we are stopped
   at.  *BSP upon return is a bpstat which points to the remaining
   breakpoints stopped at (but which is not guaranteed to be good for
   anything but further calls to bpstat_num).
   Return 0 if passed a bpstat which does not indicate any breakpoints.  */
int bpstat_num (/* bpstat *bsp; */);

/* Perform actions associated with having stopped at *BSP.  */
void bpstat_do_actions (/* bpstat bs; */);

/* Modify BS so that the actions will not be performed.  */
void bpstat_clear_actions (/* bpstat bs; */);


/* Implementation:  */
#include "value.h"
struct bpstat__struct
{
  /* Linked list because there can be two breakpoints at the
     same place, and a bpstat reflects the fact that both have been hit.  */
  bpstat next;
  /* Breakpoint that we are at.  */
  struct breakpoint *breakpoint_at;
  /* Commands left to be done.  */
  struct command_line *commands;
  /* Old value associated with a watchpoint.  */
  value old_val;
  /* Nonzero if we should print the frame.  Only significant for the first
     bpstat in the chain.  */
  char print;
  /* Nonzero if we should stop.  Only significant for the first bpstat in
     the chain.  */
  char stop;
  /* Nonzero if we hit a momentary breakpoint.  Only significant for the
     first bpstat in the chain.  */
  char momentary;
};
#endif /* breakpoint.h not already included.  */
