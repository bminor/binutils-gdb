/* Get info from stack frames;
   convert between frames, blocks, functions and pc values.
   Copyright (C) 1986, 1987, 1988 Free Software Foundation, Inc.

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

#include "defs.h"
#include "initialize.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"

/* Address of end of first object file.
   This file is assumed to be a startup file
   and frames with pc's inside it
   are treated as nonexistent.  */

CORE_ADDR first_object_file_end;

/* Address of innermost stack frame (contents of FP register) */

static FRAME current_frame;

struct block *block_for_pc ();
CORE_ADDR get_pc_function_start ();

START_FILE

/* Return the innermost (currently executing) stack frame.  */

FRAME
get_current_frame ()
{
  /* We assume its address is kept in a general register;
     param.h says which register.  */

  return current_frame;
}

void
set_current_frame (frame)
     FRAME frame;
{
  current_frame = frame;
}

/* Return the frame that called FRAME.
   If FRAME is the original frame (it has no caller), return 0.  */

FRAME
get_prev_frame (frame)
     FRAME frame;
{
  CORE_ADDR pointer;
  /* The caller of "no frame" is the innermost frame.  */
  if (frame == 0)
    return get_current_frame ();

  /* Two macros defined in param.h specify the machine-dependent
     actions to be performed here.  */
  /* First, get the frame's chain-pointer.
     If that is zero, the frame is the outermost frame.  */
  pointer = FRAME_CHAIN (frame);
  if (!FRAME_CHAIN_VALID (pointer, frame))
    return 0;
  /* If frame has a caller, combine the chain pointer and the frame's own
     address to get the address of the caller.  */
  return FRAME_CHAIN_COMBINE (pointer, frame);
}

/* Return a structure containing various interesting information
   about a specified stack frame.  */

struct frame_info
get_frame_info (frame)
     FRAME frame;
{
  struct frame_info val;
  FRAME current = get_current_frame ();
  register FRAME frame1;

  val.frame = frame;

  if (frame == current)
    {
      val.pc = read_pc ();
      val.next_frame = 0;
    }
  else 
    {
      for (frame1 = current; frame1; frame1 = get_prev_frame (frame1))
	{
	  QUIT;
	  if (frame1 == frame)
	    break;

	  val.pc = FRAME_SAVED_PC (frame1);
	  val.next_frame = frame1;
	}
    }

  return val;
}

/* Return a structure containing various interesting information
   about the frame that called FRAME.

   This is much faster than get_frame_info (get_prev_frame (FRAME))
   because it does not need to search the entire stack
   to find the frame called by the one being described -- that is FRAME.  */

struct frame_info
get_prev_frame_info (next_frame)
     FRAME next_frame;
{
  struct frame_info val;
  register FRAME frame = get_prev_frame (next_frame);

  val.frame = frame;
  val.next_frame = next_frame;

  if (next_frame == 0)
    {
      val.pc = read_pc ();
    }
  else 
    {
      val.pc = FRAME_SAVED_PC (next_frame);
    }

  return val;
}

CORE_ADDR
get_frame_pc (frame)
     FRAME frame;
{
  struct frame_info fi;
  fi = get_frame_info (frame);
  return fi.pc;
}

/* Find the addresses in which registers are saved in FRAME.  */

void
get_frame_saved_regs (frame_info_addr, saved_regs_addr)
     struct frame_info *frame_info_addr;
     struct frame_saved_regs *saved_regs_addr;
{
  FRAME_FIND_SAVED_REGS (*frame_info_addr, *saved_regs_addr);
}

/* Return the innermost lexical block in execution
   in a specified stack frame.  The frame address is assumed valid.  */

struct block *
get_frame_block (frame)
     FRAME frame;
{
  struct frame_info fi;

  fi = get_frame_info (frame);
  return block_for_pc (fi.pc);
}

struct block *
get_current_block ()
{
  return block_for_pc (read_pc ());
}

CORE_ADDR
get_pc_function_start (pc)
     CORE_ADDR pc;
{
  register struct block *bl = block_for_pc (pc);
  register struct symbol *symbol;
  if (bl == 0)
    {
      register int misc_index = find_pc_misc_function (pc);
      if (misc_index >= 0)
	return misc_function_vector[misc_index].address;
      return 0;
    }
  symbol = block_function (bl);
  bl = SYMBOL_BLOCK_VALUE (symbol);
  return BLOCK_START (bl);
}  

/* Return the symbol for the function executing in frame FRAME.  */

struct symbol *
get_frame_function (frame)
     FRAME frame;
{
  register struct block *bl = get_frame_block (frame);
  if (bl == 0)
    return 0;
  return block_function (bl);
}

/* Return the innermost lexical block containing the specified pc value,
   or 0 if there is none.  */

struct block *
block_for_pc (pc)
     register CORE_ADDR pc;
{
  register struct block *b;
  register int bot, top, half;
  register struct symtab *s;
  struct blockvector *bl;

  /* First search all symtabs for one whose file contains our pc */

  for (s = symtab_list; s; s = s->next)
    {
      bl = BLOCKVECTOR (s);
      b = BLOCKVECTOR_BLOCK (bl, 0);
      if (BLOCK_START (b) <= pc
	  && BLOCK_END (b) > pc)
	break;
    }

  if (s == 0)
    return 0;

  /* Then search that symtab for the smallest block that wins.  */
  /* Use binary search to find the last block that starts before PC.  */

  bot = 0;
  top = BLOCKVECTOR_NBLOCKS (bl);

  while (top - bot > 1)
    {
      half = (top - bot + 1) >> 1;
      b = BLOCKVECTOR_BLOCK (bl, bot + half);
      if (BLOCK_START (b) <= pc)
	bot += half;
      else
	top = bot + half;
    }

  /* Now search backward for a block that ends after PC.  */

  while (bot >= 0)
    {
      b = BLOCKVECTOR_BLOCK (bl, bot);
      if (BLOCK_END (b) > pc)
	return b;
      bot--;
    }

  return 0;
}

/* Return the function containing pc value PC.
   Returns 0 if function is not known.  */

struct symbol *
find_pc_function (pc)
     CORE_ADDR pc;
{
  register struct block *b = block_for_pc (pc);
  if (b == 0)
    return 0;
  return block_function (b);
}

/* Find the misc function whose address is the largest
   while being less than PC.  Return its index in misc_function_vector.
   Returns -1 if PC is not in suitable range.  */

int
find_pc_misc_function (pc)
     register CORE_ADDR pc;
{
  register int lo = 0;
  register int hi = misc_function_count-1;
  register int new;
  register int distance;

  /* Note that the last thing in the vector is always _etext.  */

  /* Above statement is not *always* true - fix for case where there are */
  /* no misc functions at all (ie no symbol table has been read). */
  if (hi < 0) return -1;        /* no misc functions recorded */

  /* trivial reject range test */
  if (pc < misc_function_vector[0].address || 
      pc > misc_function_vector[hi].address)
    return -1;

  do {
    new = (lo + hi) >> 1;
    distance = misc_function_vector[new].address - pc;
    if (distance == 0)
      return new;		/* an exact match */
    else if (distance > 0)
      hi = new;
    else
      lo = new;
  } while (hi-lo != 1);

  /* if here, we had no exact match, so return the lower choice */
  return lo;
}

/* Return the innermost stack frame executing inside of the specified block,
   or zero if there is no such frame.  */

FRAME
block_innermost_frame (block)
     struct block *block;
{
  struct frame_info fi;
  register FRAME frame;
  register CORE_ADDR start = BLOCK_START (block);
  register CORE_ADDR end = BLOCK_END (block);

  frame = 0;
  while (1)
    {
      fi = get_prev_frame_info (frame);
      frame = fi.frame;
      if (frame == 0)
	return 0;
      if (fi.pc >= start && fi.pc < end)
	return frame;
    }
}

static
initialize ()
{
}

END_FILE
