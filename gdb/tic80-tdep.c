/* OBSOLETE /* Target-dependent code for the TI TMS320C80 (MVP) for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1996, 1997, 1999, 2000, 2001 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "value.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "inferior.h" */
/* OBSOLETE #include "obstack.h" */
/* OBSOLETE #include "target.h" */
/* OBSOLETE #include "bfd.h" */
/* OBSOLETE #include "gdb_string.h" */
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE #include "symfile.h" */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* Function: frame_find_saved_regs */
/* OBSOLETE    Return the frame_saved_regs structure for the frame. */
/* OBSOLETE    Doesn't really work for dummy frames, but it does pass back */
/* OBSOLETE    an empty frame_saved_regs, so I guess that's better than total failure */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE tic80_frame_find_saved_regs (struct frame_info *fi, */
/* OBSOLETE 			     struct frame_saved_regs *regaddr) */
/* OBSOLETE { */
/* OBSOLETE   memcpy (regaddr, &fi->fsr, sizeof (struct frame_saved_regs)); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: skip_prologue */
/* OBSOLETE    Find end of function prologue.  */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE tic80_skip_prologue (CORE_ADDR pc) */
/* OBSOLETE { */
/* OBSOLETE   CORE_ADDR func_addr, func_end; */
/* OBSOLETE   struct symtab_and_line sal; */
/* OBSOLETE  */
/* OBSOLETE   /* See what the symbol table says */ */
/* OBSOLETE  */
/* OBSOLETE   if (find_pc_partial_function (pc, NULL, &func_addr, &func_end)) */
/* OBSOLETE     { */
/* OBSOLETE       sal = find_pc_line (func_addr, 0); */
/* OBSOLETE  */
/* OBSOLETE       if (sal.line != 0 && sal.end < func_end) */
/* OBSOLETE 	return sal.end; */
/* OBSOLETE       else */
/* OBSOLETE 	/* Either there's no line info, or the line after the prologue is after */
/* OBSOLETE 	   the end of the function.  In this case, there probably isn't a */
/* OBSOLETE 	   prologue.  */ */
/* OBSOLETE 	return pc; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* We can't find the start of this function, so there's nothing we can do. */ */
/* OBSOLETE   return pc; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: tic80_scan_prologue */
/* OBSOLETE    This function decodes the target function prologue to determine: */
/* OBSOLETE    1) the size of the stack frame */
/* OBSOLETE    2) which registers are saved on it */
/* OBSOLETE    3) the offsets of saved regs */
/* OBSOLETE    4) the frame size */
/* OBSOLETE    This information is stored in the "extra" fields of the frame_info.  */ */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE tic80_scan_prologue (struct frame_info *fi) */
/* OBSOLETE { */
/* OBSOLETE   struct symtab_and_line sal; */
/* OBSOLETE   CORE_ADDR prologue_start, prologue_end, current_pc; */
/* OBSOLETE  */
/* OBSOLETE   /* Assume there is no frame until proven otherwise.  */ */
/* OBSOLETE   fi->framereg = SP_REGNUM; */
/* OBSOLETE   fi->framesize = 0; */
/* OBSOLETE   fi->frameoffset = 0; */
/* OBSOLETE  */
/* OBSOLETE   /* this code essentially duplicates skip_prologue,  */
/* OBSOLETE      but we need the start address below.  */ */
/* OBSOLETE  */
/* OBSOLETE   if (find_pc_partial_function (fi->pc, NULL, &prologue_start, &prologue_end)) */
/* OBSOLETE     { */
/* OBSOLETE       sal = find_pc_line (prologue_start, 0); */
/* OBSOLETE  */
/* OBSOLETE       if (sal.line == 0)	/* no line info, use current PC */ */
/* OBSOLETE 	if (prologue_start != entry_point_address ()) */
/* OBSOLETE 	  prologue_end = fi->pc; */
/* OBSOLETE 	else */
/* OBSOLETE 	  return;		/* _start has no frame or prologue */ */
/* OBSOLETE       else if (sal.end < prologue_end)	/* next line begins after fn end */ */
/* OBSOLETE 	prologue_end = sal.end;	/* (probably means no prologue)  */ */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE /* FIXME */ */
/* OBSOLETE     prologue_end = prologue_start + 40;		/* We're in the boondocks: allow for */ */
/* OBSOLETE   /* 16 pushes, an add, and "mv fp,sp" */ */
/* OBSOLETE  */
/* OBSOLETE   prologue_end = min (prologue_end, fi->pc); */
/* OBSOLETE  */
/* OBSOLETE   /* Now search the prologue looking for instructions that set up the */
/* OBSOLETE      frame pointer, adjust the stack pointer, and save registers.  */ */
/* OBSOLETE  */
/* OBSOLETE   for (current_pc = prologue_start; current_pc < prologue_end; current_pc += 4) */
/* OBSOLETE     { */
/* OBSOLETE       unsigned int insn; */
/* OBSOLETE       int regno; */
/* OBSOLETE       int offset = 0; */
/* OBSOLETE  */
/* OBSOLETE       insn = read_memory_unsigned_integer (current_pc, 4); */
/* OBSOLETE  */
/* OBSOLETE       if ((insn & 0x301000) == 0x301000)	/* Long immediate? */ */
/* OBSOLETE /* FIXME - set offset for long immediate instructions */ */
/* OBSOLETE 	current_pc += 4; */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  offset = insn & 0x7fff;	/* extract 15-bit offset */ */
/* OBSOLETE 	  if (offset & 0x4000)	/* if negative, sign-extend */ */
/* OBSOLETE 	    offset = -(0x8000 - offset); */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE       if ((insn & 0x7fd0000) == 0x590000)	/* st.{w,d} reg, xx(r1) */ */
/* OBSOLETE 	{ */
/* OBSOLETE 	  regno = ((insn >> 27) & 0x1f); */
/* OBSOLETE 	  fi->fsr.regs[regno] = offset; */
/* OBSOLETE 	  if (insn & 0x8000)	/* 64-bit store (st.d)? */ */
/* OBSOLETE 	    fi->fsr.regs[regno + 1] = offset + 4; */
/* OBSOLETE 	} */
/* OBSOLETE       else if ((insn & 0xffff8000) == 0x086c8000)	/* addu xx, r1, r1 */ */
/* OBSOLETE 	fi->framesize = -offset; */
/* OBSOLETE       else if ((insn & 0xffff8000) == 0xf06c8000)	/* addu xx, r1, r30 */ */
/* OBSOLETE 	{ */
/* OBSOLETE 	  fi->framereg = FP_REGNUM;	/* fp is now valid */ */
/* OBSOLETE 	  fi->frameoffset = offset; */
/* OBSOLETE 	  break;		/* end of stack adjustments */ */
/* OBSOLETE 	} */
/* OBSOLETE       else if (insn == 0xf03b2001)	/* addu r1, r0, r30 */ */
/* OBSOLETE 	{ */
/* OBSOLETE 	  fi->framereg = FP_REGNUM;	/* fp is now valid */ */
/* OBSOLETE 	  fi->frameoffset = 0; */
/* OBSOLETE 	  break;		/* end of stack adjustments */ */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE /* FIXME - handle long immediate instructions */ */
/* OBSOLETE 	break;			/* anything else isn't prologue */ */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: init_extra_frame_info */
/* OBSOLETE    This function actually figures out the frame address for a given pc and */
/* OBSOLETE    sp.  This is tricky on the c80 because we sometimes don't use an explicit */
/* OBSOLETE    frame pointer, and the previous stack pointer isn't necessarily recorded */
/* OBSOLETE    on the stack.  The only reliable way to get this info is to */
/* OBSOLETE    examine the prologue.  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE tic80_init_extra_frame_info (struct frame_info *fi) */
/* OBSOLETE { */
/* OBSOLETE   int reg; */
/* OBSOLETE  */
/* OBSOLETE   if (fi->next) */
/* OBSOLETE     fi->pc = FRAME_SAVED_PC (fi->next); */
/* OBSOLETE  */
/* OBSOLETE   /* Because zero is a valid register offset relative to SP, we initialize */
/* OBSOLETE      the offsets to -1 to indicate unused entries.  */ */
/* OBSOLETE   for (reg = 0; reg < NUM_REGS; reg++) */
/* OBSOLETE     fi->fsr.regs[reg] = -1; */
/* OBSOLETE  */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame)) */
/* OBSOLETE     { */
/* OBSOLETE       /* We need to setup fi->frame here because run_stack_dummy gets it wrong */
/* OBSOLETE          by assuming it's always FP.  */ */
/* OBSOLETE       fi->frame = generic_read_register_dummy (fi->pc, fi->frame, SP_REGNUM); */
/* OBSOLETE       fi->framesize = 0; */
/* OBSOLETE       fi->frameoffset = 0; */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       tic80_scan_prologue (fi); */
/* OBSOLETE  */
/* OBSOLETE       if (!fi->next)		/* this is the innermost frame? */ */
/* OBSOLETE 	fi->frame = read_register (fi->framereg); */
/* OBSOLETE       else */
/* OBSOLETE 	/* not the innermost frame */ */
/* OBSOLETE 	/* If this function uses FP as the frame register, and the function */
/* OBSOLETE 	   it called saved the FP, get the saved FP.  */ if (fi->framereg == FP_REGNUM && */
/* OBSOLETE 			     fi->next->fsr.regs[FP_REGNUM] != (unsigned) -1) */
/* OBSOLETE 	fi->frame = read_memory_integer (fi->next->fsr.regs[FP_REGNUM], 4); */
/* OBSOLETE  */
/* OBSOLETE       /* Convert SP-relative offsets of saved registers to real addresses.  */ */
/* OBSOLETE       for (reg = 0; reg < NUM_REGS; reg++) */
/* OBSOLETE 	if (fi->fsr.regs[reg] == (unsigned) -1) */
/* OBSOLETE 	  fi->fsr.regs[reg] = 0;	/* unused entry */ */
/* OBSOLETE 	else */
/* OBSOLETE 	  fi->fsr.regs[reg] += fi->frame - fi->frameoffset; */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: find_callers_reg */
/* OBSOLETE    Find REGNUM on the stack.  Otherwise, it's in an active register.  One thing */
/* OBSOLETE    we might want to do here is to check REGNUM against the clobber mask, and */
/* OBSOLETE    somehow flag it as invalid if it isn't saved on the stack somewhere.  This */
/* OBSOLETE    would provide a graceful failure mode when trying to get the value of */
/* OBSOLETE    caller-saves registers for an inner frame.  */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE tic80_find_callers_reg (struct frame_info *fi, int regnum) */
/* OBSOLETE { */
/* OBSOLETE   for (; fi; fi = fi->next) */
/* OBSOLETE     if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame)) */
/* OBSOLETE       return generic_read_register_dummy (fi->pc, fi->frame, regnum); */
/* OBSOLETE     else if (fi->fsr.regs[regnum] != 0) */
/* OBSOLETE       return read_memory_integer (fi->fsr.regs[regnum], */
/* OBSOLETE 				  REGISTER_RAW_SIZE (regnum)); */
/* OBSOLETE   return read_register (regnum); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: frame_chain */
/* OBSOLETE    Given a GDB frame, determine the address of the calling function's frame. */
/* OBSOLETE    This will be used to create a new GDB frame struct, and then */
/* OBSOLETE    INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame. */
/* OBSOLETE    For c80, we save the frame size when we initialize the frame_info.  */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE tic80_frame_chain (struct frame_info *fi) */
/* OBSOLETE { */
/* OBSOLETE   CORE_ADDR fn_start, callers_pc, fp; */
/* OBSOLETE  */
/* OBSOLETE   /* is this a dummy frame? */ */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame)) */
/* OBSOLETE     return fi->frame;		/* dummy frame same as caller's frame */ */
/* OBSOLETE  */
/* OBSOLETE   /* is caller-of-this a dummy frame? */ */
/* OBSOLETE   callers_pc = FRAME_SAVED_PC (fi);	/* find out who called us: */ */
/* OBSOLETE   fp = tic80_find_callers_reg (fi, FP_REGNUM); */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (callers_pc, fp, fp)) */
/* OBSOLETE     return fp;			/* dummy frame's frame may bear no relation to ours */ */
/* OBSOLETE  */
/* OBSOLETE   if (find_pc_partial_function (fi->pc, 0, &fn_start, 0)) */
/* OBSOLETE     if (fn_start == entry_point_address ()) */
/* OBSOLETE       return 0;			/* in _start fn, don't chain further */ */
/* OBSOLETE  */
/* OBSOLETE   if (fi->framereg == FP_REGNUM) */
/* OBSOLETE     return tic80_find_callers_reg (fi, FP_REGNUM); */
/* OBSOLETE   else */
/* OBSOLETE     return fi->frame + fi->framesize; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: pop_frame */
/* OBSOLETE    Discard from the stack the innermost frame, */
/* OBSOLETE    restoring all saved registers.  */ */
/* OBSOLETE  */
/* OBSOLETE struct frame_info * */
/* OBSOLETE tic80_pop_frame (struct frame_info *frame) */
/* OBSOLETE { */
/* OBSOLETE   int regnum; */
/* OBSOLETE  */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame)) */
/* OBSOLETE     generic_pop_dummy_frame (); */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       for (regnum = 0; regnum < NUM_REGS; regnum++) */
/* OBSOLETE 	if (frame->fsr.regs[regnum] != 0) */
/* OBSOLETE 	  write_register (regnum, */
/* OBSOLETE 			  read_memory_integer (frame->fsr.regs[regnum], 4)); */
/* OBSOLETE  */
/* OBSOLETE       write_register (PC_REGNUM, FRAME_SAVED_PC (frame)); */
/* OBSOLETE       write_register (SP_REGNUM, read_register (FP_REGNUM)); */
/* OBSOLETE #if 0 */
/* OBSOLETE       if (read_register (PSW_REGNUM) & 0x80) */
/* OBSOLETE 	write_register (SPU_REGNUM, read_register (SP_REGNUM)); */
/* OBSOLETE       else */
/* OBSOLETE 	write_register (SPI_REGNUM, read_register (SP_REGNUM)); */
/* OBSOLETE #endif */
/* OBSOLETE     } */
/* OBSOLETE   flush_cached_frames (); */
/* OBSOLETE   return NULL; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: frame_saved_pc */
/* OBSOLETE    Find the caller of this frame.  We do this by seeing if LR_REGNUM is saved */
/* OBSOLETE    in the stack anywhere, otherwise we get it from the registers. */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE tic80_frame_saved_pc (struct frame_info *fi) */
/* OBSOLETE { */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame)) */
/* OBSOLETE     return generic_read_register_dummy (fi->pc, fi->frame, PC_REGNUM); */
/* OBSOLETE   else */
/* OBSOLETE     return tic80_find_callers_reg (fi, LR_REGNUM); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: tic80_push_return_address (pc, sp) */
/* OBSOLETE    Set up the return address for the inferior function call. */
/* OBSOLETE    Necessary for targets that don't actually execute a JSR/BSR instruction  */
/* OBSOLETE    (ie. when using an empty CALL_DUMMY) */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE tic80_push_return_address (CORE_ADDR pc, CORE_ADDR sp) */
/* OBSOLETE { */
/* OBSOLETE   write_register (LR_REGNUM, CALL_DUMMY_ADDRESS ()); */
/* OBSOLETE   return sp; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Function: push_arguments */
/* OBSOLETE    Setup the function arguments for calling a function in the inferior. */
/* OBSOLETE  */
/* OBSOLETE    On the TI C80 architecture, there are six register pairs (R2/R3 to R12/13) */
/* OBSOLETE    which are dedicated for passing function arguments.  Up to the first six */
/* OBSOLETE    arguments (depending on size) may go into these registers. */
/* OBSOLETE    The rest go on the stack. */
/* OBSOLETE  */
/* OBSOLETE    Arguments that are smaller than 4 bytes will still take up a whole */
/* OBSOLETE    register or a whole 32-bit word on the stack, and will be */
/* OBSOLETE    right-justified in the register or the stack word.  This includes */
/* OBSOLETE    chars, shorts, and small aggregate types. */
/* OBSOLETE  */
/* OBSOLETE    Arguments that are four bytes or less in size are placed in the */
/* OBSOLETE    even-numbered register of a register pair, and the odd-numbered */
/* OBSOLETE    register is not used. */
/* OBSOLETE  */
/* OBSOLETE    Arguments of 8 bytes size (such as floating point doubles) are placed */
/* OBSOLETE    in a register pair.  The least significant 32-bit word is placed in */
/* OBSOLETE    the even-numbered register, and the most significant word in the */
/* OBSOLETE    odd-numbered register. */
/* OBSOLETE  */
/* OBSOLETE    Aggregate types with sizes between 4 and 8 bytes are passed */
/* OBSOLETE    entirely on the stack, and are left-justified within the */
/* OBSOLETE    double-word (as opposed to aggregates smaller than 4 bytes */
/* OBSOLETE    which are right-justified). */
/* OBSOLETE  */
/* OBSOLETE    Aggregates of greater than 8 bytes are first copied onto the stack,  */
/* OBSOLETE    and then a pointer to the copy is passed in the place of the normal */
/* OBSOLETE    argument (either in a register if available, or on the stack). */
/* OBSOLETE  */
/* OBSOLETE    Functions that must return an aggregate type can return it in the  */
/* OBSOLETE    normal return value registers (R2 and R3) if its size is 8 bytes or */
/* OBSOLETE    less.  For larger return values, the caller must allocate space for  */
/* OBSOLETE    the callee to copy the return value to.  A pointer to this space is */
/* OBSOLETE    passed as an implicit first argument, always in R0. */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE tic80_push_arguments (int nargs, value_ptr *args, CORE_ADDR sp, */
/* OBSOLETE 		      unsigned char struct_return, CORE_ADDR struct_addr) */
/* OBSOLETE { */
/* OBSOLETE   int stack_offset, stack_alloc; */
/* OBSOLETE   int argreg; */
/* OBSOLETE   int argnum; */
/* OBSOLETE   struct type *type; */
/* OBSOLETE   CORE_ADDR regval; */
/* OBSOLETE   char *val; */
/* OBSOLETE   char valbuf[4]; */
/* OBSOLETE   int len; */
/* OBSOLETE   int odd_sized_struct; */
/* OBSOLETE   int is_struct; */
/* OBSOLETE  */
/* OBSOLETE   /* first force sp to a 4-byte alignment */ */
/* OBSOLETE   sp = sp & ~3; */
/* OBSOLETE  */
/* OBSOLETE   argreg = ARG0_REGNUM; */
/* OBSOLETE   /* The "struct return pointer" pseudo-argument goes in R0 */ */
/* OBSOLETE   if (struct_return) */
/* OBSOLETE     write_register (argreg++, struct_addr); */
/* OBSOLETE  */
/* OBSOLETE   /* Now make sure there's space on the stack */ */
/* OBSOLETE   for (argnum = 0, stack_alloc = 0; */
/* OBSOLETE        argnum < nargs; argnum++) */
/* OBSOLETE     stack_alloc += ((TYPE_LENGTH (VALUE_TYPE (args[argnum])) + 3) & ~3); */
/* OBSOLETE   sp -= stack_alloc;		/* make room on stack for args */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE   /* Now load as many as possible of the first arguments into */
/* OBSOLETE      registers, and push the rest onto the stack.  There are 16 bytes */
/* OBSOLETE      in four registers available.  Loop thru args from first to last.  */ */
/* OBSOLETE  */
/* OBSOLETE   argreg = ARG0_REGNUM; */
/* OBSOLETE   for (argnum = 0, stack_offset = 0; argnum < nargs; argnum++) */
/* OBSOLETE     { */
/* OBSOLETE       type = VALUE_TYPE (args[argnum]); */
/* OBSOLETE       len = TYPE_LENGTH (type); */
/* OBSOLETE       memset (valbuf, 0, sizeof (valbuf)); */
/* OBSOLETE       val = (char *) VALUE_CONTENTS (args[argnum]); */
/* OBSOLETE  */
/* OBSOLETE /* FIXME -- tic80 can take doubleword arguments in register pairs */ */
/* OBSOLETE       is_struct = (type->code == TYPE_CODE_STRUCT); */
/* OBSOLETE       odd_sized_struct = 0; */
/* OBSOLETE  */
/* OBSOLETE       if (!is_struct) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  if (len < 4) */
/* OBSOLETE 	    {			/* value gets right-justified in the register or stack word */ */
/* OBSOLETE 	      memcpy (valbuf + (4 - len), val, len); */
/* OBSOLETE 	      val = valbuf; */
/* OBSOLETE 	    } */
/* OBSOLETE 	  if (len > 4 && (len & 3) != 0) */
/* OBSOLETE 	    odd_sized_struct = 1;	/* such structs go entirely on stack */ */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Structs are always passed by reference. */ */
/* OBSOLETE 	  write_register (argreg, sp + stack_offset); */
/* OBSOLETE 	  argreg++; */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE       while (len > 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  if (is_struct || argreg > ARGLAST_REGNUM || odd_sized_struct) */
/* OBSOLETE 	    {			/* must go on the stack */ */
/* OBSOLETE 	      write_memory (sp + stack_offset, val, 4); */
/* OBSOLETE 	      stack_offset += 4; */
/* OBSOLETE 	    } */
/* OBSOLETE 	  /* NOTE WELL!!!!!  This is not an "else if" clause!!! */
/* OBSOLETE 	     That's because some things get passed on the stack */
/* OBSOLETE 	     AND in the registers!   */ */
/* OBSOLETE 	  if (!is_struct && argreg <= ARGLAST_REGNUM) */
/* OBSOLETE 	    {			/* there's room in a register */ */
/* OBSOLETE 	      regval = extract_address (val, REGISTER_RAW_SIZE (argreg)); */
/* OBSOLETE 	      write_register (argreg, regval); */
/* OBSOLETE 	      argreg += 2;	/* FIXME -- what about doubleword args? */ */
/* OBSOLETE 	    } */
/* OBSOLETE 	  /* Store the value 4 bytes at a time.  This means that things */
/* OBSOLETE 	     larger than 4 bytes may go partly in registers and partly */
/* OBSOLETE 	     on the stack.  */ */
/* OBSOLETE 	  len -= REGISTER_RAW_SIZE (argreg); */
/* OBSOLETE 	  val += REGISTER_RAW_SIZE (argreg); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   return sp; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Function: tic80_write_sp */
/* OBSOLETE    Because SP is really a read-only register that mirrors either SPU or SPI, */
/* OBSOLETE    we must actually write one of those two as well, depending on PSW. */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE tic80_write_sp (CORE_ADDR val) */
/* OBSOLETE { */
/* OBSOLETE #if 0 */
/* OBSOLETE   unsigned long psw = read_register (PSW_REGNUM); */
/* OBSOLETE  */
/* OBSOLETE   if (psw & 0x80)		/* stack mode: user or interrupt */ */
/* OBSOLETE     write_register (SPU_REGNUM, val); */
/* OBSOLETE   else */
/* OBSOLETE     write_register (SPI_REGNUM, val); */
/* OBSOLETE #endif */
/* OBSOLETE   write_register (SP_REGNUM, val); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE _initialize_tic80_tdep (void) */
/* OBSOLETE { */
/* OBSOLETE   tm_print_insn = print_insn_tic80; */
/* OBSOLETE } */
