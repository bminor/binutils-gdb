// OBSOLETE /* Target-dependent code for the Matsushita MN10200 for GDB, the GNU debugger.
// OBSOLETE 
// OBSOLETE    Copyright 1997, 1998, 1999, 2000, 2001, 2003 Free Software
// OBSOLETE    Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "frame.h"
// OBSOLETE #include "inferior.h"
// OBSOLETE #include "target.h"
// OBSOLETE #include "value.h"
// OBSOLETE #include "bfd.h"
// OBSOLETE #include "gdb_string.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "symfile.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Should call_function allocate stack space for a struct return?  */
// OBSOLETE int
// OBSOLETE mn10200_use_struct_convention (int gcc_p, struct type *type)
// OBSOLETE {
// OBSOLETE   return (TYPE_NFIELDS (type) > 1 || TYPE_LENGTH (type) > 8);
// OBSOLETE }
// OBSOLETE /* *INDENT-OFF* */
// OBSOLETE /* The main purpose of this file is dealing with prologues to extract
// OBSOLETE    information about stack frames and saved registers.
// OBSOLETE 
// OBSOLETE    For reference here's how prologues look on the mn10200:
// OBSOLETE 
// OBSOLETE      With frame pointer:
// OBSOLETE 	mov fp,a0
// OBSOLETE 	mov sp,fp
// OBSOLETE 	add <size>,sp
// OBSOLETE 	Register saves for d2, d3, a1, a2 as needed.  Saves start
// OBSOLETE 	at fp - <size> + <outgoing_args_size> and work towards higher
// OBSOLETE 	addresses.  Note that the saves are actually done off the stack
// OBSOLETE 	pointer in the prologue!  This makes for smaller code and easier
// OBSOLETE 	prologue scanning as the displacement fields will unlikely
// OBSOLETE         be more than 8 bits!
// OBSOLETE 
// OBSOLETE      Without frame pointer:
// OBSOLETE         add <size>,sp
// OBSOLETE 	Register saves for d2, d3, a1, a2 as needed.  Saves start
// OBSOLETE 	at sp + <outgoing_args_size> and work towards higher addresses.
// OBSOLETE 
// OBSOLETE      Out of line prologue:
// OBSOLETE 	add <local size>,sp  -- optional
// OBSOLETE 	jsr __prologue
// OBSOLETE 	add <outgoing_size>,sp -- optional
// OBSOLETE 
// OBSOLETE    The stack pointer remains constant throughout the life of most
// OBSOLETE    functions.  As a result the compiler will usually omit the
// OBSOLETE    frame pointer, so we must handle frame pointerless functions.  */
// OBSOLETE 
// OBSOLETE /* Analyze the prologue to determine where registers are saved,
// OBSOLETE    the end of the prologue, etc etc.  Return the end of the prologue
// OBSOLETE    scanned.
// OBSOLETE 
// OBSOLETE    We store into FI (if non-null) several tidbits of information:
// OBSOLETE 
// OBSOLETE     * stack_size -- size of this stack frame.  Note that if we stop in
// OBSOLETE     certain parts of the prologue/epilogue we may claim the size of the
// OBSOLETE     current frame is zero.  This happens when the current frame has
// OBSOLETE     not been allocated yet or has already been deallocated.
// OBSOLETE 
// OBSOLETE     * fsr -- Addresses of registers saved in the stack by this frame.
// OBSOLETE 
// OBSOLETE     * status -- A (relatively) generic status indicator.  It's a bitmask
// OBSOLETE     with the following bits: 
// OBSOLETE 
// OBSOLETE       MY_FRAME_IN_SP: The base of the current frame is actually in
// OBSOLETE       the stack pointer.  This can happen for frame pointerless
// OBSOLETE       functions, or cases where we're stopped in the prologue/epilogue
// OBSOLETE       itself.  For these cases mn10200_analyze_prologue will need up
// OBSOLETE       update fi->frame before returning or analyzing the register
// OBSOLETE       save instructions.
// OBSOLETE 
// OBSOLETE       MY_FRAME_IN_FP: The base of the current frame is in the
// OBSOLETE       frame pointer register ($a2).
// OBSOLETE 
// OBSOLETE       CALLER_A2_IN_A0: $a2 from the caller's frame is temporarily
// OBSOLETE       in $a0.  This can happen if we're stopped in the prologue.
// OBSOLETE 
// OBSOLETE       NO_MORE_FRAMES: Set this if the current frame is "start" or
// OBSOLETE       if the first instruction looks like mov <imm>,sp.  This tells
// OBSOLETE       frame chain to not bother trying to unwind past this frame.  */
// OBSOLETE /* *INDENT-ON* */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE #define MY_FRAME_IN_SP 0x1
// OBSOLETE #define MY_FRAME_IN_FP 0x2
// OBSOLETE #define CALLER_A2_IN_A0 0x4
// OBSOLETE #define NO_MORE_FRAMES 0x8
// OBSOLETE 
// OBSOLETE static CORE_ADDR
// OBSOLETE mn10200_analyze_prologue (struct frame_info *fi, CORE_ADDR pc)
// OBSOLETE {
// OBSOLETE   CORE_ADDR func_addr, func_end, addr, stop;
// OBSOLETE   CORE_ADDR stack_size = 0;
// OBSOLETE   unsigned char buf[4];
// OBSOLETE   int status;
// OBSOLETE   char *name;
// OBSOLETE   int out_of_line_prologue = 0;
// OBSOLETE 
// OBSOLETE   /* Use the PC in the frame if it's provided to look up the
// OBSOLETE      start of this function.  */
// OBSOLETE   pc = (fi ? get_frame_pc (fi) : pc);
// OBSOLETE 
// OBSOLETE   /* Find the start of this function.  */
// OBSOLETE   status = find_pc_partial_function (pc, &name, &func_addr, &func_end);
// OBSOLETE 
// OBSOLETE   /* Do nothing if we couldn't find the start of this function or if we're
// OBSOLETE      stopped at the first instruction in the prologue.  */
// OBSOLETE   if (status == 0)
// OBSOLETE     return pc;
// OBSOLETE 
// OBSOLETE   /* If we're in start, then give up.  */
// OBSOLETE   if (strcmp (name, "start") == 0)
// OBSOLETE     {
// OBSOLETE       if (fi)
// OBSOLETE 	fi->status = NO_MORE_FRAMES;
// OBSOLETE       return pc;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* At the start of a function our frame is in the stack pointer.  */
// OBSOLETE   if (fi)
// OBSOLETE     fi->status = MY_FRAME_IN_SP;
// OBSOLETE 
// OBSOLETE   /* If we're physically on an RTS instruction, then our frame has already
// OBSOLETE      been deallocated.
// OBSOLETE 
// OBSOLETE      fi->frame is bogus, we need to fix it.  */
// OBSOLETE   if (fi && get_frame_pc (fi) + 1 == func_end)
// OBSOLETE     {
// OBSOLETE       status = target_read_memory (get_frame_pc (fi), buf, 1);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (get_next_frame (fi) == NULL)
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return get_frame_pc (fi);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       if (buf[0] == 0xfe)
// OBSOLETE 	{
// OBSOLETE 	  if (get_next_frame (fi) == NULL)
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return get_frame_pc (fi);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Similarly if we're stopped on the first insn of a prologue as our
// OBSOLETE      frame hasn't been allocated yet.  */
// OBSOLETE   if (fi && get_frame_pc (fi) == func_addr)
// OBSOLETE     {
// OBSOLETE       if (get_next_frame (fi) == NULL)
// OBSOLETE 	deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE       return get_frame_pc (fi);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Figure out where to stop scanning.  */
// OBSOLETE   stop = fi ? get_frame_pc (fi) : func_end;
// OBSOLETE 
// OBSOLETE   /* Don't walk off the end of the function.  */
// OBSOLETE   stop = stop > func_end ? func_end : stop;
// OBSOLETE 
// OBSOLETE   /* Start scanning on the first instruction of this function.  */
// OBSOLETE   addr = func_addr;
// OBSOLETE 
// OBSOLETE   status = target_read_memory (addr, buf, 2);
// OBSOLETE   if (status != 0)
// OBSOLETE     {
// OBSOLETE       if (fi && get_next_frame (fi) == NULL && fi->status & MY_FRAME_IN_SP)
// OBSOLETE 	deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE       return addr;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* First see if this insn sets the stack pointer; if so, it's something
// OBSOLETE      we won't understand, so quit now.   */
// OBSOLETE   if (buf[0] == 0xdf
// OBSOLETE       || (buf[0] == 0xf4 && buf[1] == 0x77))
// OBSOLETE     {
// OBSOLETE       if (fi)
// OBSOLETE 	fi->status = NO_MORE_FRAMES;
// OBSOLETE       return addr;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Now see if we have a frame pointer.
// OBSOLETE 
// OBSOLETE      Search for mov a2,a0 (0xf278)
// OBSOLETE      then       mov a3,a2 (0xf27e).  */
// OBSOLETE 
// OBSOLETE   if (buf[0] == 0xf2 && buf[1] == 0x78)
// OBSOLETE     {
// OBSOLETE       /* Our caller's $a2 will be found in $a0 now.  Note it for
// OBSOLETE          our callers.  */
// OBSOLETE       if (fi)
// OBSOLETE 	fi->status |= CALLER_A2_IN_A0;
// OBSOLETE       addr += 2;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	{
// OBSOLETE 	  /* We still haven't allocated our local stack.  Handle this
// OBSOLETE 	     as if we stopped on the first or last insn of a function.   */
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       status = target_read_memory (addr, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE       if (buf[0] == 0xf2 && buf[1] == 0x7e)
// OBSOLETE 	{
// OBSOLETE 	  addr += 2;
// OBSOLETE 
// OBSOLETE 	  /* Our frame pointer is valid now.  */
// OBSOLETE 	  if (fi)
// OBSOLETE 	    {
// OBSOLETE 	      fi->status |= MY_FRAME_IN_FP;
// OBSOLETE 	      fi->status &= ~MY_FRAME_IN_SP;
// OBSOLETE 	    }
// OBSOLETE 	  if (addr >= stop)
// OBSOLETE 	    return addr;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Next we should allocate the local frame.
// OBSOLETE 
// OBSOLETE      Search for add imm8,a3 (0xd3XX)
// OBSOLETE      or add imm16,a3 (0xf70bXXXX)
// OBSOLETE      or add imm24,a3 (0xf467XXXXXX).
// OBSOLETE 
// OBSOLETE      If none of the above was found, then this prologue has
// OBSOLETE      no stack, and therefore can't have any register saves,
// OBSOLETE      so quit now.  */
// OBSOLETE   status = target_read_memory (addr, buf, 2);
// OBSOLETE   if (status != 0)
// OBSOLETE     {
// OBSOLETE       if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE       return addr;
// OBSOLETE     }
// OBSOLETE   if (buf[0] == 0xd3)
// OBSOLETE     {
// OBSOLETE       stack_size = extract_signed_integer (&buf[1], 1);
// OBSOLETE       if (fi)
// OBSOLETE 	fi->stack_size = stack_size;
// OBSOLETE       addr += 2;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp () - stack_size);
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else if (buf[0] == 0xf7 && buf[1] == 0x0b)
// OBSOLETE     {
// OBSOLETE       status = target_read_memory (addr + 2, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE       stack_size = extract_signed_integer (buf, 2);
// OBSOLETE       if (fi)
// OBSOLETE 	fi->stack_size = stack_size;
// OBSOLETE       addr += 4;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp () - stack_size);
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else if (buf[0] == 0xf4 && buf[1] == 0x67)
// OBSOLETE     {
// OBSOLETE       status = target_read_memory (addr + 2, buf, 3);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE       stack_size = extract_signed_integer (buf, 3);
// OBSOLETE       if (fi)
// OBSOLETE 	fi->stack_size = stack_size;
// OBSOLETE       addr += 5;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp () - stack_size);
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Now see if we have a call to __prologue for an out of line
// OBSOLETE      prologue.  */
// OBSOLETE   status = target_read_memory (addr, buf, 2);
// OBSOLETE   if (status != 0)
// OBSOLETE     return addr;
// OBSOLETE 
// OBSOLETE   /* First check for 16bit pc-relative call to __prologue.  */
// OBSOLETE   if (buf[0] == 0xfd)
// OBSOLETE     {
// OBSOLETE       CORE_ADDR temp;
// OBSOLETE       status = target_read_memory (addr + 1, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Get the PC this instruction will branch to.  */
// OBSOLETE       temp = (extract_signed_integer (buf, 2) + addr + 3) & 0xffffff;
// OBSOLETE 
// OBSOLETE       /* Get the name of the function at the target address.  */
// OBSOLETE       status = find_pc_partial_function (temp, &name, NULL, NULL);
// OBSOLETE       if (status == 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Note if it is an out of line prologue.  */
// OBSOLETE       out_of_line_prologue = (strcmp (name, "__prologue") == 0);
// OBSOLETE 
// OBSOLETE       /* This sucks up 3 bytes of instruction space.  */
// OBSOLETE       if (out_of_line_prologue)
// OBSOLETE 	addr += 3;
// OBSOLETE 
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 	    {
// OBSOLETE 	      fi->stack_size -= 16;
// OBSOLETE 	      deprecated_update_frame_base_hack (fi, read_sp () - fi->stack_size);
// OBSOLETE 	    }
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   /* Now check for the 24bit pc-relative call to __prologue.  */
// OBSOLETE   else if (buf[0] == 0xf4 && buf[1] == 0xe1)
// OBSOLETE     {
// OBSOLETE       CORE_ADDR temp;
// OBSOLETE       status = target_read_memory (addr + 2, buf, 3);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Get the PC this instruction will branch to.  */
// OBSOLETE       temp = (extract_signed_integer (buf, 3) + addr + 5) & 0xffffff;
// OBSOLETE 
// OBSOLETE       /* Get the name of the function at the target address.  */
// OBSOLETE       status = find_pc_partial_function (temp, &name, NULL, NULL);
// OBSOLETE       if (status == 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    deprecated_update_frame_base_hack (fi, read_sp ());
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       /* Note if it is an out of line prologue.  */
// OBSOLETE       out_of_line_prologue = (strcmp (name, "__prologue") == 0);
// OBSOLETE 
// OBSOLETE       /* This sucks up 5 bytes of instruction space.  */
// OBSOLETE       if (out_of_line_prologue)
// OBSOLETE 	addr += 5;
// OBSOLETE 
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	{
// OBSOLETE 	  if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP))
// OBSOLETE 	    {
// OBSOLETE 	      fi->stack_size -= 16;
// OBSOLETE 	      deprecated_update_frame_base_hack (fi, read_sp () - fi->stack_size);
// OBSOLETE 	    }
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Now actually handle the out of line prologue.  */
// OBSOLETE   if (out_of_line_prologue)
// OBSOLETE     {
// OBSOLETE       int outgoing_args_size = 0;
// OBSOLETE 
// OBSOLETE       /* First adjust the stack size for this function.  The out of
// OBSOLETE          line prologue saves 4 registers (16bytes of data).  */
// OBSOLETE       if (fi)
// OBSOLETE 	fi->stack_size -= 16;
// OBSOLETE 
// OBSOLETE       /* Update fi->frame if necessary.  */
// OBSOLETE       if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 	deprecated_update_frame_base_hack (fi, read_sp () - fi->stack_size);
// OBSOLETE 
// OBSOLETE       /* After the out of line prologue, there may be another
// OBSOLETE          stack adjustment for the outgoing arguments.
// OBSOLETE 
// OBSOLETE          Search for add imm8,a3 (0xd3XX)
// OBSOLETE          or     add imm16,a3 (0xf70bXXXX)
// OBSOLETE          or     add imm24,a3 (0xf467XXXXXX).  */
// OBSOLETE 
// OBSOLETE       status = target_read_memory (addr, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	{
// OBSOLETE 	  if (fi)
// OBSOLETE 	    {
// OBSOLETE 	      fi->fsr.regs[2] = get_frame_base (fi) + fi->stack_size + 4;
// OBSOLETE 	      fi->fsr.regs[3] = get_frame_base (fi) + fi->stack_size + 8;
// OBSOLETE 	      fi->fsr.regs[5] = get_frame_base (fi) + fi->stack_size + 12;
// OBSOLETE 	      fi->fsr.regs[6] = get_frame_base (fi) + fi->stack_size + 16;
// OBSOLETE 	    }
// OBSOLETE 	  return addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       if (buf[0] == 0xd3)
// OBSOLETE 	{
// OBSOLETE 	  outgoing_args_size = extract_signed_integer (&buf[1], 1);
// OBSOLETE 	  addr += 2;
// OBSOLETE 	}
// OBSOLETE       else if (buf[0] == 0xf7 && buf[1] == 0x0b)
// OBSOLETE 	{
// OBSOLETE 	  status = target_read_memory (addr + 2, buf, 2);
// OBSOLETE 	  if (status != 0)
// OBSOLETE 	    {
// OBSOLETE 	      if (fi)
// OBSOLETE 		{
// OBSOLETE 		  fi->fsr.regs[2] = get_frame_base (fi) + fi->stack_size + 4;
// OBSOLETE 		  fi->fsr.regs[3] = get_frame_base (fi) + fi->stack_size + 8;
// OBSOLETE 		  fi->fsr.regs[5] = get_frame_base (fi) + fi->stack_size + 12;
// OBSOLETE 		  fi->fsr.regs[6] = get_frame_base (fi) + fi->stack_size + 16;
// OBSOLETE 		}
// OBSOLETE 	      return addr;
// OBSOLETE 	    }
// OBSOLETE 	  outgoing_args_size = extract_signed_integer (buf, 2);
// OBSOLETE 	  addr += 4;
// OBSOLETE 	}
// OBSOLETE       else if (buf[0] == 0xf4 && buf[1] == 0x67)
// OBSOLETE 	{
// OBSOLETE 	  status = target_read_memory (addr + 2, buf, 3);
// OBSOLETE 	  if (status != 0)
// OBSOLETE 	    {
// OBSOLETE 	      if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 		{
// OBSOLETE 		  fi->fsr.regs[2] = get_frame_base (fi) + fi->stack_size + 4;
// OBSOLETE 		  fi->fsr.regs[3] = get_frame_base (fi) + fi->stack_size + 8;
// OBSOLETE 		  fi->fsr.regs[5] = get_frame_base (fi) + fi->stack_size + 12;
// OBSOLETE 		  fi->fsr.regs[6] = get_frame_base (fi) + fi->stack_size + 16;
// OBSOLETE 		}
// OBSOLETE 	      return addr;
// OBSOLETE 	    }
// OBSOLETE 	  outgoing_args_size = extract_signed_integer (buf, 3);
// OBSOLETE 	  addr += 5;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	outgoing_args_size = 0;
// OBSOLETE 
// OBSOLETE       /* Now that we know the size of the outgoing arguments, fix
// OBSOLETE          fi->frame again if this is the innermost frame.  */
// OBSOLETE       if (fi && get_next_frame (fi) == NULL)
// OBSOLETE 	deprecated_update_frame_base_hack (fi, get_frame_base (fi) - outgoing_args_size);
// OBSOLETE 
// OBSOLETE       /* Note the register save information and update the stack
// OBSOLETE          size for this frame too.  */
// OBSOLETE       if (fi)
// OBSOLETE 	{
// OBSOLETE 	  fi->fsr.regs[2] = get_frame_base (fi) + fi->stack_size + 4;
// OBSOLETE 	  fi->fsr.regs[3] = get_frame_base (fi) + fi->stack_size + 8;
// OBSOLETE 	  fi->fsr.regs[5] = get_frame_base (fi) + fi->stack_size + 12;
// OBSOLETE 	  fi->fsr.regs[6] = get_frame_base (fi) + fi->stack_size + 16;
// OBSOLETE 	  fi->stack_size += outgoing_args_size;
// OBSOLETE 	}
// OBSOLETE       /* There can be no more prologue insns, so return now.  */
// OBSOLETE       return addr;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* At this point fi->frame needs to be correct.
// OBSOLETE 
// OBSOLETE      If MY_FRAME_IN_SP is set and we're the innermost frame, then we
// OBSOLETE      need to fix fi->frame so that backtracing, find_frame_saved_regs,
// OBSOLETE      etc work correctly.  */
// OBSOLETE   if (fi && get_next_frame (fi) == NULL && (fi->status & MY_FRAME_IN_SP) != 0)
// OBSOLETE     deprecated_update_frame_base_hack (fi, read_sp () - fi->stack_size);
// OBSOLETE 
// OBSOLETE   /* And last we have the register saves.  These are relatively
// OBSOLETE      simple because they're physically done off the stack pointer,
// OBSOLETE      and thus the number of different instructions we need to
// OBSOLETE      check is greatly reduced because we know the displacements
// OBSOLETE      will be small.
// OBSOLETE 
// OBSOLETE      Search for movx d2,(X,a3) (0xf55eXX)
// OBSOLETE      then       movx d3,(X,a3) (0xf55fXX)
// OBSOLETE      then       mov  a1,(X,a3) (0x5dXX)    No frame pointer case
// OBSOLETE      then       mov  a2,(X,a3) (0x5eXX)    No frame pointer case
// OBSOLETE      or  mov  a0,(X,a3) (0x5cXX)           Frame pointer case.  */
// OBSOLETE 
// OBSOLETE   status = target_read_memory (addr, buf, 2);
// OBSOLETE   if (status != 0)
// OBSOLETE     return addr;
// OBSOLETE   if (buf[0] == 0xf5 && buf[1] == 0x5e)
// OBSOLETE     {
// OBSOLETE       if (fi)
// OBSOLETE 	{
// OBSOLETE 	  status = target_read_memory (addr + 2, buf, 1);
// OBSOLETE 	  if (status != 0)
// OBSOLETE 	    return addr;
// OBSOLETE 	  fi->fsr.regs[2] = (get_frame_base (fi) + stack_size
// OBSOLETE 			     + extract_signed_integer (buf, 1));
// OBSOLETE 	}
// OBSOLETE       addr += 3;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	return addr;
// OBSOLETE       status = target_read_memory (addr, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	return addr;
// OBSOLETE     }
// OBSOLETE   if (buf[0] == 0xf5 && buf[1] == 0x5f)
// OBSOLETE     {
// OBSOLETE       if (fi)
// OBSOLETE 	{
// OBSOLETE 	  status = target_read_memory (addr + 2, buf, 1);
// OBSOLETE 	  if (status != 0)
// OBSOLETE 	    return addr;
// OBSOLETE 	  fi->fsr.regs[3] = (get_frame_base (fi) + stack_size
// OBSOLETE 			     + extract_signed_integer (buf, 1));
// OBSOLETE 	}
// OBSOLETE       addr += 3;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	return addr;
// OBSOLETE       status = target_read_memory (addr, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	return addr;
// OBSOLETE     }
// OBSOLETE   if (buf[0] == 0x5d)
// OBSOLETE     {
// OBSOLETE       if (fi)
// OBSOLETE 	{
// OBSOLETE 	  status = target_read_memory (addr + 1, buf, 1);
// OBSOLETE 	  if (status != 0)
// OBSOLETE 	    return addr;
// OBSOLETE 	  fi->fsr.regs[5] = (get_frame_base (fi) + stack_size
// OBSOLETE 			     + extract_signed_integer (buf, 1));
// OBSOLETE 	}
// OBSOLETE       addr += 2;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	return addr;
// OBSOLETE       status = target_read_memory (addr, buf, 2);
// OBSOLETE       if (status != 0)
// OBSOLETE 	return addr;
// OBSOLETE     }
// OBSOLETE   if (buf[0] == 0x5e || buf[0] == 0x5c)
// OBSOLETE     {
// OBSOLETE       if (fi)
// OBSOLETE 	{
// OBSOLETE 	  status = target_read_memory (addr + 1, buf, 1);
// OBSOLETE 	  if (status != 0)
// OBSOLETE 	    return addr;
// OBSOLETE 	  fi->fsr.regs[6] = (get_frame_base (fi) + stack_size
// OBSOLETE 			     + extract_signed_integer (buf, 1));
// OBSOLETE 	  fi->status &= ~CALLER_A2_IN_A0;
// OBSOLETE 	}
// OBSOLETE       addr += 2;
// OBSOLETE       if (addr >= stop)
// OBSOLETE 	return addr;
// OBSOLETE       return addr;
// OBSOLETE     }
// OBSOLETE   return addr;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: frame_chain
// OBSOLETE    Figure out and return the caller's frame pointer given current
// OBSOLETE    frame_info struct.
// OBSOLETE 
// OBSOLETE    We don't handle dummy frames yet but we would probably just return the
// OBSOLETE    stack pointer that was in use at the time the function call was made?  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE mn10200_frame_chain (struct frame_info *fi)
// OBSOLETE {
// OBSOLETE   struct frame_info *dummy_frame = deprecated_frame_xmalloc ();
// OBSOLETE   struct cleanup *old_chain = make_cleanup (xfree, dummy_frame);
// OBSOLETE   CORE_ADDR ret;
// OBSOLETE 
// OBSOLETE   /* Walk through the prologue to determine the stack size,
// OBSOLETE      location of saved registers, end of the prologue, etc.  */
// OBSOLETE   if (fi->status == 0)
// OBSOLETE     mn10200_analyze_prologue (fi, (CORE_ADDR) 0);
// OBSOLETE 
// OBSOLETE   /* Quit now if mn10200_analyze_prologue set NO_MORE_FRAMES.  */
// OBSOLETE   if (fi->status & NO_MORE_FRAMES)
// OBSOLETE     return 0;
// OBSOLETE 
// OBSOLETE   /* Now that we've analyzed our prologue, determine the frame
// OBSOLETE      pointer for our caller.
// OBSOLETE 
// OBSOLETE      If our caller has a frame pointer, then we need to
// OBSOLETE      find the entry value of $a2 to our function.
// OBSOLETE 
// OBSOLETE      If CALLER_A2_IN_A0, then the chain is in $a0.
// OBSOLETE 
// OBSOLETE      If fsr.regs[6] is nonzero, then it's at the memory
// OBSOLETE      location pointed to by fsr.regs[6].
// OBSOLETE 
// OBSOLETE      Else it's still in $a2.
// OBSOLETE 
// OBSOLETE      If our caller does not have a frame pointer, then his
// OBSOLETE      frame base is fi->frame + -caller's stack size + 4.  */
// OBSOLETE 
// OBSOLETE   /* The easiest way to get that info is to analyze our caller's frame.
// OBSOLETE 
// OBSOLETE      So we set up a dummy frame and call mn10200_analyze_prologue to
// OBSOLETE      find stuff for us.  */
// OBSOLETE   deprecated_update_frame_pc_hack (dummy_frame, FRAME_SAVED_PC (fi));
// OBSOLETE   deprecated_update_frame_base_hack (dummy_frame, get_frame_base (fi));
// OBSOLETE   memset (dummy_frame->fsr.regs, '\000', sizeof dummy_frame->fsr.regs);
// OBSOLETE   dummy_frame->status = 0;
// OBSOLETE   dummy_frame->stack_size = 0;
// OBSOLETE   mn10200_analyze_prologue (dummy_frame, 0);
// OBSOLETE 
// OBSOLETE   if (dummy_frame->status & MY_FRAME_IN_FP)
// OBSOLETE     {
// OBSOLETE       /* Our caller has a frame pointer.  So find the frame in $a2, $a0,
// OBSOLETE          or in the stack.  */
// OBSOLETE       if (fi->fsr.regs[6])
// OBSOLETE 	ret = (read_memory_integer (fi->fsr.regs[FP_REGNUM], REGISTER_SIZE)
// OBSOLETE 	       & 0xffffff);
// OBSOLETE       else if (fi->status & CALLER_A2_IN_A0)
// OBSOLETE 	ret = read_register (4);
// OBSOLETE       else
// OBSOLETE 	ret = read_register (FP_REGNUM);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       /* Our caller does not have a frame pointer.  So his frame starts
// OBSOLETE          at the base of our frame (fi->frame) + <his size> + 4 (saved pc).  */
// OBSOLETE       ret = get_frame_base (fi) + -dummy_frame->stack_size + 4;
// OBSOLETE     }
// OBSOLETE   do_cleanups (old_chain);
// OBSOLETE   return ret;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: skip_prologue
// OBSOLETE    Return the address of the first inst past the prologue of the function.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE mn10200_skip_prologue (CORE_ADDR pc)
// OBSOLETE {
// OBSOLETE   /* We used to check the debug symbols, but that can lose if
// OBSOLETE      we have a null prologue.  */
// OBSOLETE   return mn10200_analyze_prologue (NULL, pc);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: pop_frame
// OBSOLETE    This routine gets called when either the user uses the `return'
// OBSOLETE    command, or the call dummy breakpoint gets hit.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE mn10200_pop_frame (struct frame_info *frame)
// OBSOLETE {
// OBSOLETE   int regnum;
// OBSOLETE 
// OBSOLETE   if (DEPRECATED_PC_IN_CALL_DUMMY (get_frame_pc (frame),
// OBSOLETE 				   get_frame_base (frame),
// OBSOLETE 				   get_frame_base (frame)))
// OBSOLETE     generic_pop_dummy_frame ();
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       write_register (PC_REGNUM, FRAME_SAVED_PC (frame));
// OBSOLETE 
// OBSOLETE       /* Restore any saved registers.  */
// OBSOLETE       for (regnum = 0; regnum < NUM_REGS; regnum++)
// OBSOLETE 	if (frame->fsr.regs[regnum] != 0)
// OBSOLETE 	  {
// OBSOLETE 	    ULONGEST value;
// OBSOLETE 
// OBSOLETE 	    value = read_memory_unsigned_integer (frame->fsr.regs[regnum],
// OBSOLETE 						REGISTER_RAW_SIZE (regnum));
// OBSOLETE 	    write_register (regnum, value);
// OBSOLETE 	  }
// OBSOLETE 
// OBSOLETE       /* Actually cut back the stack.  */
// OBSOLETE       write_register (SP_REGNUM, get_frame_base (frame));
// OBSOLETE 
// OBSOLETE       /* Don't we need to set the PC?!?  XXX FIXME.  */
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Throw away any cached frame information.  */
// OBSOLETE   flush_cached_frames ();
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: push_arguments
// OBSOLETE    Setup arguments for a call to the target.  Arguments go in
// OBSOLETE    order on the stack.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE mn10200_push_arguments (int nargs, struct value **args, CORE_ADDR sp,
// OBSOLETE 			unsigned char struct_return, CORE_ADDR struct_addr)
// OBSOLETE {
// OBSOLETE   int argnum = 0;
// OBSOLETE   int len = 0;
// OBSOLETE   int stack_offset = 0;
// OBSOLETE   int regsused = struct_return ? 1 : 0;
// OBSOLETE 
// OBSOLETE   /* This should be a nop, but align the stack just in case something
// OBSOLETE      went wrong.  Stacks are two byte aligned on the mn10200.  */
// OBSOLETE   sp &= ~1;
// OBSOLETE 
// OBSOLETE   /* Now make space on the stack for the args.
// OBSOLETE 
// OBSOLETE      XXX This doesn't appear to handle pass-by-invisible reference
// OBSOLETE      arguments.  */
// OBSOLETE   for (argnum = 0; argnum < nargs; argnum++)
// OBSOLETE     {
// OBSOLETE       int arg_length = (TYPE_LENGTH (VALUE_TYPE (args[argnum])) + 1) & ~1;
// OBSOLETE 
// OBSOLETE       /* If we've used all argument registers, then this argument is
// OBSOLETE          pushed.  */
// OBSOLETE       if (regsused >= 2 || arg_length > 4)
// OBSOLETE 	{
// OBSOLETE 	  regsused = 2;
// OBSOLETE 	  len += arg_length;
// OBSOLETE 	}
// OBSOLETE       /* We know we've got some arg register space left.  If this argument
// OBSOLETE          will fit entirely in regs, then put it there.  */
// OBSOLETE       else if (arg_length <= 2
// OBSOLETE 	       || TYPE_CODE (VALUE_TYPE (args[argnum])) == TYPE_CODE_PTR)
// OBSOLETE 	{
// OBSOLETE 	  regsused++;
// OBSOLETE 	}
// OBSOLETE       else if (regsused == 0)
// OBSOLETE 	{
// OBSOLETE 	  regsused = 2;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  regsused = 2;
// OBSOLETE 	  len += arg_length;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   /* Allocate stack space.  */
// OBSOLETE   sp -= len;
// OBSOLETE 
// OBSOLETE   regsused = struct_return ? 1 : 0;
// OBSOLETE   /* Push all arguments onto the stack. */
// OBSOLETE   for (argnum = 0; argnum < nargs; argnum++)
// OBSOLETE     {
// OBSOLETE       int len;
// OBSOLETE       char *val;
// OBSOLETE 
// OBSOLETE       /* XXX Check this.  What about UNIONS?  */
// OBSOLETE       if (TYPE_CODE (VALUE_TYPE (*args)) == TYPE_CODE_STRUCT
// OBSOLETE 	  && TYPE_LENGTH (VALUE_TYPE (*args)) > 8)
// OBSOLETE 	{
// OBSOLETE 	  /* XXX Wrong, we want a pointer to this argument.  */
// OBSOLETE 	  len = TYPE_LENGTH (VALUE_TYPE (*args));
// OBSOLETE 	  val = (char *) VALUE_CONTENTS (*args);
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  len = TYPE_LENGTH (VALUE_TYPE (*args));
// OBSOLETE 	  val = (char *) VALUE_CONTENTS (*args);
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       if (regsused < 2
// OBSOLETE 	  && (len <= 2
// OBSOLETE 	      || TYPE_CODE (VALUE_TYPE (*args)) == TYPE_CODE_PTR))
// OBSOLETE 	{
// OBSOLETE 	  write_register (regsused, extract_unsigned_integer (val, 4));
// OBSOLETE 	  regsused++;
// OBSOLETE 	}
// OBSOLETE       else if (regsused == 0 && len == 4)
// OBSOLETE 	{
// OBSOLETE 	  write_register (regsused, extract_unsigned_integer (val, 2));
// OBSOLETE 	  write_register (regsused + 1, extract_unsigned_integer (val + 2, 2));
// OBSOLETE 	  regsused = 2;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  regsused = 2;
// OBSOLETE 	  while (len > 0)
// OBSOLETE 	    {
// OBSOLETE 	      write_memory (sp + stack_offset, val, 2);
// OBSOLETE 
// OBSOLETE 	      len -= 2;
// OBSOLETE 	      val += 2;
// OBSOLETE 	      stack_offset += 2;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE       args++;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return sp;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: push_return_address (pc)
// OBSOLETE    Set up the return address for the inferior function call.
// OBSOLETE    Needed for targets where we don't actually execute a JSR/BSR instruction */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE mn10200_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
// OBSOLETE {
// OBSOLETE   unsigned char buf[4];
// OBSOLETE 
// OBSOLETE   store_unsigned_integer (buf, 4, CALL_DUMMY_ADDRESS ());
// OBSOLETE   write_memory (sp - 4, buf, 4);
// OBSOLETE   return sp - 4;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: store_struct_return (addr,sp)
// OBSOLETE    Store the structure value return address for an inferior function
// OBSOLETE    call.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE mn10200_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
// OBSOLETE {
// OBSOLETE   /* The structure return address is passed as the first argument.  */
// OBSOLETE   write_register (0, addr);
// OBSOLETE   return sp;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: frame_saved_pc 
// OBSOLETE    Find the caller of this frame.  We do this by seeing if RP_REGNUM
// OBSOLETE    is saved in the stack anywhere, otherwise we get it from the
// OBSOLETE    registers.  If the inner frame is a dummy frame, return its PC
// OBSOLETE    instead of RP, because that's where "caller" of the dummy-frame
// OBSOLETE    will be found.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE mn10200_frame_saved_pc (struct frame_info *fi)
// OBSOLETE {
// OBSOLETE   /* The saved PC will always be at the base of the current frame.  */
// OBSOLETE   return (read_memory_integer (get_frame_base (fi), REGISTER_SIZE) & 0xffffff);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: init_extra_frame_info
// OBSOLETE    Setup the frame's frame pointer, pc, and frame addresses for saved
// OBSOLETE    registers.  Most of the work is done in mn10200_analyze_prologue().
// OBSOLETE 
// OBSOLETE    Note that when we are called for the last frame (currently active frame),
// OBSOLETE    that get_frame_pc (fi) and fi->frame will already be setup.  However, fi->frame will
// OBSOLETE    be valid only if this routine uses FP.  For previous frames, fi-frame will
// OBSOLETE    always be correct.  mn10200_analyze_prologue will fix fi->frame if
// OBSOLETE    it's not valid.
// OBSOLETE 
// OBSOLETE    We can be called with the PC in the call dummy under two circumstances.
// OBSOLETE    First, during normal backtracing, second, while figuring out the frame
// OBSOLETE    pointer just prior to calling the target function (see run_stack_dummy).  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE mn10200_init_extra_frame_info (struct frame_info *fi)
// OBSOLETE {
// OBSOLETE   if (get_next_frame (fi))
// OBSOLETE     deprecated_update_frame_pc_hack (fi, FRAME_SAVED_PC (get_next_frame (fi)));
// OBSOLETE 
// OBSOLETE   memset (fi->fsr.regs, '\000', sizeof fi->fsr.regs);
// OBSOLETE   fi->status = 0;
// OBSOLETE   fi->stack_size = 0;
// OBSOLETE 
// OBSOLETE   mn10200_analyze_prologue (fi, 0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_mn10200_tdep (void)
// OBSOLETE {
// OBSOLETE   tm_print_insn = print_insn_mn10200;
// OBSOLETE }
