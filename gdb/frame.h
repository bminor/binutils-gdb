/* Definitions for dealing with stack frames, for GDB, the GNU debugger.
   Copyright (C) 1986 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

/* Note that frame.h requires param.h! */

#define FRAME CORE_ADDR

struct frame_info
  {
    /* Nominal address of the frame described.  */
    FRAME frame;
    /* Address at which execution is occurring in this frame.
       For the innermost frame, it's the current pc.
       For other frames, it is a pc saved in the next frame.  */
    CORE_ADDR pc;
    /* The frame called by the frame we are describing, or 0.  */
    FRAME next_frame;
  };

/* Describe the saved registers of a frame.  */

struct frame_saved_regs
  {
    /* For each register, address of where it was saved on entry to the frame,
       or zero if it was not saved on entry to this frame.  */
    CORE_ADDR regs[NUM_REGS];
  };

/* The stack frame that the user has specified for commands to act on.
   Note that one cannot assume this is the address of valid data.  */

extern FRAME selected_frame;

extern struct frame_info get_frame_info ();
extern struct frame_info get_prev_frame_info ();

extern void get_frame_saved_regs ();

extern FRAME get_prev_frame ();

extern FRAME get_current_frame ();

extern struct block *get_frame_block ();
extern struct block *get_current_block ();
extern struct block *get_selected_block ();
extern struct symbol *get_frame_function ();
extern struct symbol *get_pc_function ();
