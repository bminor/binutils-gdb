/* OBSOLETE /* Definitions to target GDB on an ISI Optimum V (3.05) under 4.3bsd. */
/* OBSOLETE    Copyright 1987, 1989, 1991, 1993, 1999, 2000 */
/* OBSOLETE    Free Software Foundation, Inc. */
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
/* OBSOLETE /* This has not been tested on ISI's running BSD 4.2, but it will probably */
/* OBSOLETE    work.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Data segment starts at etext rounded up to DATAROUND in {N,Z}MAGIC files */ */
/* OBSOLETE  */
/* OBSOLETE #define DATAROUND	0x20000 */
/* OBSOLETE #define N_DATADDR(hdr)	(hdr.a_magic != OMAGIC ? \ */
/* OBSOLETE 	(hdr.a_text + DATAROUND) & ~(DATAROUND-1) : hdr.a_text) */
/* OBSOLETE  */
/* OBSOLETE /* Text segment starts at sizeof (struct exec) in {N,Z}MAGIC files */ */
/* OBSOLETE  */
/* OBSOLETE #define N_TXTADDR(hdr)	(hdr.a_magic != OMAGIC ? sizeof (struct exec) : 0) */
/* OBSOLETE  */
/* OBSOLETE /* Amount PC must be decremented by after a breakpoint. */
/* OBSOLETE    This is often the number of bytes in BREAKPOINT */
/* OBSOLETE    but not always. */
/* OBSOLETE    On the ISI, the kernel resets the pc to the trap instr */ */
/* OBSOLETE  */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 0 */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Return number of args passed to a frame. */
/* OBSOLETE    Can return -1, meaning no way to tell.  */ */
/* OBSOLETE  */
/* OBSOLETE extern int isi_frame_num_args (struct frame_info *fi); */
/* OBSOLETE #define FRAME_NUM_ARGS(fi) (isi_frame_num_args ((fi))) */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)		\ */
/* OBSOLETE { register int regnum;							\ */
/* OBSOLETE   register int regmask;							\ */
/* OBSOLETE   register CORE_ADDR next_addr;						\ */
/* OBSOLETE   register CORE_ADDR pc;						\ */
/* OBSOLETE   register int insn;							\ */
/* OBSOLETE   register int offset;							\ */
/* OBSOLETE   memset (&frame_saved_regs, '\0', sizeof frame_saved_regs);			\ */
/* OBSOLETE   if ((frame_info)->pc >= (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM*4 - 8*12 - 4 \ */
/* OBSOLETE       && (frame_info)->pc <= (frame_info)->frame)				\ */
/* OBSOLETE     { next_addr = (frame_info)->frame;					\ */
/* OBSOLETE       pc = (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 8*12 - 4; }\ */
/* OBSOLETE   else   								\ */
/* OBSOLETE     { pc = get_pc_function_start ((frame_info)->pc); 			\ */
/* OBSOLETE       /* Verify we have a link a6 instruction next,			\ */
/* OBSOLETE 	 or a branch followed by a link a6 instruction;			\ */
/* OBSOLETE 	 if not we lose.  If we win, find the address above the saved   \ */
/* OBSOLETE 	 regs using the amount of storage from the link instruction.  */\ */
/* OBSOLETE retry:									\ */
/* OBSOLETE       insn = read_memory_integer (pc, 2);				\ */
/* OBSOLETE       if (insn == 044016)						\ */
/* OBSOLETE 	next_addr = (frame_info)->frame - read_memory_integer (pc += 2, 4), pc+=4; \ */
/* OBSOLETE       else if (insn == 047126)						\ */
/* OBSOLETE 	next_addr = (frame_info)->frame - read_memory_integer (pc += 2, 2), pc+=2; \ */
/* OBSOLETE       else if ((insn & 0177400) == 060000)	/* bra insn */		\ */
/* OBSOLETE 	{ offset = insn & 0377;						\ */
/* OBSOLETE           pc += 2;				/* advance past bra */	\ */
/* OBSOLETE 	  if (offset == 0)			/* bra #word */		\ */
/* OBSOLETE 	    offset = read_memory_integer (pc, 2), pc += 2;		\ */
/* OBSOLETE 	  else if (offset == 0377)		/* bra #long */		\ */
/* OBSOLETE 	    offset = read_memory_integer (pc, 4), pc += 4;		\ */
/* OBSOLETE 	  pc += offset;							\ */
/* OBSOLETE 	  goto retry;							\ */
/* OBSOLETE       } else goto lose;							\ */
/* OBSOLETE       /* If have an addal #-n, sp next, adjust next_addr.  */		\ */
/* OBSOLETE       if ((0177777 & read_memory_integer (pc, 2)) == 0157774)		\ */
/* OBSOLETE 	next_addr += read_memory_integer (pc += 2, 4), pc += 4;		\ */
/* OBSOLETE     }									\ */
/* OBSOLETE   /* next should be a moveml to (sp) or -(sp) or a movl r,-(sp) */	\ */
/* OBSOLETE   insn = read_memory_integer (pc, 2), pc += 2;				\ */
/* OBSOLETE   regmask = read_memory_integer (pc, 2);				\ */
/* OBSOLETE   if ((insn & 0177760) == 022700)	/* movl rn, (sp) */		\ */
/* OBSOLETE     (frame_saved_regs).regs[(insn&7) + ((insn&010)?8:0)] = next_addr;	\ */
/* OBSOLETE   else if ((insn & 0177760) == 024700)	/* movl rn, -(sp) */		\ */
/* OBSOLETE     (frame_saved_regs).regs[(insn&7) + ((insn&010)?8:0)] = next_addr-=4; \ */
/* OBSOLETE   else if (insn == 0044327)		/* moveml mask, (sp) */		\ */
/* OBSOLETE     { pc += 2;								\ */
/* OBSOLETE       /* Regmask's low bit is for register 0, the first written */	\ */
/* OBSOLETE       next_addr -= 4;							\ */
/* OBSOLETE       for (regnum = 0; regnum < 16; regnum++, regmask >>= 1)		\ */
/* OBSOLETE 	if (regmask & 1)						\ */
/* OBSOLETE           (frame_saved_regs).regs[regnum] = (next_addr += 4);		\ */
/* OBSOLETE   } else if (insn == 0044347)		/* moveml mask, -(sp) */	\ */
/* OBSOLETE     { pc += 2;								\ */
/* OBSOLETE       /* Regmask's low bit is for register 15, the first pushed */	\ */
/* OBSOLETE       for (regnum = 15; regnum >= 0; regnum--, regmask >>= 1)		\ */
/* OBSOLETE 	if (regmask & 1)						\ */
/* OBSOLETE           (frame_saved_regs).regs[regnum] = (next_addr -= 4); }		\ */
/* OBSOLETE   /* clrw -(sp); movw ccr,-(sp) may follow.  */				\ */
/* OBSOLETE   if (read_memory_integer (pc, 2) == 041147 				\ */
/* OBSOLETE       && read_memory_integer (pc+2, 2) == 042347)			\ */
/* OBSOLETE     (frame_saved_regs).regs[PS_REGNUM] = (next_addr -= 4);		\ */
/* OBSOLETE   lose: ;								\ */
/* OBSOLETE   (frame_saved_regs).regs[SP_REGNUM] = (frame_info)->frame + 8;		\ */
/* OBSOLETE   (frame_saved_regs).regs[FP_REGNUM] = (frame_info)->frame;		\ */
/* OBSOLETE   (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 4;		\ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* The only reason this is here is the tm-isi.h reference below.  It */
/* OBSOLETE    was moved back here from tm-m68k.h.  FIXME? */ */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR isi_skip_prologue (CORE_ADDR); */
/* OBSOLETE #define SKIP_PROLOGUE(pc) (isi_skip_prologue (pc)) */
/* OBSOLETE  */
/* OBSOLETE #include "m68k/tm-m68k.h" */
