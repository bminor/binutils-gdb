/* OBSOLETE /* Parameters for targe of a Gould Powernode, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1993 Free Software Foundation, Inc. */
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
/* OBSOLETE #define GOULD_PN */
/* OBSOLETE  */
/* OBSOLETE #define TARGET_BYTE_ORDER BIG_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE /* This code appears in libraries on Gould machines.  Ignore it. *x/ */
/* OBSOLETE #define IGNORE_SYMBOL(type) (type == N_ENTRY) */
/* OBSOLETE  */
/* OBSOLETE /* We don't want the extra gnu symbols on the machine; */
/* OBSOLETE    they will interfere with the shared segment symbols.  *x/ */
/* OBSOLETE #define NO_GNU_STABS */
/* OBSOLETE  */
/* OBSOLETE /* Macro for text-offset and data info (in PN a.out format).  *x/ */
/* OBSOLETE #define	TEXTINFO						\ */
/* OBSOLETE 	text_offset = N_TXTOFF (exec_coffhdr);			\ */
/* OBSOLETE 	exec_data_offset = N_TXTOFF (exec_coffhdr)		\ */
/* OBSOLETE 		+ exec_aouthdr.a_text */
/* OBSOLETE  */
/* OBSOLETE /* Macro for number of symbol table entries (this used to be checked */
/* OBSOLETE    in dbxread.c and caused the last psymtab to use this as the end of */
/* OBSOLETE    text.  I'm not sure whether it would still be necessary).  *x/ */
/* OBSOLETE #define END_OF_TEXT_DEFAULT					\ */
/* OBSOLETE 	(0xffffff) */
/* OBSOLETE  */
/* OBSOLETE /* Macro for number of symbol table entries *x/ */
/* OBSOLETE #define NUMBER_OF_SYMBOLS					\ */
/* OBSOLETE 	(coffhdr.f_nsyms) */
/* OBSOLETE  */
/* OBSOLETE /* Macro for file-offset of symbol table (in usual a.out format).  *x/ */
/* OBSOLETE #define SYMBOL_TABLE_OFFSET					\ */
/* OBSOLETE 	N_SYMOFF (coffhdr) */
/* OBSOLETE  */
/* OBSOLETE /* Macro for file-offset of string table (in usual a.out format).  *x/ */
/* OBSOLETE #define STRING_TABLE_OFFSET					\ */
/* OBSOLETE 	(N_STROFF (coffhdr) + sizeof(int)) */
/* OBSOLETE  */
/* OBSOLETE /* Macro to store the length of the string table data in INTO.  *x/ */
/* OBSOLETE #define READ_STRING_TABLE_SIZE(INTO)				\ */
/* OBSOLETE 	{ INTO = hdr.a_stsize; } */
/* OBSOLETE  */
/* OBSOLETE /* Macro to declare variables to hold the file's header data.  *x/ */
/* OBSOLETE #define DECLARE_FILE_HEADERS  struct old_exec hdr;		\ */
/* OBSOLETE 			      FILHDR coffhdr */
/* OBSOLETE  */
/* OBSOLETE /* Macro to read the header data from descriptor DESC and validate it. */
/* OBSOLETE    NAME is the file name, for error messages.  *x/ */
/* OBSOLETE #define READ_FILE_HEADERS(DESC, NAME)				\ */
/* OBSOLETE { val = myread (DESC, &coffhdr, sizeof coffhdr);		\ */
/* OBSOLETE   if (val < 0)							\ */
/* OBSOLETE     perror_with_name (NAME);					\ */
/* OBSOLETE   val = myread (DESC, &hdr, sizeof hdr);			\ */
/* OBSOLETE   if (val < 0)							\ */
/* OBSOLETE     perror_with_name (NAME);					\ */
/* OBSOLETE   if (coffhdr.f_magic != GNP1MAGIC)				\ */
/* OBSOLETE     error ("File \"%s\" not in coff executable format.", NAME);	\ */
/* OBSOLETE   if (N_BADMAG (hdr))						\ */
/* OBSOLETE     error ("File \"%s\" not in executable format.", NAME); } */
/* OBSOLETE  */
/* OBSOLETE /* Define COFF and other symbolic names needed on NP1 *x/ */
/* OBSOLETE #define	NS32GMAGIC	GDPMAGIC */
/* OBSOLETE #define	NS32SMAGIC	PN_MAGIC */
/* OBSOLETE  */
/* OBSOLETE /* Offset from address of function to start of its code. */
/* OBSOLETE    Zero on most machines.  *x/ */
/* OBSOLETE #define FUNCTION_START_OFFSET	4 */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  One PN we can have one or two startup */
/* OBSOLETE    sequences depending on the size of the local stack: */
/* OBSOLETE  */
/* OBSOLETE    Either: */
/* OBSOLETE       "suabr b2, #" */
/* OBSOLETE    of */
/* OBSOLETE       "lil r4, #", "suabr b2, #(r4)" */
/* OBSOLETE  */
/* OBSOLETE    "lwbr b6, #", "stw r1, 8(b2)" */
/* OBSOLETE    Optional "stwbr b3, c(b2)" */
/* OBSOLETE    Optional "trr r2,r7"      (Gould first argument register passing) */
/* OBSOLETE      or */
/* OBSOLETE    Optional "stw r2,8(b3)"   (Gould first argument register passing) */
/* OBSOLETE  *x/ */
/* OBSOLETE #define SKIP_PROLOGUE(pc) { 						\ */
/* OBSOLETE 	register int op = read_memory_integer ((pc), 4);		\ */
/* OBSOLETE 	if ((op & 0xffff0000) == 0x580B0000) { 				\ */
/* OBSOLETE 	    pc += 4;							\ */
/* OBSOLETE 	    op = read_memory_integer ((pc), 4);				\ */
/* OBSOLETE 	    if ((op & 0xffff0000) == 0x59400000) {			\ */
/* OBSOLETE 		pc += 4;						\ */
/* OBSOLETE 	        op = read_memory_integer ((pc), 4);			\ */
/* OBSOLETE 		if ((op & 0xffff0000) == 0x5F000000) {			\ */
/* OBSOLETE 		    pc += 4;						\ */
/* OBSOLETE 	            op = read_memory_integer ((pc), 4);			\ */
/* OBSOLETE 		    if (op == 0xD4820008) {				\ */
/* OBSOLETE 		    	pc += 4;					\ */
/* OBSOLETE 	            	op = read_memory_integer ((pc), 4);		\ */
/* OBSOLETE 		    	if (op == 0x5582000C) {				\ */
/* OBSOLETE 		    	    pc += 4;					\ */
/* OBSOLETE 	                    op = read_memory_integer ((pc), 2);		\ */
/* OBSOLETE 		            if (op == 0x2fa0) {				\ */
/* OBSOLETE 		    	        pc += 2;				\ */
/* OBSOLETE 		            } else {					\ */
/* OBSOLETE 	                        op = read_memory_integer ((pc), 4);	\ */
/* OBSOLETE 		                if (op == 0xd5030008) {			\ */
/* OBSOLETE 		    	            pc += 4;				\ */
/* OBSOLETE 		                }					\ */
/* OBSOLETE 		            }						\ */
/* OBSOLETE 		        } else {					\ */
/* OBSOLETE 	                    op = read_memory_integer ((pc), 2);		\ */
/* OBSOLETE 		            if (op == 0x2fa0) {				\ */
/* OBSOLETE 		    	        pc += 2;				\ */
/* OBSOLETE 		            }						\ */
/* OBSOLETE 		        }						\ */
/* OBSOLETE 		    }							\ */
/* OBSOLETE 		}							\ */
/* OBSOLETE 	    }								\ */
/* OBSOLETE 	} 								\ */
/* OBSOLETE 	if ((op & 0xffff0000) == 0x59000000) { 				\ */
/* OBSOLETE 	    pc += 4;							\ */
/* OBSOLETE 	    op = read_memory_integer ((pc), 4);				\ */
/* OBSOLETE 	    if ((op & 0xffff0000) == 0x5F000000) {			\ */
/* OBSOLETE 		pc += 4;						\ */
/* OBSOLETE 	        op = read_memory_integer ((pc), 4);			\ */
/* OBSOLETE 		if (op == 0xD4820008) {					\ */
/* OBSOLETE 		    pc += 4;						\ */
/* OBSOLETE 	            op = read_memory_integer ((pc), 4);			\ */
/* OBSOLETE 		    if (op == 0x5582000C) {				\ */
/* OBSOLETE 		    	pc += 4;					\ */
/* OBSOLETE 	                op = read_memory_integer ((pc), 2);		\ */
/* OBSOLETE 		        if (op == 0x2fa0) {				\ */
/* OBSOLETE 		    	    pc += 2;					\ */
/* OBSOLETE 		        } else {					\ */
/* OBSOLETE 	                    op = read_memory_integer ((pc), 4);		\ */
/* OBSOLETE 		            if (op == 0xd5030008) {			\ */
/* OBSOLETE 		    	        pc += 4;				\ */
/* OBSOLETE 		            }						\ */
/* OBSOLETE 		        }						\ */
/* OBSOLETE 		    } else {						\ */
/* OBSOLETE 	                op = read_memory_integer ((pc), 2);		\ */
/* OBSOLETE 		        if (op == 0x2fa0) {				\ */
/* OBSOLETE 		    	    pc += 2;					\ */
/* OBSOLETE 		        }						\ */
/* OBSOLETE 		    }							\ */
/* OBSOLETE 		}							\ */
/* OBSOLETE 	    }								\ */
/* OBSOLETE 	} 								\ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Immediately after a function call, return the saved pc. */
/* OBSOLETE    Can't go through the frames for this because on some machines */
/* OBSOLETE    the new frame is not set up until the new function executes */
/* OBSOLETE    some instructions.  True on PN!  Return address is in R1. */
/* OBSOLETE    Note: true return location is 4 bytes past R1! *x/ */
/* OBSOLETE #define SAVED_PC_AFTER_CALL(frame) \ */
/* OBSOLETE 	(read_register(R1_REGNUM) + 4) */
/* OBSOLETE  */
/* OBSOLETE /* Address of end of stack space.  *x/ */
/* OBSOLETE #define STACK_END_ADDR 		0x480000 */
/* OBSOLETE  */
/* OBSOLETE /* Stack grows downward.  *x/ */
/* OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs)) */
/* OBSOLETE  */
/* OBSOLETE /* Sequence of bytes for breakpoint instruction.  *x/ */
/* OBSOLETE #define BREAKPOINT 		{0x28, 0x09} */
/* OBSOLETE  */
/* OBSOLETE /* Amount PC must be decremented by after a breakpoint. */
/* OBSOLETE    This is often the number of bytes in BREAKPOINT */
/* OBSOLETE    but not always.  *x/ */
/* OBSOLETE #define DECR_PC_AFTER_BREAK	2 */
/* OBSOLETE  */
/* OBSOLETE /* Return 1 if P points to an invalid floating point value.  *x/ */
/* OBSOLETE #define INVALID_FLOAT(p, len) 	((*(short *)p & 0xff80) == 0x8000) */
/* OBSOLETE  */
/* OBSOLETE /* Say how long (ordinary) registers are.  This is a piece of bogosity */
/* OBSOLETE    used in push_word and a few other places; REGISTER_RAW_SIZE is the */
/* OBSOLETE    real way to know how big a register is.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Number of machine registers *x/ */
/* OBSOLETE #define NUM_REGS 		19 */
/* OBSOLETE #define NUM_GEN_REGS		16 */
/* OBSOLETE #define NUM_CPU_REGS		3 */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    There should be NUM_REGS strings in this initializer.  *x/ */
/* OBSOLETE #define REGISTER_NAMES { \ */
/* OBSOLETE   "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \ */
/* OBSOLETE   "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", \ */
/* OBSOLETE   "sp", "ps", "pc", \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Register numbers of various important registers. */
/* OBSOLETE    Note that some of these values are "real" register numbers, */
/* OBSOLETE    and correspond to the general registers of the machine, */
/* OBSOLETE    and some are "phony" register numbers which are too large */
/* OBSOLETE    to be actual register numbers as far as the user is concerned */
/* OBSOLETE    but do serve to get the desired values when passed to read_register.  *x/ */
/* OBSOLETE #define R1_REGNUM	1	/* Gr1 => return address of caller *x/ */
/* OBSOLETE #define R4_REGNUM	4	/* Gr4 => register save area *x/ */
/* OBSOLETE #define R5_REGNUM	5	/* Gr5 => register save area *x/ */
/* OBSOLETE #define R6_REGNUM	6	/* Gr6 => register save area *x/ */
/* OBSOLETE #define R7_REGNUM	7	/* Gr7 => register save area *x/ */
/* OBSOLETE #define B1_REGNUM	9	/* Br1 => start of this code routine *x/ */
/* OBSOLETE #define FP_REGNUM	10	/* Br2 == (sp) *x/ */
/* OBSOLETE #define AP_REGNUM	11	/* Br3 == (ap) *x/ */
/* OBSOLETE #define SP_REGNUM 	16	/* A copy of Br2 saved in trap *x/ */
/* OBSOLETE #define PS_REGNUM 	17	/* Contains processor status *x/ */
/* OBSOLETE #define PC_REGNUM 	18	/* Contains program counter *x/ */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  *x/ */
/* OBSOLETE #define REGISTER_BYTES			(NUM_GEN_REGS*4 + NUM_CPU_REGS*4) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  *x/ */
/* OBSOLETE #define REGISTER_BYTE(N)  		((N) * 4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the actual machine representation */
/* OBSOLETE    for register N.  On the PN, all normal regs are 4 bytes. *x/ */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) 		(4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the program's representation */
/* OBSOLETE    for register N.  On the PN, all regs are 4 bytes. *x/ */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N)	(4) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  *x/ */
/* OBSOLETE #define MAX_REGISTER_RAW_SIZE		(4) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  *x/ */
/* OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE	(4) */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  *x/ */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N)	(builtin_type_int) */
/* OBSOLETE  */
/* OBSOLETE /* Store the address of the place in which to copy the structure the */
/* OBSOLETE    subroutine will return.  This is called from call_function. */
/* OBSOLETE  */
/* OBSOLETE    On this machine this is a no-op, because gcc isn't used on it */
/* OBSOLETE    yet.  So this calling convention is not used. *x/ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an arrary REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF. *x/ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE 	memcpy (VALBUF, REGBUF, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE 	write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    the address in which a function should return its structure value, */
/* OBSOLETE    as a CORE_ADDR (or an expression that can be used as one).  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(int *)(REGBUF)) */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Describe the pointer in each stack frame to the previous stack frame */
/* OBSOLETE    (its caller).  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* FRAME_CHAIN takes a frame's nominal address */
/* OBSOLETE    and produces the frame's chain-pointer. */
/* OBSOLETE  */
/* OBSOLETE    However, if FRAME_CHAIN_VALID returns zero, */
/* OBSOLETE    it means the given frame is the outermost one and has no caller.  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* In the case of the NPL, the frame's norminal address is Br2 and the  */
/* OBSOLETE    previous routines frame is up the stack X bytes, where X is the */
/* OBSOLETE    value stored in the code function header xA(Br1). *x/ */
/* OBSOLETE #define FRAME_CHAIN(thisframe)		(findframe(thisframe)) */
/* OBSOLETE  */
/* OBSOLETE extern int gould_frame_chain_valid PARAMS ((CORE_ADDR, struct frame_info *)); */
/* OBSOLETE #define FRAME_CHAIN_VALID(chain, thisframe) gould_frame_chain_valid (chain, thisframe) */
/* OBSOLETE  */
/* OBSOLETE /* Define other aspects of the stack frame on NPL.  *x/ */
/* OBSOLETE #define FRAME_SAVED_PC(frame) \ */
/* OBSOLETE 	(read_memory_integer ((frame)->frame + 8, 4)) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_ADDRESS(fi) \ */
/* OBSOLETE 	((fi)->next ? \ */
/* OBSOLETE 	 read_memory_integer ((fi)->frame + 12, 4) : \ */
/* OBSOLETE 	 read_register (AP_REGNUM)) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_LOCALS_ADDRESS(fi)	((fi)->frame + 80) */
/* OBSOLETE  */
/* OBSOLETE /* Set VAL to the number of args passed to frame described by FI. */
/* OBSOLETE    Can set VAL to -1, meaning no way to tell.  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* We can check the stab info to see how */
/* OBSOLETE    many arg we have.  No info in stack will tell us *x/ */
/* OBSOLETE #define FRAME_NUM_ARGS(fi)		(findarg(fi)) */
/* OBSOLETE  */
/* OBSOLETE /* Return number of bytes at start of arglist that are not really args.  *x/ */
/* OBSOLETE #define FRAME_ARGS_SKIP			8 */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)		\ */
/* OBSOLETE {                                                                       \ */
/* OBSOLETE   memset (&frame_saved_regs, '\0', sizeof frame_saved_regs);			\ */
/* OBSOLETE   (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 8;		\ */
/* OBSOLETE   (frame_saved_regs).regs[R4_REGNUM] = (frame_info)->frame + 0x30;	\ */
/* OBSOLETE   (frame_saved_regs).regs[R5_REGNUM] = (frame_info)->frame + 0x34;	\ */
/* OBSOLETE   (frame_saved_regs).regs[R6_REGNUM] = (frame_info)->frame + 0x38;	\ */
/* OBSOLETE   (frame_saved_regs).regs[R7_REGNUM] = (frame_info)->frame + 0x3C;	\ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Things needed for making the inferior call functions.  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* Push an empty stack frame, to record the current PC, etc.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define PUSH_DUMMY_FRAME \ */
/* OBSOLETE { register CORE_ADDR sp = read_register (SP_REGNUM);			\ */
/* OBSOLETE   register int regnum;							\ */
/* OBSOLETE   sp = push_word (sp, read_register (PC_REGNUM));			\ */
/* OBSOLETE   sp = push_word (sp, read_register (FP_REGNUM));			\ */
/* OBSOLETE   write_register (FP_REGNUM, sp);					\ */
/* OBSOLETE   for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)			\ */
/* OBSOLETE     sp = push_word (sp, read_register (regnum));			\ */
/* OBSOLETE   sp = push_word (sp, read_register (PS_REGNUM));			\ */
/* OBSOLETE   write_register (SP_REGNUM, sp);  } */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame,  */
/* OBSOLETE    restoring all saved registers.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define POP_FRAME  \ */
/* OBSOLETE { register struct frame_info *frame = get_current_frame ();	 \ */
/* OBSOLETE   register CORE_ADDR fp;					 \ */
/* OBSOLETE   register int regnum;						 \ */
/* OBSOLETE   struct frame_saved_regs fsr;					 \ */
/* OBSOLETE   struct frame_info *fi;					 \ */
/* OBSOLETE   fp = frame->frame;						 \ */
/* OBSOLETE   get_frame_saved_regs (frame, &fsr);				 \ */
/* OBSOLETE   for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)		 \ */
/* OBSOLETE     if (fsr.regs[regnum])					 \ */
/* OBSOLETE       write_register (regnum, read_memory_integer (fsr.regs[regnum], 4)); \ */
/* OBSOLETE   if (fsr.regs[PS_REGNUM])					 \ */
/* OBSOLETE     write_register (PS_REGNUM, read_memory_integer (fsr.regs[PS_REGNUM], 4)); \ */
/* OBSOLETE   write_register (FP_REGNUM, read_memory_integer (fp, 4));	 \ */
/* OBSOLETE   write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));   \ */
/* OBSOLETE   write_register (SP_REGNUM, fp + 8);				 \ */
/* OBSOLETE   flush_cached_frames ();					 \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* This sequence of words is the instructions: */
/* OBSOLETE      halt */
/* OBSOLETE      halt */
/* OBSOLETE      halt */
/* OBSOLETE      halt */
/* OBSOLETE      suabr	b2, #<stacksize> */
/* OBSOLETE      lwbr	b6, #con */
/* OBSOLETE      stw	r1, 8(b2)	- save caller address, do we care? */
/* OBSOLETE      lw		r2, 60(b2)	- arg1 */
/* OBSOLETE      labr	b3, 50(b2) */
/* OBSOLETE      std	r4, 30(b2)	- save r4-r7 */
/* OBSOLETE      std	r6, 38(b2) */
/* OBSOLETE      lwbr	b1, #<func>	- load function call address */
/* OBSOLETE      brlnk	r1, 8(b1)	- call function */
/* OBSOLETE      halt */
/* OBSOLETE      halt */
/* OBSOLETE      ld		r4, 30(b2)	- restore r4-r7 */
/* OBSOLETE      ld		r6, 38(b2) */
/* OBSOLETE  */
/* OBSOLETE    Setup our stack frame, load argumemts, call and then restore registers. */
/* OBSOLETE *x/ */
/* OBSOLETE  */
/* OBSOLETE /* FIXME:  The below defines an m68k CALL_DUMMY, which looks nothing like what */
/* OBSOLETE    is documented above. *x/ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY {0xf227e0ff, 0x48e7fffc, 0x426742e7, 0x4eb93232, 0x3232dffc, 0x69696969, 0x4e4f4e71} */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_LENGTH 28 */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_START_OFFSET 12 */
/* OBSOLETE  */
/* OBSOLETE /* Insert the specified number of args and function address */
/* OBSOLETE    into a call sequence of the above form stored at DUMMYNAME.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)     \ */
/* OBSOLETE { *(int *)((char *) dummyname + 20) = nargs * 4;  \ */
/* OBSOLETE   *(int *)((char *) dummyname + 14) = fun; } */
