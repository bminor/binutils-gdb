/* OBSOLETE /* Target-machine dependent code for the AMD 29000 */
/* OBSOLETE    Copyright 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, */
/* OBSOLETE    2001 */
/* OBSOLETE    Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by Cygnus Support.  Written by Jim Kingdon. */
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
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "value.h" */
/* OBSOLETE #include "symtab.h" */
/* OBSOLETE #include "inferior.h" */
/* OBSOLETE #include "gdbcmd.h" */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* If all these bits in an instruction word are zero, it is a "tag word" */
/* OBSOLETE    which precedes a function entry point and gives stack traceback info. */
/* OBSOLETE    This used to be defined as 0xff000000, but that treated 0x00000deb as */
/* OBSOLETE    a tag word, while it is really used as a breakpoint.  */ */
/* OBSOLETE #define	TAGWORD_ZERO_MASK	0xff00f800 */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR text_start;	/* FIXME, kludge... */ */
/* OBSOLETE  */
/* OBSOLETE /* The user-settable top of the register stack in virtual memory.  We */
/* OBSOLETE    won't attempt to access any stored registers above this address, if set */
/* OBSOLETE    nonzero.  */ */
/* OBSOLETE  */
/* OBSOLETE static CORE_ADDR rstack_high_address = UINT_MAX; */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Should call_function allocate stack space for a struct return?  */ */
/* OBSOLETE /* On the a29k objects over 16 words require the caller to allocate space.  */ */
/* OBSOLETE int */
/* OBSOLETE a29k_use_struct_convention (int gcc_p, struct type *type) */
/* OBSOLETE { */
/* OBSOLETE   return (TYPE_LENGTH (type) > 16 * 4); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Structure to hold cached info about function prologues.  */ */
/* OBSOLETE  */
/* OBSOLETE struct prologue_info */
/* OBSOLETE { */
/* OBSOLETE   CORE_ADDR pc;			/* First addr after fn prologue */ */
/* OBSOLETE   unsigned rsize, msize;	/* register stack frame size, mem stack ditto */ */
/* OBSOLETE   unsigned mfp_used:1;		/* memory frame pointer used */ */
/* OBSOLETE   unsigned rsize_valid:1;	/* Validity bits for the above */ */
/* OBSOLETE   unsigned msize_valid:1; */
/* OBSOLETE   unsigned mfp_valid:1; */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE /* Examine the prologue of a function which starts at PC.  Return */
/* OBSOLETE    the first addess past the prologue.  If MSIZE is non-NULL, then */
/* OBSOLETE    set *MSIZE to the memory stack frame size.  If RSIZE is non-NULL, */
/* OBSOLETE    then set *RSIZE to the register stack frame size (not including */
/* OBSOLETE    incoming arguments and the return address & frame pointer stored */
/* OBSOLETE    with them).  If no prologue is found, *RSIZE is set to zero. */
/* OBSOLETE    If no prologue is found, or a prologue which doesn't involve */
/* OBSOLETE    allocating a memory stack frame, then set *MSIZE to zero. */
/* OBSOLETE  */
/* OBSOLETE    Note that both msize and rsize are in bytes.  This is not consistent */
/* OBSOLETE    with the _User's Manual_ with respect to rsize, but it is much more */
/* OBSOLETE    convenient. */
/* OBSOLETE  */
/* OBSOLETE    If MFP_USED is non-NULL, *MFP_USED is set to nonzero if a memory */
/* OBSOLETE    frame pointer is being used.  */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE examine_prologue (CORE_ADDR pc, unsigned *rsize, unsigned *msize, int *mfp_used) */
/* OBSOLETE { */
/* OBSOLETE   long insn; */
/* OBSOLETE   CORE_ADDR p = pc; */
/* OBSOLETE   struct minimal_symbol *msymbol = lookup_minimal_symbol_by_pc (pc); */
/* OBSOLETE   struct prologue_info *mi = 0; */
/* OBSOLETE  */
/* OBSOLETE   if (msymbol != NULL) */
/* OBSOLETE     mi = (struct prologue_info *) msymbol->info; */
/* OBSOLETE  */
/* OBSOLETE   if (mi != 0) */
/* OBSOLETE     { */
/* OBSOLETE       int valid = 1; */
/* OBSOLETE       if (rsize != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  *rsize = mi->rsize; */
/* OBSOLETE 	  valid &= mi->rsize_valid; */
/* OBSOLETE 	} */
/* OBSOLETE       if (msize != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  *msize = mi->msize; */
/* OBSOLETE 	  valid &= mi->msize_valid; */
/* OBSOLETE 	} */
/* OBSOLETE       if (mfp_used != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  *mfp_used = mi->mfp_used; */
/* OBSOLETE 	  valid &= mi->mfp_valid; */
/* OBSOLETE 	} */
/* OBSOLETE       if (valid) */
/* OBSOLETE 	return mi->pc; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   if (rsize != NULL) */
/* OBSOLETE     *rsize = 0; */
/* OBSOLETE   if (msize != NULL) */
/* OBSOLETE     *msize = 0; */
/* OBSOLETE   if (mfp_used != NULL) */
/* OBSOLETE     *mfp_used = 0; */
/* OBSOLETE  */
/* OBSOLETE   /* Prologue must start with subtracting a constant from gr1. */
/* OBSOLETE      Normally this is sub gr1,gr1,<rsize * 4>.  */ */
/* OBSOLETE   insn = read_memory_integer (p, 4); */
/* OBSOLETE   if ((insn & 0xffffff00) != 0x25010100) */
/* OBSOLETE     { */
/* OBSOLETE       /* If the frame is large, instead of a single instruction it */
/* OBSOLETE          might be a pair of instructions: */
/* OBSOLETE          const <reg>, <rsize * 4> */
/* OBSOLETE          sub gr1,gr1,<reg> */
/* OBSOLETE        */ */
/* OBSOLETE       int reg; */
/* OBSOLETE       /* Possible value for rsize.  */ */
/* OBSOLETE       unsigned int rsize0; */
/* OBSOLETE  */
/* OBSOLETE       if ((insn & 0xff000000) != 0x03000000) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  p = pc; */
/* OBSOLETE 	  goto done; */
/* OBSOLETE 	} */
/* OBSOLETE       reg = (insn >> 8) & 0xff; */
/* OBSOLETE       rsize0 = (((insn >> 8) & 0xff00) | (insn & 0xff)); */
/* OBSOLETE       p += 4; */
/* OBSOLETE       insn = read_memory_integer (p, 4); */
/* OBSOLETE       if ((insn & 0xffffff00) != 0x24010100 */
/* OBSOLETE 	  || (insn & 0xff) != reg) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  p = pc; */
/* OBSOLETE 	  goto done; */
/* OBSOLETE 	} */
/* OBSOLETE       if (rsize != NULL) */
/* OBSOLETE 	*rsize = rsize0; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       if (rsize != NULL) */
/* OBSOLETE 	*rsize = (insn & 0xff); */
/* OBSOLETE     } */
/* OBSOLETE   p += 4; */
/* OBSOLETE  */
/* OBSOLETE   /* Next instruction ought to be asgeu V_SPILL,gr1,rab.   */
/* OBSOLETE    * We don't check the vector number to allow for kernel debugging.  The  */
/* OBSOLETE    * kernel will use a different trap number.  */
/* OBSOLETE    * If this insn is missing, we just keep going; Metaware R2.3u compiler */
/* OBSOLETE    * generates prologue that intermixes initializations and puts the asgeu */
/* OBSOLETE    * way down. */
/* OBSOLETE    */ */
/* OBSOLETE   insn = read_memory_integer (p, 4); */
/* OBSOLETE   if ((insn & 0xff00ffff) == (0x5e000100 | RAB_HW_REGNUM)) */
/* OBSOLETE     { */
/* OBSOLETE       p += 4; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Next instruction usually sets the frame pointer (lr1) by adding */
/* OBSOLETE      <size * 4> from gr1.  However, this can (and high C does) be */
/* OBSOLETE      deferred until anytime before the first function call.  So it is */
/* OBSOLETE      OK if we don't see anything which sets lr1.   */
/* OBSOLETE      To allow for alternate register sets (gcc -mkernel-registers)  the msp */
/* OBSOLETE      register number is a compile time constant. */ */
/* OBSOLETE  */
/* OBSOLETE   /* Normally this is just add lr1,gr1,<size * 4>.  */ */
/* OBSOLETE   insn = read_memory_integer (p, 4); */
/* OBSOLETE   if ((insn & 0xffffff00) == 0x15810100) */
/* OBSOLETE     p += 4; */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* However, for large frames it can be */
/* OBSOLETE          const <reg>, <size *4> */
/* OBSOLETE          add lr1,gr1,<reg> */
/* OBSOLETE        */ */
/* OBSOLETE       int reg; */
/* OBSOLETE       CORE_ADDR q; */
/* OBSOLETE  */
/* OBSOLETE       if ((insn & 0xff000000) == 0x03000000) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  reg = (insn >> 8) & 0xff; */
/* OBSOLETE 	  q = p + 4; */
/* OBSOLETE 	  insn = read_memory_integer (q, 4); */
/* OBSOLETE 	  if ((insn & 0xffffff00) == 0x14810100 */
/* OBSOLETE 	      && (insn & 0xff) == reg) */
/* OBSOLETE 	    p = q; */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Next comes "add lr{<rsize-1>},msp,0", but only if a memory */
/* OBSOLETE      frame pointer is in use.  We just check for add lr<anything>,msp,0; */
/* OBSOLETE      we don't check this rsize against the first instruction, and */
/* OBSOLETE      we don't check that the trace-back tag indicates a memory frame pointer */
/* OBSOLETE      is in use.   */
/* OBSOLETE      To allow for alternate register sets (gcc -mkernel-registers)  the msp */
/* OBSOLETE      register number is a compile time constant. */
/* OBSOLETE  */
/* OBSOLETE      The recommended instruction is actually "sll lr<whatever>,msp,0".  */
/* OBSOLETE      We check for that, too.  Originally Jim Kingdon's code seemed */
/* OBSOLETE      to be looking for a "sub" instruction here, but the mask was set */
/* OBSOLETE      up to lose all the time. */ */
/* OBSOLETE   insn = read_memory_integer (p, 4); */
/* OBSOLETE   if (((insn & 0xff80ffff) == (0x15800000 | (MSP_HW_REGNUM << 8)))	/* add */ */
/* OBSOLETE       || ((insn & 0xff80ffff) == (0x81800000 | (MSP_HW_REGNUM << 8))))	/* sll */ */
/* OBSOLETE     { */
/* OBSOLETE       p += 4; */
/* OBSOLETE       if (mfp_used != NULL) */
/* OBSOLETE 	*mfp_used = 1; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Next comes a subtraction from msp to allocate a memory frame, */
/* OBSOLETE      but only if a memory frame is */
/* OBSOLETE      being used.  We don't check msize against the trace-back tag. */
/* OBSOLETE  */
/* OBSOLETE      To allow for alternate register sets (gcc -mkernel-registers) the msp */
/* OBSOLETE      register number is a compile time constant. */
/* OBSOLETE  */
/* OBSOLETE      Normally this is just */
/* OBSOLETE      sub msp,msp,<msize> */
/* OBSOLETE    */ */
/* OBSOLETE   insn = read_memory_integer (p, 4); */
/* OBSOLETE   if ((insn & 0xffffff00) == */
/* OBSOLETE       (0x25000000 | (MSP_HW_REGNUM << 16) | (MSP_HW_REGNUM << 8))) */
/* OBSOLETE     { */
/* OBSOLETE       p += 4; */
/* OBSOLETE       if (msize != NULL) */
/* OBSOLETE 	*msize = insn & 0xff; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* For large frames, instead of a single instruction it might */
/* OBSOLETE          be */
/* OBSOLETE  */
/* OBSOLETE          const <reg>, <msize> */
/* OBSOLETE          consth <reg>, <msize>     ; optional */
/* OBSOLETE          sub msp,msp,<reg> */
/* OBSOLETE        */ */
/* OBSOLETE       int reg; */
/* OBSOLETE       unsigned msize0; */
/* OBSOLETE       CORE_ADDR q = p; */
/* OBSOLETE  */
/* OBSOLETE       if ((insn & 0xff000000) == 0x03000000) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  reg = (insn >> 8) & 0xff; */
/* OBSOLETE 	  msize0 = ((insn >> 8) & 0xff00) | (insn & 0xff); */
/* OBSOLETE 	  q += 4; */
/* OBSOLETE 	  insn = read_memory_integer (q, 4); */
/* OBSOLETE 	  /* Check for consth.  */ */
/* OBSOLETE 	  if ((insn & 0xff000000) == 0x02000000 */
/* OBSOLETE 	      && (insn & 0x0000ff00) == reg) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      msize0 |= (insn << 8) & 0xff000000; */
/* OBSOLETE 	      msize0 |= (insn << 16) & 0x00ff0000; */
/* OBSOLETE 	      q += 4; */
/* OBSOLETE 	      insn = read_memory_integer (q, 4); */
/* OBSOLETE 	    } */
/* OBSOLETE 	  /* Check for sub msp,msp,<reg>.  */ */
/* OBSOLETE 	  if ((insn & 0xffffff00) == */
/* OBSOLETE 	      (0x24000000 | (MSP_HW_REGNUM << 16) | (MSP_HW_REGNUM << 8)) */
/* OBSOLETE 	      && (insn & 0xff) == reg) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      p = q + 4; */
/* OBSOLETE 	      if (msize != NULL) */
/* OBSOLETE 		*msize = msize0; */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Next instruction might be asgeu V_SPILL,gr1,rab.   */
/* OBSOLETE    * We don't check the vector number to allow for kernel debugging.  The  */
/* OBSOLETE    * kernel will use a different trap number.  */
/* OBSOLETE    * Metaware R2.3u compiler */
/* OBSOLETE    * generates prologue that intermixes initializations and puts the asgeu */
/* OBSOLETE    * way down after everything else. */
/* OBSOLETE    */ */
/* OBSOLETE   insn = read_memory_integer (p, 4); */
/* OBSOLETE   if ((insn & 0xff00ffff) == (0x5e000100 | RAB_HW_REGNUM)) */
/* OBSOLETE     { */
/* OBSOLETE       p += 4; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE done: */
/* OBSOLETE   if (msymbol != NULL) */
/* OBSOLETE     { */
/* OBSOLETE       if (mi == 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Add a new cache entry.  */ */
/* OBSOLETE 	  mi = (struct prologue_info *) xmalloc (sizeof (struct prologue_info)); */
/* OBSOLETE 	  msymbol->info = (char *) mi; */
/* OBSOLETE 	  mi->rsize_valid = 0; */
/* OBSOLETE 	  mi->msize_valid = 0; */
/* OBSOLETE 	  mi->mfp_valid = 0; */
/* OBSOLETE 	} */
/* OBSOLETE       /* else, cache entry exists, but info is incomplete.  */ */
/* OBSOLETE       mi->pc = p; */
/* OBSOLETE       if (rsize != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  mi->rsize = *rsize; */
/* OBSOLETE 	  mi->rsize_valid = 1; */
/* OBSOLETE 	} */
/* OBSOLETE       if (msize != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  mi->msize = *msize; */
/* OBSOLETE 	  mi->msize_valid = 1; */
/* OBSOLETE 	} */
/* OBSOLETE       if (mfp_used != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  mi->mfp_used = *mfp_used; */
/* OBSOLETE 	  mi->mfp_valid = 1; */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   return p; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE a29k_skip_prologue (CORE_ADDR pc) */
/* OBSOLETE { */
/* OBSOLETE   return examine_prologue (pc, NULL, NULL, NULL); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Examine the one or two word tag at the beginning of a function. */
/* OBSOLETE  * The tag word is expect to be at 'p', if it is not there, we fail */
/* OBSOLETE  * by returning 0.  The documentation for the tag word was taken from */
/* OBSOLETE  * page 7-15 of the 29050 User's Manual.  We are assuming that the */
/* OBSOLETE  * m bit is in bit 22 of the tag word, which seems to be the agreed upon */
/* OBSOLETE  * convention today (1/15/92). */
/* OBSOLETE  * msize is return in bytes. */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE static int			/* 0/1 - failure/success of finding the tag word  */ */
/* OBSOLETE examine_tag (CORE_ADDR p, int *is_trans, int *argcount, unsigned *msize, */
/* OBSOLETE 	     int *mfp_used) */
/* OBSOLETE { */
/* OBSOLETE   unsigned int tag1, tag2; */
/* OBSOLETE  */
/* OBSOLETE   tag1 = read_memory_integer (p, 4); */
/* OBSOLETE   if ((tag1 & TAGWORD_ZERO_MASK) != 0)	/* Not a tag word */ */
/* OBSOLETE     return 0; */
/* OBSOLETE   if (tag1 & (1 << 23))		/* A two word tag */ */
/* OBSOLETE     { */
/* OBSOLETE       tag2 = read_memory_integer (p - 4, 4); */
/* OBSOLETE       if (msize) */
/* OBSOLETE 	*msize = tag2 * 2; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     /* A one word tag */ */
/* OBSOLETE     { */
/* OBSOLETE       if (msize) */
/* OBSOLETE 	*msize = tag1 & 0x7ff; */
/* OBSOLETE     } */
/* OBSOLETE   if (is_trans) */
/* OBSOLETE     *is_trans = ((tag1 & (1 << 21)) ? 1 : 0); */
/* OBSOLETE   /* Note that this includes the frame pointer and the return address */
/* OBSOLETE      register, so the actual number of registers of arguments is two less. */
/* OBSOLETE      argcount can be zero, however, sometimes, for strange assembler */
/* OBSOLETE      routines.  */ */
/* OBSOLETE   if (argcount) */
/* OBSOLETE     *argcount = (tag1 >> 16) & 0x1f; */
/* OBSOLETE   if (mfp_used) */
/* OBSOLETE     *mfp_used = ((tag1 & (1 << 22)) ? 1 : 0); */
/* OBSOLETE   return 1; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Initialize the frame.  In addition to setting "extra" frame info, */
/* OBSOLETE    we also set ->frame because we use it in a nonstandard way, and ->pc */
/* OBSOLETE    because we need to know it to get the other stuff.  See the diagram */
/* OBSOLETE    of stacks and the frame cache in tm-a29k.h for more detail.  */ */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE init_frame_info (int innermost_frame, struct frame_info *frame) */
/* OBSOLETE { */
/* OBSOLETE   CORE_ADDR p; */
/* OBSOLETE   long insn; */
/* OBSOLETE   unsigned rsize; */
/* OBSOLETE   unsigned msize; */
/* OBSOLETE   int mfp_used, trans; */
/* OBSOLETE   struct symbol *func; */
/* OBSOLETE  */
/* OBSOLETE   p = frame->pc; */
/* OBSOLETE  */
/* OBSOLETE   if (innermost_frame) */
/* OBSOLETE     frame->frame = read_register (GR1_REGNUM); */
/* OBSOLETE   else */
/* OBSOLETE     frame->frame = frame->next->frame + frame->next->rsize; */
/* OBSOLETE  */
/* OBSOLETE #if 0				/* CALL_DUMMY_LOCATION == ON_STACK */ */
/* OBSOLETE   This wont work; */
/* OBSOLETE #else */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (p, 0, 0)) */
/* OBSOLETE #endif */
/* OBSOLETE     { */
/* OBSOLETE       frame->rsize = DUMMY_FRAME_RSIZE; */
/* OBSOLETE       /* This doesn't matter since we never try to get locals or args */
/* OBSOLETE          from a dummy frame.  */ */
/* OBSOLETE       frame->msize = 0; */
/* OBSOLETE       /* Dummy frames always use a memory frame pointer.  */ */
/* OBSOLETE       frame->saved_msp = */
/* OBSOLETE 	read_register_stack_integer (frame->frame + DUMMY_FRAME_RSIZE - 4, 4); */
/* OBSOLETE       frame->flags |= (TRANSPARENT_FRAME | MFP_USED); */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   func = find_pc_function (p); */
/* OBSOLETE   if (func != NULL) */
/* OBSOLETE     p = BLOCK_START (SYMBOL_BLOCK_VALUE (func)); */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* Search backward to find the trace-back tag.  However, */
/* OBSOLETE          do not trace back beyond the start of the text segment */
/* OBSOLETE          (just as a sanity check to avoid going into never-never land).  */ */
/* OBSOLETE #if 1 */
/* OBSOLETE       while (p >= text_start */
/* OBSOLETE 	  && ((insn = read_memory_integer (p, 4)) & TAGWORD_ZERO_MASK) != 0) */
/* OBSOLETE 	p -= 4; */
/* OBSOLETE #else /* 0 */ */
/* OBSOLETE       char pat[4] = */
/* OBSOLETE       {0, 0, 0, 0}; */
/* OBSOLETE       char mask[4]; */
/* OBSOLETE       char insn_raw[4]; */
/* OBSOLETE       store_unsigned_integer (mask, 4, TAGWORD_ZERO_MASK); */
/* OBSOLETE       /* Enable this once target_search is enabled and tested.  */ */
/* OBSOLETE       target_search (4, pat, mask, p, -4, text_start, p + 1, &p, &insn_raw); */
/* OBSOLETE       insn = extract_unsigned_integer (insn_raw, 4); */
/* OBSOLETE #endif /* 0 */ */
/* OBSOLETE  */
/* OBSOLETE       if (p < text_start) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Couldn't find the trace-back tag. */
/* OBSOLETE 	     Something strange is going on.  */ */
/* OBSOLETE 	  frame->saved_msp = 0; */
/* OBSOLETE 	  frame->rsize = 0; */
/* OBSOLETE 	  frame->msize = 0; */
/* OBSOLETE 	  frame->flags = TRANSPARENT_FRAME; */
/* OBSOLETE 	  return; */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	/* Advance to the first word of the function, i.e. the word */
/* OBSOLETE 	   after the trace-back tag.  */ */
/* OBSOLETE 	p += 4; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* We've found the start of the function.   */
/* OBSOLETE      Try looking for a tag word that indicates whether there is a */
/* OBSOLETE      memory frame pointer and what the memory stack allocation is. */
/* OBSOLETE      If one doesn't exist, try using a more exhaustive search of */
/* OBSOLETE      the prologue.  */ */
/* OBSOLETE  */
/* OBSOLETE   if (examine_tag (p - 4, &trans, (int *) NULL, &msize, &mfp_used))	/* Found good tag */ */
/* OBSOLETE     examine_prologue (p, &rsize, 0, 0); */
/* OBSOLETE   else				/* No tag try prologue */ */
/* OBSOLETE     examine_prologue (p, &rsize, &msize, &mfp_used); */
/* OBSOLETE  */
/* OBSOLETE   frame->rsize = rsize; */
/* OBSOLETE   frame->msize = msize; */
/* OBSOLETE   frame->flags = 0; */
/* OBSOLETE   if (mfp_used) */
/* OBSOLETE     frame->flags |= MFP_USED; */
/* OBSOLETE   if (trans) */
/* OBSOLETE     frame->flags |= TRANSPARENT_FRAME; */
/* OBSOLETE   if (innermost_frame) */
/* OBSOLETE     { */
/* OBSOLETE       frame->saved_msp = read_register (MSP_REGNUM) + msize; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       if (mfp_used) */
/* OBSOLETE 	frame->saved_msp = */
/* OBSOLETE 	  read_register_stack_integer (frame->frame + rsize - 4, 4); */
/* OBSOLETE       else */
/* OBSOLETE 	frame->saved_msp = frame->next->saved_msp + msize; */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE init_extra_frame_info (struct frame_info *frame) */
/* OBSOLETE { */
/* OBSOLETE   if (frame->next == 0) */
/* OBSOLETE     /* Assume innermost frame.  May produce strange results for "info frame" */
/* OBSOLETE        but there isn't any way to tell the difference.  */ */
/* OBSOLETE     init_frame_info (1, frame); */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* We're in get_prev_frame. */
/* OBSOLETE          Take care of everything in init_frame_pc.  */ */
/* OBSOLETE       ; */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE init_frame_pc (int fromleaf, struct frame_info *frame) */
/* OBSOLETE { */
/* OBSOLETE   frame->pc = (fromleaf ? SAVED_PC_AFTER_CALL (frame->next) : */
/* OBSOLETE 	       frame->next ? FRAME_SAVED_PC (frame->next) : read_pc ()); */
/* OBSOLETE   init_frame_info (fromleaf, frame); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Local variables (i.e. LOC_LOCAL) are on the memory stack, with their */
/* OBSOLETE    offsets being relative to the memory stack pointer (high C) or */
/* OBSOLETE    saved_msp (gcc).  */ */
/* OBSOLETE  */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE frame_locals_address (struct frame_info *fi) */
/* OBSOLETE { */
/* OBSOLETE   if (fi->flags & MFP_USED) */
/* OBSOLETE     return fi->saved_msp; */
/* OBSOLETE   else */
/* OBSOLETE     return fi->saved_msp - fi->msize; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Routines for reading the register stack.  The caller gets to treat */
/* OBSOLETE    the register stack as a uniform stack in memory, from address $gr1 */
/* OBSOLETE    straight through $rfb and beyond.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Analogous to read_memory except the length is understood to be 4. */
/* OBSOLETE    Also, myaddr can be NULL (meaning don't bother to read), and */
/* OBSOLETE    if actual_mem_addr is non-NULL, store there the address that it */
/* OBSOLETE    was fetched from (or if from a register the offset within */
/* OBSOLETE    registers).  Set *LVAL to lval_memory or lval_register, depending */
/* OBSOLETE    on where it came from.  The contents written into MYADDR are in */
/* OBSOLETE    target format.  */ */
/* OBSOLETE void */
/* OBSOLETE read_register_stack (CORE_ADDR memaddr, char *myaddr, */
/* OBSOLETE 		     CORE_ADDR *actual_mem_addr, enum lval_type *lval) */
/* OBSOLETE { */
/* OBSOLETE   long rfb = read_register (RFB_REGNUM); */
/* OBSOLETE   long rsp = read_register (RSP_REGNUM); */
/* OBSOLETE  */
/* OBSOLETE   /* If we don't do this 'info register' stops in the middle. */ */
/* OBSOLETE   if (memaddr >= rstack_high_address) */
/* OBSOLETE     { */
/* OBSOLETE       /* a bogus value */ */
/* OBSOLETE       static char val[] = */
/* OBSOLETE       {~0, ~0, ~0, ~0}; */
/* OBSOLETE       /* It's in a local register, but off the end of the stack.  */ */
/* OBSOLETE       int regnum = (memaddr - rsp) / 4 + LR0_REGNUM; */
/* OBSOLETE       if (myaddr != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Provide bogusness */ */
/* OBSOLETE 	  memcpy (myaddr, val, 4); */
/* OBSOLETE 	} */
/* OBSOLETE       supply_register (regnum, val);	/* More bogusness */ */
/* OBSOLETE       if (lval != NULL) */
/* OBSOLETE 	*lval = lval_register; */
/* OBSOLETE       if (actual_mem_addr != NULL) */
/* OBSOLETE 	*actual_mem_addr = REGISTER_BYTE (regnum); */
/* OBSOLETE     } */
/* OBSOLETE   /* If it's in the part of the register stack that's in real registers, */
/* OBSOLETE      get the value from the registers.  If it's anywhere else in memory */
/* OBSOLETE      (e.g. in another thread's saved stack), skip this part and get */
/* OBSOLETE      it from real live memory.  */ */
/* OBSOLETE   else if (memaddr < rfb && memaddr >= rsp) */
/* OBSOLETE     { */
/* OBSOLETE       /* It's in a register.  */ */
/* OBSOLETE       int regnum = (memaddr - rsp) / 4 + LR0_REGNUM; */
/* OBSOLETE       if (regnum > LR0_REGNUM + 127) */
/* OBSOLETE 	error ("Attempt to read register stack out of range."); */
/* OBSOLETE       if (myaddr != NULL) */
/* OBSOLETE 	read_register_gen (regnum, myaddr); */
/* OBSOLETE       if (lval != NULL) */
/* OBSOLETE 	*lval = lval_register; */
/* OBSOLETE       if (actual_mem_addr != NULL) */
/* OBSOLETE 	*actual_mem_addr = REGISTER_BYTE (regnum); */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* It's in the memory portion of the register stack.  */ */
/* OBSOLETE       if (myaddr != NULL) */
/* OBSOLETE 	read_memory (memaddr, myaddr, 4); */
/* OBSOLETE       if (lval != NULL) */
/* OBSOLETE 	*lval = lval_memory; */
/* OBSOLETE       if (actual_mem_addr != NULL) */
/* OBSOLETE 	*actual_mem_addr = memaddr; */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Analogous to read_memory_integer */
/* OBSOLETE    except the length is understood to be 4.  */ */
/* OBSOLETE long */
/* OBSOLETE read_register_stack_integer (CORE_ADDR memaddr, int len) */
/* OBSOLETE { */
/* OBSOLETE   char buf[4]; */
/* OBSOLETE   read_register_stack (memaddr, buf, NULL, NULL); */
/* OBSOLETE   return extract_signed_integer (buf, 4); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Copy 4 bytes from GDB memory at MYADDR into inferior memory */
/* OBSOLETE    at MEMADDR and put the actual address written into in */
/* OBSOLETE    *ACTUAL_MEM_ADDR.  */ */
/* OBSOLETE static void */
/* OBSOLETE write_register_stack (CORE_ADDR memaddr, char *myaddr, */
/* OBSOLETE 		      CORE_ADDR *actual_mem_addr) */
/* OBSOLETE { */
/* OBSOLETE   long rfb = read_register (RFB_REGNUM); */
/* OBSOLETE   long rsp = read_register (RSP_REGNUM); */
/* OBSOLETE   /* If we don't do this 'info register' stops in the middle. */ */
/* OBSOLETE   if (memaddr >= rstack_high_address) */
/* OBSOLETE     { */
/* OBSOLETE       /* It's in a register, but off the end of the stack.  */ */
/* OBSOLETE       if (actual_mem_addr != NULL) */
/* OBSOLETE 	*actual_mem_addr = 0; */
/* OBSOLETE     } */
/* OBSOLETE   else if (memaddr < rfb) */
/* OBSOLETE     { */
/* OBSOLETE       /* It's in a register.  */ */
/* OBSOLETE       int regnum = (memaddr - rsp) / 4 + LR0_REGNUM; */
/* OBSOLETE       if (regnum < LR0_REGNUM || regnum > LR0_REGNUM + 127) */
/* OBSOLETE 	error ("Attempt to read register stack out of range."); */
/* OBSOLETE       if (myaddr != NULL) */
/* OBSOLETE 	write_register (regnum, *(long *) myaddr); */
/* OBSOLETE       if (actual_mem_addr != NULL) */
/* OBSOLETE 	*actual_mem_addr = 0; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* It's in the memory portion of the register stack.  */ */
/* OBSOLETE       if (myaddr != NULL) */
/* OBSOLETE 	write_memory (memaddr, myaddr, 4); */
/* OBSOLETE       if (actual_mem_addr != NULL) */
/* OBSOLETE 	*actual_mem_addr = memaddr; */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Find register number REGNUM relative to FRAME and put its */
/* OBSOLETE    (raw) contents in *RAW_BUFFER.  Set *OPTIMIZED if the variable */
/* OBSOLETE    was optimized out (and thus can't be fetched).  If the variable */
/* OBSOLETE    was fetched from memory, set *ADDRP to where it was fetched from, */
/* OBSOLETE    otherwise it was fetched from a register. */
/* OBSOLETE  */
/* OBSOLETE    The argument RAW_BUFFER must point to aligned memory.  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE a29k_get_saved_register (char *raw_buffer, int *optimized, CORE_ADDR *addrp, */
/* OBSOLETE 			 struct frame_info *frame, int regnum, */
/* OBSOLETE 			 enum lval_type *lvalp) */
/* OBSOLETE { */
/* OBSOLETE   struct frame_info *fi; */
/* OBSOLETE   CORE_ADDR addr; */
/* OBSOLETE   enum lval_type lval; */
/* OBSOLETE  */
/* OBSOLETE   if (!target_has_registers) */
/* OBSOLETE     error ("No registers."); */
/* OBSOLETE  */
/* OBSOLETE   /* Probably now redundant with the target_has_registers check.  */ */
/* OBSOLETE   if (frame == 0) */
/* OBSOLETE     return; */
/* OBSOLETE  */
/* OBSOLETE   /* Once something has a register number, it doesn't get optimized out.  */ */
/* OBSOLETE   if (optimized != NULL) */
/* OBSOLETE     *optimized = 0; */
/* OBSOLETE   if (regnum == RSP_REGNUM) */
/* OBSOLETE     { */
/* OBSOLETE       if (raw_buffer != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  store_address (raw_buffer, REGISTER_RAW_SIZE (regnum), frame->frame); */
/* OBSOLETE 	} */
/* OBSOLETE       if (lvalp != NULL) */
/* OBSOLETE 	*lvalp = not_lval; */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE   else if (regnum == PC_REGNUM && frame->next != NULL) */
/* OBSOLETE     { */
/* OBSOLETE       if (raw_buffer != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  store_address (raw_buffer, REGISTER_RAW_SIZE (regnum), frame->pc); */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE       /* Not sure we have to do this.  */ */
/* OBSOLETE       if (lvalp != NULL) */
/* OBSOLETE 	*lvalp = not_lval; */
/* OBSOLETE  */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE   else if (regnum == MSP_REGNUM) */
/* OBSOLETE     { */
/* OBSOLETE       if (raw_buffer != NULL) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  if (frame->next != NULL) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      store_address (raw_buffer, REGISTER_RAW_SIZE (regnum), */
/* OBSOLETE 			     frame->next->saved_msp); */
/* OBSOLETE 	    } */
/* OBSOLETE 	  else */
/* OBSOLETE 	    read_register_gen (MSP_REGNUM, raw_buffer); */
/* OBSOLETE 	} */
/* OBSOLETE       /* The value may have been computed, not fetched.  */ */
/* OBSOLETE       if (lvalp != NULL) */
/* OBSOLETE 	*lvalp = not_lval; */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE   else if (regnum < LR0_REGNUM || regnum >= LR0_REGNUM + 128) */
/* OBSOLETE     { */
/* OBSOLETE       /* These registers are not saved over procedure calls, */
/* OBSOLETE          so just print out the current values.  */ */
/* OBSOLETE       if (raw_buffer != NULL) */
/* OBSOLETE 	read_register_gen (regnum, raw_buffer); */
/* OBSOLETE       if (lvalp != NULL) */
/* OBSOLETE 	*lvalp = lval_register; */
/* OBSOLETE       if (addrp != NULL) */
/* OBSOLETE 	*addrp = REGISTER_BYTE (regnum); */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   addr = frame->frame + (regnum - LR0_REGNUM) * 4; */
/* OBSOLETE   if (raw_buffer != NULL) */
/* OBSOLETE     read_register_stack (addr, raw_buffer, &addr, &lval); */
/* OBSOLETE   if (lvalp != NULL) */
/* OBSOLETE     *lvalp = lval; */
/* OBSOLETE   if (addrp != NULL) */
/* OBSOLETE     *addrp = addr; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame, */
/* OBSOLETE    restoring all saved registers.  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE pop_frame (void) */
/* OBSOLETE { */
/* OBSOLETE   struct frame_info *frame = get_current_frame (); */
/* OBSOLETE   CORE_ADDR rfb = read_register (RFB_REGNUM); */
/* OBSOLETE   CORE_ADDR gr1 = frame->frame + frame->rsize; */
/* OBSOLETE   CORE_ADDR lr1; */
/* OBSOLETE   CORE_ADDR original_lr0; */
/* OBSOLETE   int must_fix_lr0 = 0; */
/* OBSOLETE   int i; */
/* OBSOLETE  */
/* OBSOLETE   /* If popping a dummy frame, need to restore registers.  */ */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (read_register (PC_REGNUM), */
/* OBSOLETE 			read_register (SP_REGNUM), */
/* OBSOLETE 			FRAME_FP (frame))) */
/* OBSOLETE     { */
/* OBSOLETE       int lrnum = LR0_REGNUM + DUMMY_ARG / 4; */
/* OBSOLETE       for (i = 0; i < DUMMY_SAVE_SR128; ++i) */
/* OBSOLETE 	write_register (SR_REGNUM (i + 128), read_register (lrnum++)); */
/* OBSOLETE       for (i = 0; i < DUMMY_SAVE_SR160; ++i) */
/* OBSOLETE 	write_register (SR_REGNUM (i + 160), read_register (lrnum++)); */
/* OBSOLETE       for (i = 0; i < DUMMY_SAVE_GREGS; ++i) */
/* OBSOLETE 	write_register (RETURN_REGNUM + i, read_register (lrnum++)); */
/* OBSOLETE       /* Restore the PCs and prepare to restore LR0.  */ */
/* OBSOLETE       write_register (PC_REGNUM, read_register (lrnum++)); */
/* OBSOLETE       write_register (NPC_REGNUM, read_register (lrnum++)); */
/* OBSOLETE       write_register (PC2_REGNUM, read_register (lrnum++)); */
/* OBSOLETE       original_lr0 = read_register (lrnum++); */
/* OBSOLETE       must_fix_lr0 = 1; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Restore the memory stack pointer.  */ */
/* OBSOLETE   write_register (MSP_REGNUM, frame->saved_msp); */
/* OBSOLETE   /* Restore the register stack pointer.  */ */
/* OBSOLETE   write_register (GR1_REGNUM, gr1); */
/* OBSOLETE  */
/* OBSOLETE   /* If we popped a dummy frame, restore lr0 now that gr1 has been restored. */ */
/* OBSOLETE   if (must_fix_lr0) */
/* OBSOLETE     write_register (LR0_REGNUM, original_lr0); */
/* OBSOLETE  */
/* OBSOLETE   /* Check whether we need to fill registers.  */ */
/* OBSOLETE   lr1 = read_register (LR0_REGNUM + 1); */
/* OBSOLETE   if (lr1 > rfb) */
/* OBSOLETE     { */
/* OBSOLETE       /* Fill.  */ */
/* OBSOLETE       int num_bytes = lr1 - rfb; */
/* OBSOLETE       int i; */
/* OBSOLETE       long word; */
/* OBSOLETE  */
/* OBSOLETE       write_register (RAB_REGNUM, read_register (RAB_REGNUM) + num_bytes); */
/* OBSOLETE       write_register (RFB_REGNUM, lr1); */
/* OBSOLETE       for (i = 0; i < num_bytes; i += 4) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Note: word is in host byte order.  */ */
/* OBSOLETE 	  word = read_memory_integer (rfb + i, 4); */
/* OBSOLETE 	  write_register (LR0_REGNUM + ((rfb - gr1) % 0x80) + i / 4, word); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   flush_cached_frames (); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Push an empty stack frame, to record the current PC, etc.  */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE push_dummy_frame (void) */
/* OBSOLETE { */
/* OBSOLETE   long w; */
/* OBSOLETE   CORE_ADDR rab, gr1; */
/* OBSOLETE   CORE_ADDR msp = read_register (MSP_REGNUM); */
/* OBSOLETE   int lrnum, i; */
/* OBSOLETE   CORE_ADDR original_lr0; */
/* OBSOLETE  */
/* OBSOLETE   /* Read original lr0 before changing gr1.  This order isn't really needed */
/* OBSOLETE      since GDB happens to have a snapshot of all the regs and doesn't toss */
/* OBSOLETE      it when gr1 is changed.  But it's The Right Thing To Do.  */ */
/* OBSOLETE   original_lr0 = read_register (LR0_REGNUM); */
/* OBSOLETE  */
/* OBSOLETE   /* Allocate the new frame. */ */
/* OBSOLETE   gr1 = read_register (GR1_REGNUM) - DUMMY_FRAME_RSIZE; */
/* OBSOLETE   write_register (GR1_REGNUM, gr1); */
/* OBSOLETE  */
/* OBSOLETE #ifdef VXWORKS_TARGET */
/* OBSOLETE   /* We force re-reading all registers to get the new local registers set */
/* OBSOLETE      after gr1 has been modified. This fix is due to the lack of single */
/* OBSOLETE      register read/write operation in the RPC interface between VxGDB and */
/* OBSOLETE      VxWorks. This really must be changed ! */ */
/* OBSOLETE  */
/* OBSOLETE   vx_read_register (-1); */
/* OBSOLETE  */
/* OBSOLETE #endif /* VXWORK_TARGET */ */
/* OBSOLETE  */
/* OBSOLETE   rab = read_register (RAB_REGNUM); */
/* OBSOLETE   if (gr1 < rab) */
/* OBSOLETE     { */
/* OBSOLETE       /* We need to spill registers.  */ */
/* OBSOLETE       int num_bytes = rab - gr1; */
/* OBSOLETE       CORE_ADDR rfb = read_register (RFB_REGNUM); */
/* OBSOLETE       int i; */
/* OBSOLETE       long word; */
/* OBSOLETE  */
/* OBSOLETE       write_register (RFB_REGNUM, rfb - num_bytes); */
/* OBSOLETE       write_register (RAB_REGNUM, gr1); */
/* OBSOLETE       for (i = 0; i < num_bytes; i += 4) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Note:  word is in target byte order.  */ */
/* OBSOLETE 	  read_register_gen (LR0_REGNUM + i / 4, (char *) &word); */
/* OBSOLETE 	  write_memory (rfb - num_bytes + i, (char *) &word, 4); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* There are no arguments in to the dummy frame, so we don't need */
/* OBSOLETE      more than rsize plus the return address and lr1.  */ */
/* OBSOLETE   write_register (LR0_REGNUM + 1, gr1 + DUMMY_FRAME_RSIZE + 2 * 4); */
/* OBSOLETE  */
/* OBSOLETE   /* Set the memory frame pointer.  */ */
/* OBSOLETE   write_register (LR0_REGNUM + DUMMY_FRAME_RSIZE / 4 - 1, msp); */
/* OBSOLETE  */
/* OBSOLETE   /* Allocate arg_slop.  */ */
/* OBSOLETE   write_register (MSP_REGNUM, msp - 16 * 4); */
/* OBSOLETE  */
/* OBSOLETE   /* Save registers.  */ */
/* OBSOLETE   lrnum = LR0_REGNUM + DUMMY_ARG / 4; */
/* OBSOLETE   for (i = 0; i < DUMMY_SAVE_SR128; ++i) */
/* OBSOLETE     write_register (lrnum++, read_register (SR_REGNUM (i + 128))); */
/* OBSOLETE   for (i = 0; i < DUMMY_SAVE_SR160; ++i) */
/* OBSOLETE     write_register (lrnum++, read_register (SR_REGNUM (i + 160))); */
/* OBSOLETE   for (i = 0; i < DUMMY_SAVE_GREGS; ++i) */
/* OBSOLETE     write_register (lrnum++, read_register (RETURN_REGNUM + i)); */
/* OBSOLETE   /* Save the PCs and LR0.  */ */
/* OBSOLETE   write_register (lrnum++, read_register (PC_REGNUM)); */
/* OBSOLETE   write_register (lrnum++, read_register (NPC_REGNUM)); */
/* OBSOLETE   write_register (lrnum++, read_register (PC2_REGNUM)); */
/* OBSOLETE  */
/* OBSOLETE   /* Why are we saving LR0?  What would clobber it? (the dummy frame should */
/* OBSOLETE      be below it on the register stack, no?).  */ */
/* OBSOLETE   write_register (lrnum++, original_lr0); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE    This routine takes three arguments and makes the cached frames look */
/* OBSOLETE    as if these arguments defined a frame on the cache.  This allows the */
/* OBSOLETE    rest of `info frame' to extract the important arguments without much */
/* OBSOLETE    difficulty.  Since an individual frame on the 29K is determined by */
/* OBSOLETE    three values (FP, PC, and MSP), we really need all three to do a */
/* OBSOLETE    good job.  */ */
/* OBSOLETE  */
/* OBSOLETE struct frame_info * */
/* OBSOLETE setup_arbitrary_frame (int argc, CORE_ADDR *argv) */
/* OBSOLETE { */
/* OBSOLETE   struct frame_info *frame; */
/* OBSOLETE  */
/* OBSOLETE   if (argc != 3) */
/* OBSOLETE     error ("AMD 29k frame specifications require three arguments: rsp pc msp"); */
/* OBSOLETE  */
/* OBSOLETE   frame = create_new_frame (argv[0], argv[1]); */
/* OBSOLETE  */
/* OBSOLETE   if (!frame) */
/* OBSOLETE     internal_error (__FILE__, __LINE__, */
/* OBSOLETE 		    "create_new_frame returned invalid frame id"); */
/* OBSOLETE  */
/* OBSOLETE   /* Creating a new frame munges the `frame' value from the current */
/* OBSOLETE      GR1, so we restore it again here.  FIXME, untangle all this */
/* OBSOLETE      29K frame stuff...  */ */
/* OBSOLETE   frame->frame = argv[0]; */
/* OBSOLETE  */
/* OBSOLETE   /* Our MSP is in argv[2].  It'd be intelligent if we could just */
/* OBSOLETE      save this value in the FRAME.  But the way it's set up (FIXME), */
/* OBSOLETE      we must save our caller's MSP.  We compute that by adding our */
/* OBSOLETE      memory stack frame size to our MSP.  */ */
/* OBSOLETE   frame->saved_msp = argv[2] + frame->msize; */
/* OBSOLETE  */
/* OBSOLETE   return frame; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE gdb_print_insn_a29k (bfd_vma memaddr, disassemble_info *info) */
/* OBSOLETE { */
/* OBSOLETE   if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG) */
/* OBSOLETE     return print_insn_big_a29k (memaddr, info); */
/* OBSOLETE   else */
/* OBSOLETE     return print_insn_little_a29k (memaddr, info); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE enum a29k_processor_types processor_type = a29k_unknown; */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE a29k_get_processor_type (void) */
/* OBSOLETE { */
/* OBSOLETE   unsigned int cfg_reg = (unsigned int) read_register (CFG_REGNUM); */
/* OBSOLETE  */
/* OBSOLETE   /* Most of these don't have freeze mode.  */ */
/* OBSOLETE   processor_type = a29k_no_freeze_mode; */
/* OBSOLETE  */
/* OBSOLETE   switch ((cfg_reg >> 28) & 0xf) */
/* OBSOLETE     { */
/* OBSOLETE     case 0: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am29000"); */
/* OBSOLETE       break; */
/* OBSOLETE     case 1: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am29005"); */
/* OBSOLETE       break; */
/* OBSOLETE     case 2: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am29050"); */
/* OBSOLETE       processor_type = a29k_freeze_mode; */
/* OBSOLETE       break; */
/* OBSOLETE     case 3: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am29035"); */
/* OBSOLETE       break; */
/* OBSOLETE     case 4: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am29030"); */
/* OBSOLETE       break; */
/* OBSOLETE     case 5: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am2920*"); */
/* OBSOLETE       break; */
/* OBSOLETE     case 6: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am2924*"); */
/* OBSOLETE       break; */
/* OBSOLETE     case 7: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an Am29040"); */
/* OBSOLETE       break; */
/* OBSOLETE     default: */
/* OBSOLETE       fprintf_filtered (gdb_stderr, "Remote debugging an unknown Am29k\n"); */
/* OBSOLETE       /* Don't bother to print the revision.  */ */
/* OBSOLETE       return; */
/* OBSOLETE     } */
/* OBSOLETE   fprintf_filtered (gdb_stderr, " revision %c\n", 'A' + ((cfg_reg >> 24) & 0x0f)); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #ifdef GET_LONGJMP_TARGET */
/* OBSOLETE /* Figure out where the longjmp will land.  We expect that we have just entered */
/* OBSOLETE    longjmp and haven't yet setup the stack frame, so the args are still in the */
/* OBSOLETE    output regs.  lr2 (LR2_REGNUM) points at the jmp_buf structure from which we */
/* OBSOLETE    extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR. */
/* OBSOLETE    This routine returns true on success */ */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE get_longjmp_target (CORE_ADDR *pc) */
/* OBSOLETE { */
/* OBSOLETE   CORE_ADDR jb_addr; */
/* OBSOLETE   char buf[sizeof (CORE_ADDR)]; */
/* OBSOLETE  */
/* OBSOLETE   jb_addr = read_register (LR2_REGNUM); */
/* OBSOLETE  */
/* OBSOLETE   if (target_read_memory (jb_addr + JB_PC * JB_ELEMENT_SIZE, (char *) buf, */
/* OBSOLETE 			  sizeof (CORE_ADDR))) */
/* OBSOLETE     return 0; */
/* OBSOLETE  */
/* OBSOLETE   *pc = extract_address ((PTR) buf, sizeof (CORE_ADDR)); */
/* OBSOLETE   return 1; */
/* OBSOLETE } */
/* OBSOLETE #endif /* GET_LONGJMP_TARGET */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE _initialize_a29k_tdep (void) */
/* OBSOLETE { */
/* OBSOLETE   extern CORE_ADDR text_end; */
/* OBSOLETE  */
/* OBSOLETE   tm_print_insn = gdb_print_insn_a29k; */
/* OBSOLETE  */
/* OBSOLETE   /* FIXME, there should be a way to make a CORE_ADDR variable settable. */ */
/* OBSOLETE   add_show_from_set */
/* OBSOLETE     (add_set_cmd ("rstack_high_address", class_support, var_uinteger, */
/* OBSOLETE 		  (char *) &rstack_high_address, */
/* OBSOLETE 		  "Set top address in memory of the register stack.\n\ */
/* OBSOLETE Attempts to access registers saved above this address will be ignored\n\ */
/* OBSOLETE or will produce the value -1.", &setlist), */
/* OBSOLETE      &showlist); */
/* OBSOLETE  */
/* OBSOLETE   /* FIXME, there should be a way to make a CORE_ADDR variable settable. */ */
/* OBSOLETE   add_show_from_set */
/* OBSOLETE     (add_set_cmd ("call_scratch_address", class_support, var_uinteger, */
/* OBSOLETE 		  (char *) &text_end, */
/* OBSOLETE 		  "Set address in memory where small amounts of RAM can be used\n\ */
/* OBSOLETE when making function calls into the inferior.", &setlist), */
/* OBSOLETE      &showlist); */
/* OBSOLETE } */
