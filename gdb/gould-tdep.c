/* OBSOLETE /* GOULD RISC target-dependent code for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE This program is free software; you can redistribute it and/or modify */
/* OBSOLETE it under the terms of the GNU General Public License as published by */
/* OBSOLETE the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE This program is distributed in the hope that it will be useful, */
/* OBSOLETE but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE You should have received a copy of the GNU General Public License */
/* OBSOLETE along with this program; if not, write to the Free Software */
/* OBSOLETE Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "symtab.h" */
/* OBSOLETE #include "frame.h" */
/* OBSOLETE #include "gdbcore.h" */
/* OBSOLETE #if defined GOULD_PN */
/* OBSOLETE #include "opcode/pn.h" */
/* OBSOLETE #else */
/* OBSOLETE #include "opcode/np1.h" */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* GOULD RISC instructions are never longer than this many bytes.  *x/ */
/* OBSOLETE #define MAXLEN 4 */
/* OBSOLETE  */
/* OBSOLETE /* Number of elements in the opcode table.  *x/ */
/* OBSOLETE #define NOPCODES (sizeof gld_opcodes / sizeof gld_opcodes[0]) */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE gould_frame_chain_valid (chain, fi) */
/* OBSOLETE      CORE_ADDR chain; */
/* OBSOLETE      struct frame_info *fi;      /* not used here *x/ */
/* OBSOLETE { */
/* OBSOLETE   return (chain != 0 && chain != (thisframe)->frame); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Both gcc and cc return small structs in registers (i.e. in GDB */
/* OBSOLETE    terminology, small structs don't use the struct return convention).  *x/ */
/* OBSOLETE int */
/* OBSOLETE gould_use_struct_convention (gcc_p, type) */
/* OBSOLETE      int gcc_p; */
/* OBSOLETE      struct type *type; */
/* OBSOLETE { */
/* OBSOLETE   return (TYPE_LENGTH(type) > 8); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Print the GOULD instruction at address MEMADDR in debugged memory, */
/* OBSOLETE    on STREAM.  Returns length of the instruction, in bytes.  *x/ */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE gould_print_insn (memaddr, stream) */
/* OBSOLETE      CORE_ADDR memaddr; */
/* OBSOLETE      FILE *stream; */
/* OBSOLETE { */
/* OBSOLETE 	unsigned char buffer[MAXLEN]; */
/* OBSOLETE 	register int i; */
/* OBSOLETE 	register char *d; */
/* OBSOLETE 	register int bestmask; */
/* OBSOLETE 	unsigned best; */
/* OBSOLETE 	int temp, index, bestlen; */
/* OBSOLETE  */
/* OBSOLETE 	read_memory (memaddr, buffer, MAXLEN); */
/* OBSOLETE  */
/* OBSOLETE 	bestmask = 0; */
/* OBSOLETE 	index = -1; */
/* OBSOLETE 	best = 0xffffffff; */
/* OBSOLETE 	for (i = 0; i < NOPCODES; i++) */
/* OBSOLETE 	{ */
/* OBSOLETE 		register unsigned int opcode = gld_opcodes[i].opcode; */
/* OBSOLETE 		register unsigned int mask = gld_opcodes[i].mask; */
/* OBSOLETE 		register unsigned int len = gld_opcodes[i].length; */
/* OBSOLETE 		register unsigned int test; */
/* OBSOLETE  */
/* OBSOLETE 		/* Get possible opcode bytes into integer *x/ */
/* OBSOLETE 		test = buffer[0] << 24; */
/* OBSOLETE 		test |= buffer[1] << 16; */
/* OBSOLETE 		test |= buffer[2] << 8; */
/* OBSOLETE 		test |= buffer[3]; */
/* OBSOLETE  */
/* OBSOLETE 		/* Mask with opcode and see if match *x/ */
/* OBSOLETE 		if ((opcode & mask) == (test & mask)) */
/* OBSOLETE 		{ */
/* OBSOLETE 			/* See if second or third match *x/ */
/* OBSOLETE 			if (index >= 0) */
/* OBSOLETE 			{ */
/* OBSOLETE 				/* Take new one if it looks good *x/ */
/* OBSOLETE 				if (bestlen == MAXLEN && len == MAXLEN) */
/* OBSOLETE 				{ */
/* OBSOLETE 					/* See if lower bits matched *x/ */
/* OBSOLETE 					if (((bestmask & 3) == 0) && */
/* OBSOLETE 					    ((mask & 3) != 0)) */
/* OBSOLETE 					{ */
/* OBSOLETE 						bestmask = mask; */
/* OBSOLETE 						bestlen = len; */
/* OBSOLETE 						best = test; */
/* OBSOLETE 						index = i; */
/* OBSOLETE 					} */
/* OBSOLETE 				} */
/* OBSOLETE 			} */
/* OBSOLETE 			else */
/* OBSOLETE 			{ */
/* OBSOLETE 				/* First match, save it *x/ */
/* OBSOLETE 				bestmask = mask; */
/* OBSOLETE 				bestlen = len; */
/* OBSOLETE 				best = test; */
/* OBSOLETE 				index = i; */
/* OBSOLETE 			} */
/* OBSOLETE 		} */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE 	/* Handle undefined instructions.  *x/ */
/* OBSOLETE 	if (index < 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 		fprintf (stream, "undefined   0%o",(buffer[0]<<8)+buffer[1]); */
/* OBSOLETE 		return 2; */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE 	/* Print instruction name *x/ */
/* OBSOLETE 	fprintf (stream, "%-12s", gld_opcodes[index].name); */
/* OBSOLETE  */
/* OBSOLETE 	/* Adjust if short instruction *x/ */
/* OBSOLETE 	if (gld_opcodes[index].length < 4) */
/* OBSOLETE 	{ */
/* OBSOLETE 		best >>= 16; */
/* OBSOLETE 		i = 0; */
/* OBSOLETE 	} */
/* OBSOLETE 	else */
/* OBSOLETE 	{ */
/* OBSOLETE 		i = 16; */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE 	/* Dump out instruction arguments *x/ */
/* OBSOLETE   	for (d = gld_opcodes[index].args; *d; ++d) */
/* OBSOLETE 	{ */
/* OBSOLETE 	    switch (*d) */
/* OBSOLETE 	    { */
/* OBSOLETE 		case 'f': */
/* OBSOLETE 		    fprintf (stream, "%d",  (best >> (7 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'r': */
/* OBSOLETE 		    fprintf (stream, "r%d", (best >> (7 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'R': */
/* OBSOLETE 		    fprintf (stream, "r%d", (best >> (4 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'b': */
/* OBSOLETE 		    fprintf (stream, "b%d", (best >> (7 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'B': */
/* OBSOLETE 		    fprintf (stream, "b%d", (best >> (4 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'v': */
/* OBSOLETE 		    fprintf (stream, "b%d", (best >> (7 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'V': */
/* OBSOLETE 		    fprintf (stream, "b%d", (best >> (4 + i)) & 7); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'X': */
/* OBSOLETE 		    temp = (best >> 20) & 7; */
/* OBSOLETE 		    if (temp) */
/* OBSOLETE 			fprintf (stream, "r%d", temp); */
/* OBSOLETE 		    else */
/* OBSOLETE 			putc ('0', stream); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'A': */
/* OBSOLETE 		    temp = (best >> 16) & 7; */
/* OBSOLETE 		    if (temp) */
/* OBSOLETE 			fprintf (stream, "(b%d)", temp); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'S': */
/* OBSOLETE 		    fprintf (stream, "#%d", best & 0x1f); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'I': */
/* OBSOLETE 		    fprintf (stream, "#%x", best & 0xffff); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'O': */
/* OBSOLETE 		    fprintf (stream, "%x", best & 0xffff); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'h': */
/* OBSOLETE 		    fprintf (stream, "%d", best & 0xfffe); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'd': */
/* OBSOLETE 		    fprintf (stream, "%d", best & 0xfffc); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'T': */
/* OBSOLETE 		    fprintf (stream, "%d", (best >> 8) & 0xff); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		case 'N': */
/* OBSOLETE 		    fprintf (stream, "%d", best & 0xff); */
/* OBSOLETE 		    break; */
/* OBSOLETE 		default: */
/* OBSOLETE 		    putc (*d, stream); */
/* OBSOLETE 		    break; */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE  */
/* OBSOLETE 	/* Return length of instruction *x/ */
/* OBSOLETE   	return (gld_opcodes[index].length); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Find the number of arguments to a function. */
/* OBSOLETE  *x/ */
/* OBSOLETE findarg(frame) */
/* OBSOLETE 	struct frame_info *frame; */
/* OBSOLETE { */
/* OBSOLETE 	register struct symbol *func; */
/* OBSOLETE 	register unsigned pc; */
/* OBSOLETE  */
/* OBSOLETE #ifdef notdef */
/* OBSOLETE 	/* find starting address of frame function *x/ */
/* OBSOLETE 	pc = get_pc_function_start (frame->pc); */
/* OBSOLETE  */
/* OBSOLETE 	/* find function symbol info *x/ */
/* OBSOLETE 	func = find_pc_function (pc); */
/* OBSOLETE  */
/* OBSOLETE 	/* call blockframe code to look for match *x/ */
/* OBSOLETE 	if (func != NULL) */
/* OBSOLETE                 return (func->value.block->nsyms / sizeof(int)); */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE         return (-1); */
/* OBSOLETE }  */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * In the case of the NPL, the frame's norminal address is Br2 and the  */
/* OBSOLETE  * previous routines frame is up the stack X bytes.  Finding out what */
/* OBSOLETE  * 'X' is can be tricky. */
/* OBSOLETE  * */
/* OBSOLETE  *    1.) stored in the code function header xA(Br1). */
/* OBSOLETE  *    2.) must be careful of recurssion. */
/* OBSOLETE  *x/ */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE findframe(thisframe) */
/* OBSOLETE     struct frame_info *thisframe; */
/* OBSOLETE { */
/* OBSOLETE     register CORE_ADDR pointer; */
/* OBSOLETE     CORE_ADDR framechain(); */
/* OBSOLETE #if 0     */
/* OBSOLETE     struct frame_info *frame; */
/* OBSOLETE  */
/* OBSOLETE     /* Setup toplevel frame structure *x/ */
/* OBSOLETE     frame->pc = read_pc(); */
/* OBSOLETE     frame->next_frame = 0; */
/* OBSOLETE     frame->frame = read_register (SP_REGNUM);	/* Br2 *x/ */
/* OBSOLETE  */
/* OBSOLETE     /* Search for this frame (start at current Br2) *x/ */
/* OBSOLETE     do */
/* OBSOLETE     { */
/* OBSOLETE 	pointer = framechain(frame); */
/* OBSOLETE 	frame->next_frame = frame->frame; */
/* OBSOLETE 	frame->frame = pointer; */
/* OBSOLETE 	frame->pc = FRAME_SAVED_PC(frame); */
/* OBSOLETE     } */
/* OBSOLETE     while (frame->next_frame != thisframe); */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE     pointer = framechain (thisframe); */
/* OBSOLETE  */
/* OBSOLETE     /* stop gap for now, end at __base3 *x/ */
/* OBSOLETE     if (thisframe->pc == 0) */
/* OBSOLETE 	return 0; */
/* OBSOLETE  */
/* OBSOLETE     return pointer; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Gdb front-end and internal framechain routine. */
/* OBSOLETE  * Go back up stack one level.  Tricky... */
/* OBSOLETE  *x/ */
/* OBSOLETE CORE_ADDR */
/* OBSOLETE framechain(frame) */
/* OBSOLETE     register struct frame_info *frame; */
/* OBSOLETE { */
/* OBSOLETE     register CORE_ADDR func, prevsp; */
/* OBSOLETE     register unsigned value; */
/* OBSOLETE  */
/* OBSOLETE     /* Get real function start address from internal frame address *x/ */
/* OBSOLETE     func = get_pc_function_start(frame->pc); */
/* OBSOLETE  */
/* OBSOLETE     /* If no stack given, read register Br1 "(sp)" *x/ */
/* OBSOLETE     if (!frame->frame) */
/* OBSOLETE 	prevsp = read_register (SP_REGNUM); */
/* OBSOLETE     else */
/* OBSOLETE 	prevsp = frame->frame; */
/* OBSOLETE  */
/* OBSOLETE     /* Check function header, case #2 *x/ */
/* OBSOLETE     value = read_memory_integer (func, 4); */
/* OBSOLETE     if (value) */
/* OBSOLETE     { */
/* OBSOLETE 	/* 32bit call push value stored in function header *x/ */
/* OBSOLETE 	prevsp += value; */
/* OBSOLETE     } */
/* OBSOLETE     else */
/* OBSOLETE     { */
/* OBSOLETE 	/* read half-word from suabr at start of function *x/ */
/* OBSOLETE 	prevsp += read_memory_integer (func + 10, 2); */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE     return (prevsp); */
/* OBSOLETE } */
