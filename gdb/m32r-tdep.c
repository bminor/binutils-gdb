// OBSOLETE /* Target-dependent code for the Mitsubishi m32r for GDB, the GNU debugger.
// OBSOLETE 
// OBSOLETE    Copyright 1996, 1998, 1999, 2000, 2001, 2003 Free Software
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
// OBSOLETE /* Function: m32r_use_struct_convention
// OBSOLETE    Return nonzero if call_function should allocate stack space for a
// OBSOLETE    struct return? */
// OBSOLETE int
// OBSOLETE m32r_use_struct_convention (int gcc_p, struct type *type)
// OBSOLETE {
// OBSOLETE   return (TYPE_LENGTH (type) > 8);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: frame_find_saved_regs
// OBSOLETE    Return the frame_saved_regs structure for the frame.
// OBSOLETE    Doesn't really work for dummy frames, but it does pass back
// OBSOLETE    an empty frame_saved_regs, so I guess that's better than total failure */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE m32r_frame_find_saved_regs (struct frame_info *fi,
// OBSOLETE 			    struct frame_saved_regs *regaddr)
// OBSOLETE {
// OBSOLETE   memcpy (regaddr, &fi->fsr, sizeof (struct frame_saved_regs));
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Turn this on if you want to see just how much instruction decoding
// OBSOLETE    if being done, its quite a lot
// OBSOLETE  */
// OBSOLETE #if 0
// OBSOLETE static void
// OBSOLETE dump_insn (char *commnt, CORE_ADDR pc, int insn)
// OBSOLETE {
// OBSOLETE   printf_filtered ("  %s %08x %08x ",
// OBSOLETE 		   commnt, (unsigned int) pc, (unsigned int) insn);
// OBSOLETE   TARGET_PRINT_INSN (pc, &tm_print_insn_info);
// OBSOLETE   printf_filtered ("\n");
// OBSOLETE }
// OBSOLETE #define insn_debug(args) { printf_filtered args; }
// OBSOLETE #else
// OBSOLETE #define dump_insn(a,b,c) {}
// OBSOLETE #define insn_debug(args) {}
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE #define DEFAULT_SEARCH_LIMIT 44
// OBSOLETE 
// OBSOLETE /* Function: scan_prologue
// OBSOLETE    This function decodes the target function prologue to determine
// OBSOLETE    1) the size of the stack frame, and 2) which registers are saved on it.
// OBSOLETE    It saves the offsets of saved regs in the frame_saved_regs argument,
// OBSOLETE    and returns the frame size.  */
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE    The sequence it currently generates is:
// OBSOLETE 
// OBSOLETE    if (varargs function) { ddi sp,#n }
// OBSOLETE    push registers
// OBSOLETE    if (additional stack <= 256) {       addi sp,#-stack }
// OBSOLETE    else if (additional stack < 65k) { add3 sp,sp,#-stack
// OBSOLETE 
// OBSOLETE    } else if (additional stack) {
// OBSOLETE    seth sp,#(stack & 0xffff0000)
// OBSOLETE    or3 sp,sp,#(stack & 0x0000ffff)
// OBSOLETE    sub sp,r4
// OBSOLETE    }
// OBSOLETE    if (frame pointer) {
// OBSOLETE    mv sp,fp
// OBSOLETE    }
// OBSOLETE 
// OBSOLETE    These instructions are scheduled like everything else, so you should stop at
// OBSOLETE    the first branch instruction.
// OBSOLETE 
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE /* This is required by skip prologue and by m32r_init_extra_frame_info. 
// OBSOLETE    The results of decoding a prologue should be cached because this
// OBSOLETE    thrashing is getting nuts.
// OBSOLETE    I am thinking of making a container class with two indexes, name and
// OBSOLETE    address. It may be better to extend the symbol table.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE decode_prologue (CORE_ADDR start_pc, CORE_ADDR scan_limit, CORE_ADDR *pl_endptr,	/* var parameter */
// OBSOLETE 		 unsigned long *framelength, struct frame_info *fi,
// OBSOLETE 		 struct frame_saved_regs *fsr)
// OBSOLETE {
// OBSOLETE   unsigned long framesize;
// OBSOLETE   int insn;
// OBSOLETE   int op1;
// OBSOLETE   int maybe_one_more = 0;
// OBSOLETE   CORE_ADDR after_prologue = 0;
// OBSOLETE   CORE_ADDR after_stack_adjust = 0;
// OBSOLETE   CORE_ADDR current_pc;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   framesize = 0;
// OBSOLETE   after_prologue = 0;
// OBSOLETE   insn_debug (("rd prolog l(%d)\n", scan_limit - current_pc));
// OBSOLETE 
// OBSOLETE   for (current_pc = start_pc; current_pc < scan_limit; current_pc += 2)
// OBSOLETE     {
// OBSOLETE 
// OBSOLETE       insn = read_memory_unsigned_integer (current_pc, 2);
// OBSOLETE       dump_insn ("insn-1", current_pc, insn);	/* MTZ */
// OBSOLETE 
// OBSOLETE       /* If this is a 32 bit instruction, we dont want to examine its
// OBSOLETE          immediate data as though it were an instruction */
// OBSOLETE       if (current_pc & 0x02)
// OBSOLETE 	{			/* Clear the parallel execution bit from 16 bit instruction */
// OBSOLETE 	  if (maybe_one_more)
// OBSOLETE 	    {			/* The last instruction was a branch, usually terminates
// OBSOLETE 				   the series, but if this is a parallel instruction,
// OBSOLETE 				   it may be a stack framing instruction */
// OBSOLETE 	      if (!(insn & 0x8000))
// OBSOLETE 		{
// OBSOLETE 		  insn_debug (("Really done"));
// OBSOLETE 		  break;	/* nope, we are really done */
// OBSOLETE 		}
// OBSOLETE 	    }
// OBSOLETE 	  insn &= 0x7fff;	/* decode this instruction further */
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  if (maybe_one_more)
// OBSOLETE 	    break;		/* This isnt the one more */
// OBSOLETE 	  if (insn & 0x8000)
// OBSOLETE 	    {
// OBSOLETE 	      insn_debug (("32 bit insn\n"));
// OBSOLETE 	      if (current_pc == scan_limit)
// OBSOLETE 		scan_limit += 2;	/* extend the search */
// OBSOLETE 	      current_pc += 2;	/* skip the immediate data */
// OBSOLETE 	      if (insn == 0x8faf)	/* add3 sp, sp, xxxx */
// OBSOLETE 		/* add 16 bit sign-extended offset */
// OBSOLETE 		{
// OBSOLETE 		  insn_debug (("stack increment\n"));
// OBSOLETE 		  framesize += -((short) read_memory_unsigned_integer (current_pc, 2));
// OBSOLETE 		}
// OBSOLETE 	      else
// OBSOLETE 		{
// OBSOLETE 		  if (((insn >> 8) == 0xe4) &&	/* ld24 r4, xxxxxx; sub sp, r4 */
// OBSOLETE 		  read_memory_unsigned_integer (current_pc + 2, 2) == 0x0f24)
// OBSOLETE 		    {		/* subtract 24 bit sign-extended negative-offset */
// OBSOLETE 		      dump_insn ("insn-2", current_pc + 2, insn);
// OBSOLETE 		      insn = read_memory_unsigned_integer (current_pc - 2, 4);
// OBSOLETE 		      dump_insn ("insn-3(l4)", current_pc - 2, insn);
// OBSOLETE 		      if (insn & 0x00800000)	/* sign extend */
// OBSOLETE 			insn |= 0xff000000;	/* negative */
// OBSOLETE 		      else
// OBSOLETE 			insn &= 0x00ffffff;	/* positive */
// OBSOLETE 		      framesize += insn;
// OBSOLETE 		    }
// OBSOLETE 		}
// OBSOLETE 	      after_prologue = current_pc;
// OBSOLETE 	      continue;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE       op1 = insn & 0xf000;	/* isolate just the first nibble */
// OBSOLETE 
// OBSOLETE       if ((insn & 0xf0ff) == 0x207f)
// OBSOLETE 	{			/* st reg, @-sp */
// OBSOLETE 	  int regno;
// OBSOLETE 	  insn_debug (("push\n"));
// OBSOLETE #if 0				/* No, PUSH FP is not an indication that we will use a frame pointer. */
// OBSOLETE 	  if (((insn & 0xffff) == 0x2d7f) && fi)
// OBSOLETE 	    fi->using_frame_pointer = 1;
// OBSOLETE #endif
// OBSOLETE 	  framesize += 4;
// OBSOLETE #if 0
// OBSOLETE /* Why should we increase the scan limit, just because we did a push? 
// OBSOLETE    And if there is a reason, surely we would only want to do it if we
// OBSOLETE    had already reached the scan limit... */
// OBSOLETE 	  if (current_pc == scan_limit)
// OBSOLETE 	    scan_limit += 2;
// OBSOLETE #endif
// OBSOLETE 	  regno = ((insn >> 8) & 0xf);
// OBSOLETE 	  if (fsr)		/* save_regs offset */
// OBSOLETE 	    fsr->regs[regno] = framesize;
// OBSOLETE 	  after_prologue = 0;
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE       if ((insn >> 8) == 0x4f)	/* addi sp, xx */
// OBSOLETE 	/* add 8 bit sign-extended offset */
// OBSOLETE 	{
// OBSOLETE 	  int stack_adjust = (char) (insn & 0xff);
// OBSOLETE 
// OBSOLETE 	  /* there are probably two of these stack adjustments:
// OBSOLETE 	     1) A negative one in the prologue, and
// OBSOLETE 	     2) A positive one in the epilogue.
// OBSOLETE 	     We are only interested in the first one.  */
// OBSOLETE 
// OBSOLETE 	  if (stack_adjust < 0)
// OBSOLETE 	    {
// OBSOLETE 	      framesize -= stack_adjust;
// OBSOLETE 	      after_prologue = 0;
// OBSOLETE 	      /* A frameless function may have no "mv fp, sp".
// OBSOLETE 	         In that case, this is the end of the prologue.  */
// OBSOLETE 	      after_stack_adjust = current_pc + 2;
// OBSOLETE 	    }
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE       if (insn == 0x1d8f)
// OBSOLETE 	{			/* mv fp, sp */
// OBSOLETE 	  if (fi)
// OBSOLETE 	    fi->using_frame_pointer = 1;	/* fp is now valid */
// OBSOLETE 	  insn_debug (("done fp found\n"));
// OBSOLETE 	  after_prologue = current_pc + 2;
// OBSOLETE 	  break;		/* end of stack adjustments */
// OBSOLETE 	}
// OBSOLETE       if (insn == 0x7000)	/* Nop looks like a branch, continue explicitly */
// OBSOLETE 	{
// OBSOLETE 	  insn_debug (("nop\n"));
// OBSOLETE 	  after_prologue = current_pc + 2;
// OBSOLETE 	  continue;		/* nop occurs between pushes */
// OBSOLETE 	}
// OBSOLETE       /* End of prolog if any of these are branch instructions */
// OBSOLETE       if ((op1 == 0x7000)
// OBSOLETE 	  || (op1 == 0xb000)
// OBSOLETE 	  || (op1 == 0xf000))
// OBSOLETE 	{
// OBSOLETE 	  after_prologue = current_pc;
// OBSOLETE 	  insn_debug (("Done: branch\n"));
// OBSOLETE 	  maybe_one_more = 1;
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE       /* Some of the branch instructions are mixed with other types */
// OBSOLETE       if (op1 == 0x1000)
// OBSOLETE 	{
// OBSOLETE 	  int subop = insn & 0x0ff0;
// OBSOLETE 	  if ((subop == 0x0ec0) || (subop == 0x0fc0))
// OBSOLETE 	    {
// OBSOLETE 	      insn_debug (("done: jmp\n"));
// OBSOLETE 	      after_prologue = current_pc;
// OBSOLETE 	      maybe_one_more = 1;
// OBSOLETE 	      continue;		/* jmp , jl */
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (current_pc >= scan_limit)
// OBSOLETE     {
// OBSOLETE       if (pl_endptr)
// OBSOLETE 	{
// OBSOLETE #if 1
// OBSOLETE 	  if (after_stack_adjust != 0)
// OBSOLETE 	    /* We did not find a "mv fp,sp", but we DID find
// OBSOLETE 	       a stack_adjust.  Is it safe to use that as the
// OBSOLETE 	       end of the prologue?  I just don't know. */
// OBSOLETE 	    {
// OBSOLETE 	      *pl_endptr = after_stack_adjust;
// OBSOLETE 	      if (framelength)
// OBSOLETE 		*framelength = framesize;
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE #endif
// OBSOLETE 	    /* We reached the end of the loop without finding the end
// OBSOLETE 	       of the prologue.  No way to win -- we should report failure.  
// OBSOLETE 	       The way we do that is to return the original start_pc.
// OBSOLETE 	       GDB will set a breakpoint at the start of the function (etc.) */
// OBSOLETE 	    *pl_endptr = start_pc;
// OBSOLETE 	}
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE   if (after_prologue == 0)
// OBSOLETE     after_prologue = current_pc;
// OBSOLETE 
// OBSOLETE   insn_debug ((" framesize %d, firstline %08x\n", framesize, after_prologue));
// OBSOLETE   if (framelength)
// OBSOLETE     *framelength = framesize;
// OBSOLETE   if (pl_endptr)
// OBSOLETE     *pl_endptr = after_prologue;
// OBSOLETE }				/*  decode_prologue */
// OBSOLETE 
// OBSOLETE /* Function: skip_prologue
// OBSOLETE    Find end of function prologue */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE m32r_skip_prologue (CORE_ADDR pc)
// OBSOLETE {
// OBSOLETE   CORE_ADDR func_addr, func_end;
// OBSOLETE   struct symtab_and_line sal;
// OBSOLETE 
// OBSOLETE   /* See what the symbol table says */
// OBSOLETE 
// OBSOLETE   if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
// OBSOLETE     {
// OBSOLETE       sal = find_pc_line (func_addr, 0);
// OBSOLETE 
// OBSOLETE       if (sal.line != 0 && sal.end <= func_end)
// OBSOLETE 	{
// OBSOLETE 
// OBSOLETE 	  insn_debug (("BP after prologue %08x\n", sal.end));
// OBSOLETE 	  func_end = sal.end;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	/* Either there's no line info, or the line after the prologue is after
// OBSOLETE 	   the end of the function.  In this case, there probably isn't a
// OBSOLETE 	   prologue.  */
// OBSOLETE 	{
// OBSOLETE 	  insn_debug (("No line info, line(%x) sal_end(%x) funcend(%x)\n",
// OBSOLETE 		       sal.line, sal.end, func_end));
// OBSOLETE 	  func_end = min (func_end, func_addr + DEFAULT_SEARCH_LIMIT);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     func_end = pc + DEFAULT_SEARCH_LIMIT;
// OBSOLETE   decode_prologue (pc, func_end, &sal.end, 0, 0, 0);
// OBSOLETE   return sal.end;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static unsigned long
// OBSOLETE m32r_scan_prologue (struct frame_info *fi, struct frame_saved_regs *fsr)
// OBSOLETE {
// OBSOLETE   struct symtab_and_line sal;
// OBSOLETE   CORE_ADDR prologue_start, prologue_end, current_pc;
// OBSOLETE   unsigned long framesize = 0;
// OBSOLETE 
// OBSOLETE   /* this code essentially duplicates skip_prologue, 
// OBSOLETE      but we need the start address below.  */
// OBSOLETE 
// OBSOLETE   if (find_pc_partial_function (fi->pc, NULL, &prologue_start, &prologue_end))
// OBSOLETE     {
// OBSOLETE       sal = find_pc_line (prologue_start, 0);
// OBSOLETE 
// OBSOLETE       if (sal.line == 0)	/* no line info, use current PC */
// OBSOLETE 	if (prologue_start == entry_point_address ())
// OBSOLETE 	  return 0;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       prologue_start = fi->pc;
// OBSOLETE       prologue_end = prologue_start + 48;	/* We're in the boondocks: 
// OBSOLETE 						   allow for 16 pushes, an add, 
// OBSOLETE 						   and "mv fp,sp" */
// OBSOLETE     }
// OBSOLETE #if 0
// OBSOLETE   prologue_end = min (prologue_end, fi->pc);
// OBSOLETE #endif
// OBSOLETE   insn_debug (("fipc(%08x) start(%08x) end(%08x)\n",
// OBSOLETE 	       fi->pc, prologue_start, prologue_end));
// OBSOLETE   prologue_end = min (prologue_end, prologue_start + DEFAULT_SEARCH_LIMIT);
// OBSOLETE   decode_prologue (prologue_start, prologue_end, &prologue_end, &framesize,
// OBSOLETE 		   fi, fsr);
// OBSOLETE   return framesize;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: init_extra_frame_info
// OBSOLETE    This function actually figures out the frame address for a given pc and
// OBSOLETE    sp.  This is tricky on the m32r because we sometimes don't use an explicit
// OBSOLETE    frame pointer, and the previous stack pointer isn't necessarily recorded
// OBSOLETE    on the stack.  The only reliable way to get this info is to
// OBSOLETE    examine the prologue.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE m32r_init_extra_frame_info (struct frame_info *fi)
// OBSOLETE {
// OBSOLETE   int reg;
// OBSOLETE 
// OBSOLETE   if (fi->next)
// OBSOLETE     fi->pc = FRAME_SAVED_PC (fi->next);
// OBSOLETE 
// OBSOLETE   memset (fi->fsr.regs, '\000', sizeof fi->fsr.regs);
// OBSOLETE 
// OBSOLETE   if (DEPRECATED_PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
// OBSOLETE     {
// OBSOLETE       /* We need to setup fi->frame here because run_stack_dummy gets it wrong
// OBSOLETE          by assuming it's always FP.  */
// OBSOLETE       fi->frame = deprecated_read_register_dummy (fi->pc, fi->frame,
// OBSOLETE 						  SP_REGNUM);
// OBSOLETE       fi->framesize = 0;
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       fi->using_frame_pointer = 0;
// OBSOLETE       fi->framesize = m32r_scan_prologue (fi, &fi->fsr);
// OBSOLETE 
// OBSOLETE       if (!fi->next)
// OBSOLETE 	if (fi->using_frame_pointer)
// OBSOLETE 	  {
// OBSOLETE 	    fi->frame = read_register (FP_REGNUM);
// OBSOLETE 	  }
// OBSOLETE 	else
// OBSOLETE 	  fi->frame = read_register (SP_REGNUM);
// OBSOLETE       else
// OBSOLETE 	/* fi->next means this is not the innermost frame */ if (fi->using_frame_pointer)
// OBSOLETE 	/* we have an FP */
// OBSOLETE 	if (fi->next->fsr.regs[FP_REGNUM] != 0)		/* caller saved our FP */
// OBSOLETE 	  fi->frame = read_memory_integer (fi->next->fsr.regs[FP_REGNUM], 4);
// OBSOLETE       for (reg = 0; reg < NUM_REGS; reg++)
// OBSOLETE 	if (fi->fsr.regs[reg] != 0)
// OBSOLETE 	  fi->fsr.regs[reg] = fi->frame + fi->framesize - fi->fsr.regs[reg];
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: m32r_virtual_frame_pointer
// OBSOLETE    Return the register that the function uses for a frame pointer, 
// OBSOLETE    plus any necessary offset to be applied to the register before
// OBSOLETE    any frame pointer offsets.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE m32r_virtual_frame_pointer (CORE_ADDR pc, long *reg, long *offset)
// OBSOLETE {
// OBSOLETE   struct frame_info *fi = deprecated_frame_xmalloc ();
// OBSOLETE   struct cleanup *old_chain = make_cleanup (xfree, fi);
// OBSOLETE 
// OBSOLETE   /* Set up a dummy frame_info. */
// OBSOLETE   fi->next = NULL;
// OBSOLETE   fi->prev = NULL;
// OBSOLETE   fi->frame = 0;
// OBSOLETE   fi->pc = pc;
// OBSOLETE 
// OBSOLETE   /* Analyze the prolog and fill in the extra info.  */
// OBSOLETE   m32r_init_extra_frame_info (fi);
// OBSOLETE 
// OBSOLETE   /* Results will tell us which type of frame it uses.  */
// OBSOLETE   if (fi->using_frame_pointer)
// OBSOLETE     {
// OBSOLETE       *reg = FP_REGNUM;
// OBSOLETE       *offset = 0;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       *reg = SP_REGNUM;
// OBSOLETE       *offset = 0;
// OBSOLETE     }
// OBSOLETE   do_cleanups (old_chain);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: find_callers_reg
// OBSOLETE    Find REGNUM on the stack.  Otherwise, it's in an active register.  One thing
// OBSOLETE    we might want to do here is to check REGNUM against the clobber mask, and
// OBSOLETE    somehow flag it as invalid if it isn't saved on the stack somewhere.  This
// OBSOLETE    would provide a graceful failure mode when trying to get the value of
// OBSOLETE    caller-saves registers for an inner frame.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE m32r_find_callers_reg (struct frame_info *fi, int regnum)
// OBSOLETE {
// OBSOLETE   for (; fi; fi = fi->next)
// OBSOLETE     if (DEPRECATED_PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
// OBSOLETE       return deprecated_read_register_dummy (fi->pc, fi->frame, regnum);
// OBSOLETE     else if (fi->fsr.regs[regnum] != 0)
// OBSOLETE       return read_memory_integer (fi->fsr.regs[regnum],
// OBSOLETE 				  REGISTER_RAW_SIZE (regnum));
// OBSOLETE   return read_register (regnum);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: frame_chain Given a GDB frame, determine the address of
// OBSOLETE    the calling function's frame.  This will be used to create a new
// OBSOLETE    GDB frame struct, and then INIT_EXTRA_FRAME_INFO and
// OBSOLETE    DEPRECATED_INIT_FRAME_PC will be called for the new frame.  For
// OBSOLETE    m32r, we save the frame size when we initialize the frame_info.  */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE m32r_frame_chain (struct frame_info *fi)
// OBSOLETE {
// OBSOLETE   CORE_ADDR fn_start, callers_pc, fp;
// OBSOLETE 
// OBSOLETE   /* is this a dummy frame? */
// OBSOLETE   if (DEPRECATED_PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
// OBSOLETE     return fi->frame;		/* dummy frame same as caller's frame */
// OBSOLETE 
// OBSOLETE   /* is caller-of-this a dummy frame? */
// OBSOLETE   callers_pc = FRAME_SAVED_PC (fi);	/* find out who called us: */
// OBSOLETE   fp = m32r_find_callers_reg (fi, FP_REGNUM);
// OBSOLETE   if (DEPRECATED_PC_IN_CALL_DUMMY (callers_pc, fp, fp))
// OBSOLETE     return fp;			/* dummy frame's frame may bear no relation to ours */
// OBSOLETE 
// OBSOLETE   if (find_pc_partial_function (fi->pc, 0, &fn_start, 0))
// OBSOLETE     if (fn_start == entry_point_address ())
// OBSOLETE       return 0;			/* in _start fn, don't chain further */
// OBSOLETE   if (fi->framesize == 0)
// OBSOLETE     {
// OBSOLETE       printf_filtered ("cannot determine frame size @ %s , pc(%s)\n",
// OBSOLETE 		       paddr (fi->frame),
// OBSOLETE 		       paddr (fi->pc));
// OBSOLETE       return 0;
// OBSOLETE     }
// OBSOLETE   insn_debug (("m32rx frame %08x\n", fi->frame + fi->framesize));
// OBSOLETE   return fi->frame + fi->framesize;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: push_return_address (pc)
// OBSOLETE    Set up the return address for the inferior function call.
// OBSOLETE    Necessary for targets that don't actually execute a JSR/BSR instruction 
// OBSOLETE    (ie. when using an empty CALL_DUMMY) */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE m32r_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
// OBSOLETE {
// OBSOLETE   write_register (RP_REGNUM, CALL_DUMMY_ADDRESS ());
// OBSOLETE   return sp;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Function: pop_frame
// OBSOLETE    Discard from the stack the innermost frame,
// OBSOLETE    restoring all saved registers.  */
// OBSOLETE 
// OBSOLETE struct frame_info *
// OBSOLETE m32r_pop_frame (struct frame_info *frame)
// OBSOLETE {
// OBSOLETE   int regnum;
// OBSOLETE 
// OBSOLETE   if (DEPRECATED_PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
// OBSOLETE     generic_pop_dummy_frame ();
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       for (regnum = 0; regnum < NUM_REGS; regnum++)
// OBSOLETE 	if (frame->fsr.regs[regnum] != 0)
// OBSOLETE 	  write_register (regnum,
// OBSOLETE 			  read_memory_integer (frame->fsr.regs[regnum], 4));
// OBSOLETE 
// OBSOLETE       write_register (PC_REGNUM, FRAME_SAVED_PC (frame));
// OBSOLETE       write_register (SP_REGNUM, read_register (FP_REGNUM));
// OBSOLETE       if (read_register (PSW_REGNUM) & 0x80)
// OBSOLETE 	write_register (SPU_REGNUM, read_register (SP_REGNUM));
// OBSOLETE       else
// OBSOLETE 	write_register (SPI_REGNUM, read_register (SP_REGNUM));
// OBSOLETE     }
// OBSOLETE   flush_cached_frames ();
// OBSOLETE   return NULL;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: frame_saved_pc
// OBSOLETE    Find the caller of this frame.  We do this by seeing if RP_REGNUM is saved
// OBSOLETE    in the stack anywhere, otherwise we get it from the registers. */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE m32r_frame_saved_pc (struct frame_info *fi)
// OBSOLETE {
// OBSOLETE   if (DEPRECATED_PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
// OBSOLETE     return deprecated_read_register_dummy (fi->pc, fi->frame, PC_REGNUM);
// OBSOLETE   else
// OBSOLETE     return m32r_find_callers_reg (fi, RP_REGNUM);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: push_arguments
// OBSOLETE    Setup the function arguments for calling a function in the inferior.
// OBSOLETE 
// OBSOLETE    On the Mitsubishi M32R architecture, there are four registers (R0 to R3)
// OBSOLETE    which are dedicated for passing function arguments.  Up to the first 
// OBSOLETE    four arguments (depending on size) may go into these registers.
// OBSOLETE    The rest go on the stack.
// OBSOLETE 
// OBSOLETE    Arguments that are smaller than 4 bytes will still take up a whole
// OBSOLETE    register or a whole 32-bit word on the stack, and will be
// OBSOLETE    right-justified in the register or the stack word.  This includes
// OBSOLETE    chars, shorts, and small aggregate types.
// OBSOLETE 
// OBSOLETE    Arguments of 8 bytes size are split between two registers, if 
// OBSOLETE    available.  If only one register is available, the argument will 
// OBSOLETE    be split between the register and the stack.  Otherwise it is
// OBSOLETE    passed entirely on the stack.  Aggregate types with sizes between
// OBSOLETE    4 and 8 bytes are passed entirely on the stack, and are left-justified
// OBSOLETE    within the double-word (as opposed to aggregates smaller than 4 bytes
// OBSOLETE    which are right-justified).
// OBSOLETE 
// OBSOLETE    Aggregates of greater than 8 bytes are first copied onto the stack, 
// OBSOLETE    and then a pointer to the copy is passed in the place of the normal
// OBSOLETE    argument (either in a register if available, or on the stack).
// OBSOLETE 
// OBSOLETE    Functions that must return an aggregate type can return it in the 
// OBSOLETE    normal return value registers (R0 and R1) if its size is 8 bytes or
// OBSOLETE    less.  For larger return values, the caller must allocate space for 
// OBSOLETE    the callee to copy the return value to.  A pointer to this space is
// OBSOLETE    passed as an implicit first argument, always in R0. */
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE m32r_push_arguments (int nargs, struct value **args, CORE_ADDR sp,
// OBSOLETE 		     unsigned char struct_return, CORE_ADDR struct_addr)
// OBSOLETE {
// OBSOLETE   int stack_offset, stack_alloc;
// OBSOLETE   int argreg;
// OBSOLETE   int argnum;
// OBSOLETE   struct type *type;
// OBSOLETE   CORE_ADDR regval;
// OBSOLETE   char *val;
// OBSOLETE   char valbuf[4];
// OBSOLETE   int len;
// OBSOLETE   int odd_sized_struct;
// OBSOLETE 
// OBSOLETE   /* first force sp to a 4-byte alignment */
// OBSOLETE   sp = sp & ~3;
// OBSOLETE 
// OBSOLETE   argreg = ARG0_REGNUM;
// OBSOLETE   /* The "struct return pointer" pseudo-argument goes in R0 */
// OBSOLETE   if (struct_return)
// OBSOLETE     write_register (argreg++, struct_addr);
// OBSOLETE 
// OBSOLETE   /* Now make sure there's space on the stack */
// OBSOLETE   for (argnum = 0, stack_alloc = 0;
// OBSOLETE        argnum < nargs; argnum++)
// OBSOLETE     stack_alloc += ((TYPE_LENGTH (VALUE_TYPE (args[argnum])) + 3) & ~3);
// OBSOLETE   sp -= stack_alloc;		/* make room on stack for args */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE   /* Now load as many as possible of the first arguments into
// OBSOLETE      registers, and push the rest onto the stack.  There are 16 bytes
// OBSOLETE      in four registers available.  Loop thru args from first to last.  */
// OBSOLETE 
// OBSOLETE   argreg = ARG0_REGNUM;
// OBSOLETE   for (argnum = 0, stack_offset = 0; argnum < nargs; argnum++)
// OBSOLETE     {
// OBSOLETE       type = VALUE_TYPE (args[argnum]);
// OBSOLETE       len = TYPE_LENGTH (type);
// OBSOLETE       memset (valbuf, 0, sizeof (valbuf));
// OBSOLETE       if (len < 4)
// OBSOLETE 	{			/* value gets right-justified in the register or stack word */
// OBSOLETE 	  memcpy (valbuf + (4 - len),
// OBSOLETE 		  (char *) VALUE_CONTENTS (args[argnum]), len);
// OBSOLETE 	  val = valbuf;
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	val = (char *) VALUE_CONTENTS (args[argnum]);
// OBSOLETE 
// OBSOLETE       if (len > 4 && (len & 3) != 0)
// OBSOLETE 	odd_sized_struct = 1;	/* such structs go entirely on stack */
// OBSOLETE       else
// OBSOLETE 	odd_sized_struct = 0;
// OBSOLETE       while (len > 0)
// OBSOLETE 	{
// OBSOLETE 	  if (argreg > ARGLAST_REGNUM || odd_sized_struct)
// OBSOLETE 	    {			/* must go on the stack */
// OBSOLETE 	      write_memory (sp + stack_offset, val, 4);
// OBSOLETE 	      stack_offset += 4;
// OBSOLETE 	    }
// OBSOLETE 	  /* NOTE WELL!!!!!  This is not an "else if" clause!!!
// OBSOLETE 	     That's because some *&^%$ things get passed on the stack
// OBSOLETE 	     AND in the registers!   */
// OBSOLETE 	  if (argreg <= ARGLAST_REGNUM)
// OBSOLETE 	    {			/* there's room in a register */
// OBSOLETE 	      regval = extract_address (val, REGISTER_RAW_SIZE (argreg));
// OBSOLETE 	      write_register (argreg++, regval);
// OBSOLETE 	    }
// OBSOLETE 	  /* Store the value 4 bytes at a time.  This means that things
// OBSOLETE 	     larger than 4 bytes may go partly in registers and partly
// OBSOLETE 	     on the stack.  */
// OBSOLETE 	  len -= REGISTER_RAW_SIZE (argreg);
// OBSOLETE 	  val += REGISTER_RAW_SIZE (argreg);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   return sp;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Function: fix_call_dummy 
// OBSOLETE    If there is real CALL_DUMMY code (eg. on the stack), this function
// OBSOLETE    has the responsability to insert the address of the actual code that
// OBSOLETE    is the target of the target function call.  */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE m32r_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
// OBSOLETE 		     struct value **args, struct type *type, int gcc_p)
// OBSOLETE {
// OBSOLETE   /* ld24 r8, <(imm24) fun> */
// OBSOLETE   *(unsigned long *) (dummy) = (fun & 0x00ffffff) | 0xe8000000;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Function: m32r_write_sp
// OBSOLETE    Because SP is really a read-only register that mirrors either SPU or SPI,
// OBSOLETE    we must actually write one of those two as well, depending on PSW. */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE m32r_write_sp (CORE_ADDR val)
// OBSOLETE {
// OBSOLETE   unsigned long psw = read_register (PSW_REGNUM);
// OBSOLETE 
// OBSOLETE   if (psw & 0x80)		/* stack mode: user or interrupt */
// OBSOLETE     write_register (SPU_REGNUM, val);
// OBSOLETE   else
// OBSOLETE     write_register (SPI_REGNUM, val);
// OBSOLETE   write_register (SP_REGNUM, val);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_m32r_tdep (void)
// OBSOLETE {
// OBSOLETE   tm_print_insn = print_insn_m32r;
// OBSOLETE }
