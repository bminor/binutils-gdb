/* Frame unwinder for Mitsubishi D10V, for GDB,  the GNU Debugger.

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003 Free
   Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "frame.h"
#include "frame-unwind.h"
#include "d10v-tdep.h"
#include "gdbcore.h"
#include "regcache.h"
#include "target.h"
#include "gdb_assert.h"

struct frame_unwind_cache
{
  CORE_ADDR return_pc;
  int frameless;
  int size;
  CORE_ADDR *saved_regs;
  CORE_ADDR next_addr;
  int uses_frame;
  void **regs;
};


static int
prologue_find_regs (unsigned short op, struct frame_unwind_cache *info,
		    CORE_ADDR addr)
{
  int n;

  /* st  rn, @-sp */
  if ((op & 0x7E1F) == 0x6C1F)
    {
      n = (op & 0x1E0) >> 5;
      info->next_addr -= 2;
      info->saved_regs[n] = info->next_addr;
      return 1;
    }

  /* st2w  rn, @-sp */
  else if ((op & 0x7E3F) == 0x6E1F)
    {
      n = (op & 0x1E0) >> 5;
      info->next_addr -= 4;
      info->saved_regs[n] = info->next_addr;
      info->saved_regs[n + 1] = info->next_addr + 2;
      return 1;
    }

  /* subi  sp, n */
  if ((op & 0x7FE1) == 0x01E1)
    {
      n = (op & 0x1E) >> 1;
      if (n == 0)
	n = 16;
      info->next_addr -= n;
      return 1;
    }

  /* mv  r11, sp */
  if (op == 0x417E)
    {
      info->uses_frame = 1;
      return 1;
    }

  /* nop */
  if (op == 0x5E00)
    return 1;

  /* st  rn, @sp */
  if ((op & 0x7E1F) == 0x681E)
    {
      n = (op & 0x1E0) >> 5;
      info->saved_regs[n] = info->next_addr;
      return 1;
    }

  /* st2w  rn, @sp */
  if ((op & 0x7E3F) == 0x3A1E)
    {
      n = (op & 0x1E0) >> 5;
      info->saved_regs[n] = info->next_addr;
      info->saved_regs[n + 1] = info->next_addr + 2;
      return 1;
    }

  return 0;
}

struct frame_unwind_cache *
d10v_frame_unwind_cache (struct frame_info *fi,
			 struct frame_unwind_cache **cache)
{
  CORE_ADDR fp, pc;
  unsigned long op;
  unsigned short op1, op2;
  int i;
  struct frame_unwind_cache *info;

  if ((*cache))
    return (*cache);

  info = FRAME_OBSTACK_ZALLOC (struct frame_unwind_cache);
  (*cache) = info;
  info->saved_regs = frame_obstack_zalloc (SIZEOF_FRAME_SAVED_REGS);

  info->frameless = 0;
  info->size = 0;
  info->return_pc = 0;

  fp = get_frame_base (fi);
  info->next_addr = 0;

  pc = get_pc_function_start (get_frame_pc (fi));

  info->uses_frame = 0;
  while (1)
    {
      op = (unsigned long) read_memory_integer (pc, 4);
      if ((op & 0xC0000000) == 0xC0000000)
	{
	  /* long instruction */
	  if ((op & 0x3FFF0000) == 0x01FF0000)
	    {
	      /* add3 sp,sp,n */
	      short n = op & 0xFFFF;
	      info->next_addr += n;
	    }
	  else if ((op & 0x3F0F0000) == 0x340F0000)
	    {
	      /* st  rn, @(offset,sp) */
	      short offset = op & 0xFFFF;
	      short n = (op >> 20) & 0xF;
	      info->saved_regs[n] = info->next_addr + offset;
	    }
	  else if ((op & 0x3F1F0000) == 0x350F0000)
	    {
	      /* st2w  rn, @(offset,sp) */
	      short offset = op & 0xFFFF;
	      short n = (op >> 20) & 0xF;
	      info->saved_regs[n] = info->next_addr + offset;
	      info->saved_regs[n + 1] = info->next_addr + offset + 2;
	    }
	  else
	    break;
	}
      else
	{
	  /* short instructions */
	  if ((op & 0xC0000000) == 0x80000000)
	    {
	      op2 = (op & 0x3FFF8000) >> 15;
	      op1 = op & 0x7FFF;
	    }
	  else
	    {
	      op1 = (op & 0x3FFF8000) >> 15;
	      op2 = op & 0x7FFF;
	    }
	  if (!prologue_find_regs (op1, (*cache), pc) 
	      || !prologue_find_regs (op2, (*cache), pc))
	    break;
	}
      pc += 4;
    }

  info->size = -info->next_addr;

  if (!(fp & 0xffff))
    fp = d10v_read_sp ();

  for (i = 0; i < NUM_REGS - 1; i++)
    if (info->saved_regs[i])
      {
	info->saved_regs[i] = fp - (info->next_addr - info->saved_regs[i]);
      }

  if (info->saved_regs[D10V_LR_REGNUM])
    {
      CORE_ADDR return_pc 
	= read_memory_unsigned_integer (info->saved_regs[D10V_LR_REGNUM], 
					REGISTER_RAW_SIZE (D10V_LR_REGNUM));
      info->return_pc = d10v_make_iaddr (return_pc);
    }
  else
    {
      ULONGEST return_pc;
      frame_read_unsigned_register (fi, D10V_LR_REGNUM, &return_pc);
      info->return_pc = d10v_make_iaddr (return_pc);
    }

  /* The SP is not normally (ever?) saved, but check anyway */
  if (!info->saved_regs[D10V_SP_REGNUM])
    {
      /* if the FP was saved, that means the current FP is valid, */
      /* otherwise, it isn't being used, so we use the SP instead */
      if (info->uses_frame)
	info->saved_regs[D10V_SP_REGNUM] 
	  = d10v_read_fp () + info->size;
      else
	{
	  info->saved_regs[SP_REGNUM] = fp + info->size;
	  info->frameless = 1;
	  info->saved_regs[FP_REGNUM] = 0;
	}
    }

  return info;
}

static CORE_ADDR
d10v_frame_pc_unwind (struct frame_info *frame,
		      struct frame_unwind_cache **cache)
{
  struct frame_unwind_cache *info = d10v_frame_unwind_cache (frame, cache);
  return info->return_pc;
}

static void
d10v_frame_id_unwind (struct frame_info *frame,
		      struct frame_unwind_cache **cache,
		      struct frame_id *id)
{
  struct frame_unwind_cache *info = d10v_frame_unwind_cache (frame, cache);
  CORE_ADDR addr;

  /* Start with a NULL frame ID.  */
  (*id) = null_frame_id;

  if (info->return_pc == D10V_IMEM_START
      || inside_entry_file (info->return_pc))
    {
      /* This is meant to halt the backtrace at "_start".
	 Make sure we don't halt it at a generic dummy frame. */
      return;
    }

  if (!info->saved_regs[FP_REGNUM])
    {
      if (!info->saved_regs[SP_REGNUM]
	  || info->saved_regs[SP_REGNUM] == D10V_STACK_START)
	return;

      id->base = info->saved_regs[SP_REGNUM];
      id->pc = info->return_pc;
    }

  addr = read_memory_unsigned_integer (info->saved_regs[FP_REGNUM],
				       REGISTER_RAW_SIZE (FP_REGNUM));
  if (addr == 0)
    return;

  id->base = d10v_make_daddr (addr);
  id->pc = info->return_pc;
}

static void
saved_regs_unwinder (struct frame_info *frame,
		     CORE_ADDR *saved_regs,
		     int regnum, int *optimizedp,
		     enum lval_type *lvalp, CORE_ADDR *addrp,
		     int *realnump, void *bufferp)
{
  /* If we're using generic dummy frames, we'd better not be in a call
     dummy.  (generic_call_dummy_register_unwind ought to have been called
     instead.)  */
  gdb_assert (!(DEPRECATED_USE_GENERIC_DUMMY_FRAMES
		&& (get_frame_type (frame) == DUMMY_FRAME)));

  if (saved_regs[regnum] != 0)
    {
      if (regnum == SP_REGNUM)
	{
	  /* SP register treated specially.  */
	  *optimizedp = 0;
	  *lvalp = not_lval;
	  *addrp = 0;
	  *realnump = -1;
	  if (bufferp != NULL)
	    store_address (bufferp, REGISTER_RAW_SIZE (regnum),
			   saved_regs[regnum]);
	}
      else
	{
	  /* Any other register is saved in memory, fetch it but cache
	     a local copy of its value.  */
	  *optimizedp = 0;
	  *lvalp = lval_memory;
	  *addrp = saved_regs[regnum];
	  *realnump = -1;
	  if (bufferp != NULL)
	    {
	      /* Read the value in from memory.  */
	      read_memory (saved_regs[regnum], bufferp,
			   REGISTER_RAW_SIZE (regnum));
	    }
	}
      return;
    }

  /* No luck, assume this and the next frame have the same register
     value.  If a value is needed, pass the request on down the chain;
     otherwise just return an indication that the value is in the same
     register as the next frame.  */
  frame_register (frame, regnum, optimizedp, lvalp, addrp,
		  realnump, bufferp);
}


static void
d10v_frame_register_unwind (struct frame_info *frame,
			    struct frame_unwind_cache **cache,
			    int regnum, int *optimizedp,
			    enum lval_type *lvalp, CORE_ADDR *addrp,
			    int *realnump, void *bufferp)
{
  struct frame_unwind_cache *info = d10v_frame_unwind_cache (frame, cache);
  saved_regs_unwinder (frame, info->saved_regs, regnum, optimizedp,
		       lvalp, addrp, realnump, bufferp);
}


void
do_d10v_pop_frame (struct frame_info *fi)
{
  struct frame_unwind_cache *info =
    d10v_frame_unwind_cache (fi, &fi->unwind_cache);
  CORE_ADDR fp;
  int regnum;
  char raw_buffer[8];

  fp = get_frame_base (fi);

  /* now update the current registers with the old values */
  for (regnum = d10v_a0_regnum (current_gdbarch); regnum < d10v_a0_regnum (current_gdbarch) + D10V_NR_A_REGS; regnum++)
    {
      if (info->saved_regs[regnum])
	{
	  read_memory (info->saved_regs[regnum], raw_buffer, REGISTER_RAW_SIZE (regnum));
	  deprecated_write_register_bytes (REGISTER_BYTE (regnum), raw_buffer,
					   REGISTER_RAW_SIZE (regnum));
	}
    }
  for (regnum = 0; regnum < D10V_SP_REGNUM; regnum++)
    {
      if (info->saved_regs[regnum])
	{
	  write_register (regnum, read_memory_unsigned_integer (info->saved_regs[regnum], REGISTER_RAW_SIZE (regnum)));
	}
    }
  if (info->saved_regs[D10V_PSW_REGNUM])
    {
      write_register (D10V_PSW_REGNUM, read_memory_unsigned_integer (info->saved_regs[D10V_PSW_REGNUM], REGISTER_RAW_SIZE (D10V_PSW_REGNUM)));
    }

  write_register (D10V_PC_REGNUM, read_register (D10V_LR_REGNUM));
  write_register (D10V_SP_REGNUM, fp + info->size);
  target_store_registers (-1);
  flush_cached_frames ();
}

static struct frame_unwind d10v_frame_unwind = {
  d10v_frame_pc_unwind,
  d10v_frame_id_unwind,
  d10v_frame_register_unwind
};

const struct frame_unwind *
d10v_frame_p (CORE_ADDR pc)
{
  return &d10v_frame_unwind;
}
