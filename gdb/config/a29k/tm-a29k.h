/* OBSOLETE /* Parameters for target machine AMD 29000, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1990, 1991, 1993, 1994, 1995, 1996, 1998, 1999, 2000, */
/* OBSOLETE    2001 Free Software Foundation, Inc. */
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
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* Parameters for an EB29K (a board which plugs into a PC and is */
/* OBSOLETE    accessed through EBMON software running on the PC, which we */
/* OBSOLETE    use as we'd use a remote stub (see remote-eb.c). */
/* OBSOLETE  */
/* OBSOLETE    If gdb is ported to other a29k machines/systems, the */
/* OBSOLETE    machine/system-specific parts should be removed from this file (a */
/* OBSOLETE    la tm-m68k.h).  */ */
/* OBSOLETE  */
/* OBSOLETE /* Byte order is configurable, but this machine runs big-endian.  */ */
/* OBSOLETE #define TARGET_BYTE_ORDER BFD_ENDIAN_BIG */
/* OBSOLETE  */
/* OBSOLETE /* Floating point uses IEEE representations.  */ */
/* OBSOLETE #define IEEE_FLOAT (1) */
/* OBSOLETE  */
/* OBSOLETE /* Recognize our magic number.  */ */
/* OBSOLETE #define BADMAG(x) ((x).f_magic != 0572) */
/* OBSOLETE  */
/* OBSOLETE /* Offset from address of function to start of its code. */
/* OBSOLETE    Zero on most machines.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FUNCTION_START_OFFSET 0 */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SKIP_PROLOGUE(pc) (a29k_skip_prologue (pc)) */
/* OBSOLETE CORE_ADDR a29k_skip_prologue (); */
/* OBSOLETE  */
/* OBSOLETE /* Immediately after a function call, return the saved pc. */
/* OBSOLETE    Can't go through the frames for this because on some machines */
/* OBSOLETE    the new frame is not set up until the new function executes */
/* OBSOLETE    some instructions.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SAVED_PC_AFTER_CALL(frame) ((frame->flags & TRANSPARENT_FRAME) \ */
/* OBSOLETE 				    ? read_register (TPC_REGNUM) \ */
/* OBSOLETE 				    : read_register (LR0_REGNUM)) */
/* OBSOLETE  */
/* OBSOLETE /* Stack grows downward.  */ */
/* OBSOLETE  */
/* OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs)) */
/* OBSOLETE  */
/* OBSOLETE /* Stack must be aligned on 32-bit boundaries when synthesizing */
/* OBSOLETE    function calls. */ */
/* OBSOLETE  */
/* OBSOLETE #define STACK_ALIGN(ADDR) (((ADDR) + 3) & ~3) */
/* OBSOLETE  */
/* OBSOLETE /* Sequence of bytes for breakpoint instruction.  */ */
/* OBSOLETE /* ASNEQ 0x50, gr1, gr1 */
/* OBSOLETE    The trap number 0x50 is chosen arbitrarily. */
/* OBSOLETE    We let the command line (or previously included files) override this */
/* OBSOLETE    setting.  */ */
/* OBSOLETE #ifndef BREAKPOINT */
/* OBSOLETE #if TARGET_BYTE_ORDER == BFD_ENDIAN_BIG */
/* OBSOLETE #define BREAKPOINT {0x72, 0x50, 0x01, 0x01} */
/* OBSOLETE #else /* Target is little-endian.  */ */
/* OBSOLETE #define BREAKPOINT {0x01, 0x01, 0x50, 0x72} */
/* OBSOLETE #endif /* Target is little-endian.  */ */
/* OBSOLETE #endif /* BREAKPOINT */ */
/* OBSOLETE  */
/* OBSOLETE /* Amount PC must be decremented by after a breakpoint. */
/* OBSOLETE    This is often the number of bytes in BREAKPOINT */
/* OBSOLETE    but not always.  */ */
/* OBSOLETE  */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 0 */
/* OBSOLETE  */
/* OBSOLETE /* Say how long (ordinary) registers are.  This is a piece of bogosity */
/* OBSOLETE    used in push_word and a few other places; REGISTER_RAW_SIZE is the */
/* OBSOLETE    real way to know how big a register is.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Allow the register declarations here to be overridden for remote */
/* OBSOLETE    kernel debugging.  */ */
/* OBSOLETE #if !defined (REGISTER_NAMES) */
/* OBSOLETE  */
/* OBSOLETE /* Number of machine registers */ */
/* OBSOLETE  */
/* OBSOLETE #define NUM_REGS 205 */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    There should be NUM_REGS strings in this initializer. */
/* OBSOLETE  */
/* OBSOLETE    FIXME, add floating point registers and support here. */
/* OBSOLETE  */
/* OBSOLETE    Also note that this list does not attempt to deal with kernel */
/* OBSOLETE    debugging (in which the first 32 registers are gr64-gr95).  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_NAMES \ */
/* OBSOLETE {"gr96", "gr97", "gr98", "gr99", "gr100", "gr101", "gr102", "gr103", "gr104", \ */
/* OBSOLETE  "gr105", "gr106", "gr107", "gr108", "gr109", "gr110", "gr111", "gr112", \ */
/* OBSOLETE  "gr113", "gr114", "gr115", "gr116", "gr117", "gr118", "gr119", "gr120", \ */
/* OBSOLETE  "gr121", "gr122", "gr123", "gr124", "gr125", "gr126", "gr127",		 \ */
/* OBSOLETE  "lr0", "lr1", "lr2", "lr3", "lr4", "lr5", "lr6", "lr7", "lr8", "lr9",   \ */
/* OBSOLETE  "lr10", "lr11", "lr12", "lr13", "lr14", "lr15", "lr16", "lr17", "lr18", \ */
/* OBSOLETE  "lr19", "lr20", "lr21", "lr22", "lr23", "lr24", "lr25", "lr26", "lr27", \ */
/* OBSOLETE  "lr28", "lr29", "lr30", "lr31", "lr32", "lr33", "lr34", "lr35", "lr36", \ */
/* OBSOLETE  "lr37", "lr38", "lr39", "lr40", "lr41", "lr42", "lr43", "lr44", "lr45", \ */
/* OBSOLETE  "lr46", "lr47", "lr48", "lr49", "lr50", "lr51", "lr52", "lr53", "lr54", \ */
/* OBSOLETE  "lr55", "lr56", "lr57", "lr58", "lr59", "lr60", "lr61", "lr62", "lr63", \ */
/* OBSOLETE  "lr64", "lr65", "lr66", "lr67", "lr68", "lr69", "lr70", "lr71", "lr72", \ */
/* OBSOLETE  "lr73", "lr74", "lr75", "lr76", "lr77", "lr78", "lr79", "lr80", "lr81", \ */
/* OBSOLETE  "lr82", "lr83", "lr84", "lr85", "lr86", "lr87", "lr88", "lr89", "lr90", \ */
/* OBSOLETE  "lr91", "lr92", "lr93", "lr94", "lr95", "lr96", "lr97", "lr98", "lr99", \ */
/* OBSOLETE  "lr100", "lr101", "lr102", "lr103", "lr104", "lr105", "lr106", "lr107", \ */
/* OBSOLETE  "lr108", "lr109", "lr110", "lr111", "lr112", "lr113", "lr114", "lr115", \ */
/* OBSOLETE  "lr116", "lr117", "lr118", "lr119", "lr120", "lr121", "lr122", "lr123", \ */
/* OBSOLETE  "lr124", "lr125", "lr126", "lr127",					 \ */
/* OBSOLETE   "AI0", "AI1", "AI2", "AI3", "AI4", "AI5", "AI6", "AI7", "AI8", "AI9",  \ */
/* OBSOLETE   "AI10", "AI11", "AI12", "AI13", "AI14", "AI15", "FP",			 \ */
/* OBSOLETE   "bp", "fc", "cr", "q",						 \ */
/* OBSOLETE   "vab", "ops", "cps", "cfg", "cha", "chd", "chc", "rbp", "tmc", "tmr",	 \ */
/* OBSOLETE   "pc0", "pc1", "pc2", "mmu", "lru", "fpe", "inte", "fps", "exo", "gr1",  \ */
/* OBSOLETE   "alu", "ipc", "ipa", "ipb" } */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Converts an sdb register number to an internal gdb register number. */
/* OBSOLETE  * Currently under epi, gr96->0...gr127->31...lr0->32...lr127->159, or... */
/* OBSOLETE  *                      gr64->0...gr95->31, lr0->32...lr127->159. */
/* OBSOLETE  */ */
/* OBSOLETE #define SDB_REG_TO_REGNUM(value) \ */
/* OBSOLETE   (((value) >= 96 && (value) <= 127) ? ((value) - 96) : \ */
/* OBSOLETE    ((value) >= 128 && (value) <=  255) ? ((value) - 128 + LR0_REGNUM) : \ */
/* OBSOLETE    (value)) */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Provide the processor register numbers of some registers that are */
/* OBSOLETE  * expected/written in instructions that might change under different */
/* OBSOLETE  * register sets.  Namely, gcc can compile (-mkernel-registers) so that */
/* OBSOLETE  * it uses gr64-gr95 in stead of gr96-gr127. */
/* OBSOLETE  */ */
/* OBSOLETE #define MSP_HW_REGNUM	125	/* gr125 */ */
/* OBSOLETE #define RAB_HW_REGNUM	126	/* gr126 */ */
/* OBSOLETE  */
/* OBSOLETE /* Convert Processor Special register #x to REGISTER_NAMES register # */ */
/* OBSOLETE #define SR_REGNUM(x) \ */
/* OBSOLETE   ((x) < 15  ? VAB_REGNUM + (x)					 \ */
/* OBSOLETE    : (x) >= 128 && (x) < 131 ? IPC_REGNUM + (x) - 128		 \ */
/* OBSOLETE    : (x) == 131 ? Q_REGNUM					 \ */
/* OBSOLETE    : (x) == 132 ? ALU_REGNUM					 \ */
/* OBSOLETE    : (x) >= 133 && (x) < 136 ? BP_REGNUM + (x) - 133		 \ */
/* OBSOLETE    : (x) >= 160 && (x) < 163 ? FPE_REGNUM + (x) - 160		 \ */
/* OBSOLETE    : (x) == 164 ? EXO_REGNUM                                     \ */
/* OBSOLETE    : (error ("Internal error in SR_REGNUM"), 0)) */
/* OBSOLETE #define GR96_REGNUM 0 */
/* OBSOLETE  */
/* OBSOLETE /* Define the return register separately, so it can be overridden for */
/* OBSOLETE    kernel procedure calling conventions. */ */
/* OBSOLETE #define	RETURN_REGNUM	GR96_REGNUM */
/* OBSOLETE #define GR1_REGNUM 200 */
/* OBSOLETE /* This needs to be the memory stack pointer, not the register stack pointer, */
/* OBSOLETE    to make call_function work right.  */ */
/* OBSOLETE #define SP_REGNUM MSP_REGNUM */
/* OBSOLETE #define FP_REGNUM 33		/* lr1 */ */
/* OBSOLETE  */
/* OBSOLETE /* Return register for transparent calling convention (gr122).  */ */
/* OBSOLETE #define TPC_REGNUM (122 - 96 + GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Large Return Pointer (gr123).  */ */
/* OBSOLETE #define LRP_REGNUM (123 - 96 + GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Static link pointer (gr124).  */ */
/* OBSOLETE #define SLP_REGNUM (124 - 96 + GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Memory Stack Pointer (gr125).  */ */
/* OBSOLETE #define MSP_REGNUM (125 - 96 + GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Register allocate bound (gr126).  */ */
/* OBSOLETE #define RAB_REGNUM (126 - 96 + GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Register Free Bound (gr127).  */ */
/* OBSOLETE #define RFB_REGNUM (127 - 96 + GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Register Stack Pointer.  */ */
/* OBSOLETE #define RSP_REGNUM GR1_REGNUM */
/* OBSOLETE #define LR0_REGNUM 32 */
/* OBSOLETE #define BP_REGNUM 177 */
/* OBSOLETE #define FC_REGNUM 178 */
/* OBSOLETE #define CR_REGNUM 179 */
/* OBSOLETE #define Q_REGNUM 180 */
/* OBSOLETE #define VAB_REGNUM 181 */
/* OBSOLETE #define OPS_REGNUM (VAB_REGNUM + 1) */
/* OBSOLETE #define CPS_REGNUM (VAB_REGNUM + 2) */
/* OBSOLETE #define CFG_REGNUM (VAB_REGNUM + 3) */
/* OBSOLETE #define CHA_REGNUM (VAB_REGNUM + 4) */
/* OBSOLETE #define CHD_REGNUM (VAB_REGNUM + 5) */
/* OBSOLETE #define CHC_REGNUM (VAB_REGNUM + 6) */
/* OBSOLETE #define RBP_REGNUM (VAB_REGNUM + 7) */
/* OBSOLETE #define TMC_REGNUM (VAB_REGNUM + 8) */
/* OBSOLETE #define TMR_REGNUM (VAB_REGNUM + 9) */
/* OBSOLETE #define NPC_REGNUM (VAB_REGNUM + 10)	/* pc0 */ */
/* OBSOLETE #define PC_REGNUM  (VAB_REGNUM + 11)	/* pc1 */ */
/* OBSOLETE #define PC2_REGNUM (VAB_REGNUM + 12) */
/* OBSOLETE #define MMU_REGNUM (VAB_REGNUM + 13) */
/* OBSOLETE #define LRU_REGNUM (VAB_REGNUM + 14) */
/* OBSOLETE #define FPE_REGNUM (VAB_REGNUM + 15) */
/* OBSOLETE #define INTE_REGNUM (VAB_REGNUM + 16) */
/* OBSOLETE #define FPS_REGNUM (VAB_REGNUM + 17) */
/* OBSOLETE #define EXO_REGNUM (VAB_REGNUM + 18) */
/* OBSOLETE /* gr1 is defined above as 200 = VAB_REGNUM + 19 */ */
/* OBSOLETE #define ALU_REGNUM (VAB_REGNUM + 20) */
/* OBSOLETE #define PS_REGNUM  ALU_REGNUM */
/* OBSOLETE #define IPC_REGNUM (VAB_REGNUM + 21) */
/* OBSOLETE #define IPA_REGNUM (VAB_REGNUM + 22) */
/* OBSOLETE #define IPB_REGNUM (VAB_REGNUM + 23) */
/* OBSOLETE  */
/* OBSOLETE #endif /* !defined(REGISTER_NAMES) */ */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  */ */
/* OBSOLETE #define REGISTER_BYTES (NUM_REGS * 4) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  */ */
/* OBSOLETE #define REGISTER_BYTE(N)  ((N)*4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the actual machine representation */
/* OBSOLETE    for register N.  */ */
/* OBSOLETE  */
/* OBSOLETE /* All regs are 4 bytes.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) (4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the program's representation */
/* OBSOLETE    for register N.  */ */
/* OBSOLETE  */
/* OBSOLETE /* All regs are 4 bytes.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) (4) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_RAW_SIZE (4) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE (4) */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \ */
/* OBSOLETE   (((N) == PC_REGNUM || (N) == LRP_REGNUM || (N) == SLP_REGNUM         \ */
/* OBSOLETE     || (N) == MSP_REGNUM || (N) == RAB_REGNUM || (N) == RFB_REGNUM     \ */
/* OBSOLETE     || (N) == GR1_REGNUM || (N) == FP_REGNUM || (N) == LR0_REGNUM       \ */
/* OBSOLETE     || (N) == NPC_REGNUM || (N) == PC2_REGNUM)                           \ */
/* OBSOLETE    ? lookup_pointer_type (builtin_type_void) : builtin_type_int) */
/* OBSOLETE  */
/* OBSOLETE /* Store the address of the place in which to copy the structure the */
/* OBSOLETE    subroutine will return.  This is called from call_function. */ */
/* OBSOLETE /* On the a29k the LRP points to the part of the structure beyond the first */
/* OBSOLETE    16 words.  */ */
/* OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) \ */
/* OBSOLETE   write_register (LRP_REGNUM, (ADDR) + 16 * 4); */
/* OBSOLETE  */
/* OBSOLETE /* Should call_function allocate stack space for a struct return?  */ */
/* OBSOLETE /* On the a29k objects over 16 words require the caller to allocate space.  */ */
/* OBSOLETE extern use_struct_convention_fn a29k_use_struct_convention; */
/* OBSOLETE #define USE_STRUCT_CONVENTION(gcc_p, type) a29k_use_struct_convention (gcc_p, type) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF.  */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF)	      \ */
/* OBSOLETE   {    	       	       	       	       	       	       	       	       	   \ */
/* OBSOLETE     int reg_length = TYPE_LENGTH (TYPE);				   \ */
/* OBSOLETE     if (reg_length > 16 * 4)						   \ */
/* OBSOLETE       {									   \ */
/* OBSOLETE 	reg_length = 16 * 4;						   \ */
/* OBSOLETE 	read_memory (*((int *)(REGBUF) + LRP_REGNUM), (VALBUF) + 16 * 4,   \ */
/* OBSOLETE 		     TYPE_LENGTH (TYPE) - 16 * 4);			   \ */
/* OBSOLETE       }									   \ */
/* OBSOLETE     memcpy ((VALBUF), ((int *)(REGBUF))+RETURN_REGNUM, reg_length);	   \ */
/* OBSOLETE   } */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE   {									  \ */
/* OBSOLETE     int reg_length = TYPE_LENGTH (TYPE);				  \ */
/* OBSOLETE     if (reg_length > 16 * 4)						  \ */
/* OBSOLETE       {									  \ */
/* OBSOLETE         reg_length = 16 * 4;						  \ */
/* OBSOLETE         write_memory (read_register (LRP_REGNUM),			  \ */
/* OBSOLETE 		      (char *)(VALBUF) + 16 * 4,			  \ */
/* OBSOLETE 		      TYPE_LENGTH (TYPE) - 16 * 4);			  \ */
/* OBSOLETE       }									  \ */
/* OBSOLETE     write_register_bytes (REGISTER_BYTE (RETURN_REGNUM), (char *)(VALBUF),  \ */
/* OBSOLETE 			  TYPE_LENGTH (TYPE));				  \ */
/* OBSOLETE   } */
/* OBSOLETE /* *INDENT-OFF* */ */
/* OBSOLETE /* The a29k user's guide documents well what the stacks look like. */
/* OBSOLETE    But what isn't so clear there is how this interracts with the */
/* OBSOLETE    symbols, or with GDB. */
/* OBSOLETE    In the following saved_msp, saved memory stack pointer (which functions */
/* OBSOLETE    as a memory frame pointer), means either */
/* OBSOLETE    a register containing the memory frame pointer or, in the case of */
/* OBSOLETE    functions with fixed size memory frames (i.e. those who don't use */
/* OBSOLETE    alloca()), the result of the calculation msp + msize. */
/* OBSOLETE  */
/* OBSOLETE    LOC_ARG, LOC_LOCAL - For GCC, these are relative to saved_msp. */
/* OBSOLETE    For high C, these are relative to msp (making alloca impossible). */
/* OBSOLETE    LOC_REGISTER, LOC_REGPARM - The register number is the number at the */
/* OBSOLETE    time the function is running (after the prologue), or in the case */
/* OBSOLETE    of LOC_REGPARM, may be a register number in the range 160-175. */
/* OBSOLETE  */
/* OBSOLETE    The compilers do things like store an argument into memory, and then put out */
/* OBSOLETE    a LOC_ARG for it, or put it into global registers and put out a */
/* OBSOLETE    LOC_REGPARM.  Thus is it important to execute the first line of */
/* OBSOLETE    code (i.e. the line of the open brace, i.e. the prologue) of a function */
/* OBSOLETE    before trying to print arguments or anything. */
/* OBSOLETE  */
/* OBSOLETE    The following diagram attempts to depict what is going on in memory */
/* OBSOLETE    (see also the _a29k user's guide_) and also how that interacts with */
/* OBSOLETE    GDB frames.  We arbitrarily pick fci->frame to point the same place */
/* OBSOLETE    as the register stack pointer; since we set it ourself in */
/* OBSOLETE    INIT_EXTRA_FRAME_INFO, and access it only through the FRAME_* */
/* OBSOLETE    macros, it doesn't really matter exactly how we */
/* OBSOLETE    do it.  However, note that FRAME_FP is used in two ways in GDB: */
/* OBSOLETE    (1) as a "magic cookie" which uniquely identifies frames (even over */
/* OBSOLETE    calls to the inferior), (2) (in PC_IN_CALL_DUMMY [ON_STACK]) */
/* OBSOLETE    as the value of SP_REGNUM before the dummy frame was pushed.  These */
/* OBSOLETE    two meanings would be incompatible for the a29k if we defined */
/* OBSOLETE    CALL_DUMMY_LOCATION == ON_STACK (but we don't, so don't worry about it). */
/* OBSOLETE    Also note that "lr1" below, while called a frame pointer */
/* OBSOLETE    in the user's guide, has only one function:  To determine whether */
/* OBSOLETE    registers need to be filled in the function epilogue. */
/* OBSOLETE  */
/* OBSOLETE    Consider the code: */
/* OBSOLETE    < call bar> */
/* OBSOLETE    loc1: . . . */
/* OBSOLETE    bar:  sub gr1,gr1,rsize_b */
/* OBSOLETE    . . . */
/* OBSOLETE    add mfp,msp,0 */
/* OBSOLETE    sub msp,msp,msize_b */
/* OBSOLETE    . . . */
/* OBSOLETE    < call foo > */
/* OBSOLETE    loc2: . . . */
/* OBSOLETE    foo:  sub gr1,gr1,rsize_f */
/* OBSOLETE    . . . */
/* OBSOLETE    add mfp,msp,0 */
/* OBSOLETE    sub msp,msp,msize_f */
/* OBSOLETE    . . . */
/* OBSOLETE    loc3: < suppose the inferior stops here > */
/* OBSOLETE  */
/* OBSOLETE    memory stack      register stack */
/* OBSOLETE    |           |     |____________| */
/* OBSOLETE    |           |     |____loc1____| */
/* OBSOLETE    +------->|___________|     |            |   ^ */
/* OBSOLETE    |        | ^         |     |  locals_b  |   | */
/* OBSOLETE    |        | |         |     |____________|   | */
/* OBSOLETE    |        | |         |     |            |   | rsize_b */
/* OBSOLETE    |        | | msize_b |     | args_to_f  |   | */
/* OBSOLETE    |        | |         |     |____________|   | */
/* OBSOLETE    |        | |         |     |____lr1_____|   V */
/* OBSOLETE    |        | V         |     |____loc2____|<----------------+ */
/* OBSOLETE    |   +--->|___________|<---------mfp     |   ^             | */
/* OBSOLETE    |   |    | ^         |     |  locals_f  |   |             | */
/* OBSOLETE    |   |    | | msize_f |     |____________|   |             | */
/* OBSOLETE    |   |    | |         |     |            |   | rsize_f     | */
/* OBSOLETE    |   |    | V         |     |   args     |   |             | */
/* OBSOLETE    |   |    |___________|<msp |____________|   |             | */
/* OBSOLETE    |   |                      |_____lr1____|   V             | */
/* OBSOLETE    |   |                      |___garbage__| <- gr1 <----+   | */
/* OBSOLETE    |   |                                                |   | */
/* OBSOLETE    |   |                                                |   | */
/* OBSOLETE    |   |                             pc=loc3                    |   | */
/* OBSOLETE    |   |                                                |   | */
/* OBSOLETE    |   |                                                |   | */
/* OBSOLETE    |   |            frame cache                         |   | */
/* OBSOLETE    |   |       |_________________|                      |   | */
/* OBSOLETE    |   |       |rsize=rsize_b    |                      |   | */
/* OBSOLETE    |   |       |msize=msize_b    |                      |   | */
/* OBSOLETE    +---|--------saved_msp        |                      |   | */
/* OBSOLETE    |       |frame------------------------------------|---+ */
/* OBSOLETE    |       |pc=loc2          |                       | */
/* OBSOLETE    |       |_________________|                       | */
/* OBSOLETE    |       |rsize=rsize_f    |                       | */
/* OBSOLETE    |       |msize=msize_f    |                       | */
/* OBSOLETE    +--------saved_msp        |                       | */
/* OBSOLETE    |frame------------------------------------+ */
/* OBSOLETE    |pc=loc3          | */
/* OBSOLETE    |_________________| */
/* OBSOLETE  */
/* OBSOLETE    So, is that sufficiently confusing?  Welcome to the 29000. */
/* OBSOLETE    Notes: */
/* OBSOLETE    * The frame for foo uses a memory frame pointer but the frame for */
/* OBSOLETE    bar does not.  In the latter case the saved_msp is */
/* OBSOLETE    computed by adding msize to the saved_msp of the */
/* OBSOLETE    next frame. */
/* OBSOLETE    * msize is in the frame cache only for high C's sake.  */ */
/* OBSOLETE /* *INDENT-ON* */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE void read_register_stack (); */
/* OBSOLETE long read_register_stack_integer (); */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_INIT_SAVED_REGS(fi)	/*no-op */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRA_FRAME_INFO  \ */
/* OBSOLETE   CORE_ADDR saved_msp;    \ */
/* OBSOLETE   unsigned int rsize;     \ */
/* OBSOLETE   unsigned int msize;	  \ */
/* OBSOLETE   unsigned char flags; */
/* OBSOLETE  */
/* OBSOLETE /* Bits for flags in EXTRA_FRAME_INFO */ */
/* OBSOLETE #define TRANSPARENT_FRAME	0x1	/* This is a transparent frame */ */
/* OBSOLETE #define MFP_USED		0x2	/* A memory frame pointer is used */ */
/* OBSOLETE  */
/* OBSOLETE /* Because INIT_FRAME_PC gets passed fromleaf, that's where we init */
/* OBSOLETE    not only ->pc and ->frame, but all the extra stuff, when called from */
/* OBSOLETE    get_prev_frame, that is.  */ */
/* OBSOLETE #define INIT_EXTRA_FRAME_INFO(fromleaf, fci)  init_extra_frame_info(fci) */
/* OBSOLETE void init_extra_frame_info (); */
/* OBSOLETE  */
/* OBSOLETE #define INIT_FRAME_PC(fromleaf, fci) init_frame_pc(fromleaf, fci) */
/* OBSOLETE void init_frame_pc (); */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* FRAME_CHAIN takes a FRAME */
/* OBSOLETE    and produces the frame's chain-pointer. */
/* OBSOLETE  */
/* OBSOLETE    However, if FRAME_CHAIN_VALID returns zero, */
/* OBSOLETE    it means the given frame is the outermost one and has no caller.  */ */
/* OBSOLETE  */
/* OBSOLETE /* On the a29k, the nominal address of a frame is the address on the */
/* OBSOLETE    register stack of the return address (the one next to the incoming */
/* OBSOLETE    arguments, not down at the bottom so nominal address == stack pointer). */
/* OBSOLETE  */
/* OBSOLETE    GDB expects "nominal address" to equal contents of FP_REGNUM, */
/* OBSOLETE    at least when it comes time to create the innermost frame. */
/* OBSOLETE    However, that doesn't work for us, so when creating the innermost */
/* OBSOLETE    frame we set ->frame ourselves in INIT_EXTRA_FRAME_INFO.  */ */
/* OBSOLETE  */
/* OBSOLETE /* These are mostly dummies for the a29k because INIT_FRAME_PC */
/* OBSOLETE    sets prev->frame instead.  */ */
/* OBSOLETE /* If rsize is zero, we must be at end of stack (or otherwise hosed). */
/* OBSOLETE    If we don't check rsize, we loop forever if we see rsize == 0.  */ */
/* OBSOLETE #define FRAME_CHAIN(thisframe) \ */
/* OBSOLETE   ((thisframe)->rsize == 0 \ */
/* OBSOLETE    ? 0 \ */
/* OBSOLETE    : (thisframe)->frame + (thisframe)->rsize) */
/* OBSOLETE  */
/* OBSOLETE /* Determine if the frame has a 'previous' and back-traceable frame. */ */
/* OBSOLETE #define FRAME_IS_UNCHAINED(frame)	((frame)->flags & TRANSPARENT_FRAME) */
/* OBSOLETE  */
/* OBSOLETE /* Find the previous frame of a transparent routine. */
/* OBSOLETE  * For now lets not try and trace through a transparent routine (we might  */
/* OBSOLETE  * have to assume that all transparent routines are traps). */
/* OBSOLETE  */ */
/* OBSOLETE #define FIND_PREV_UNCHAINED_FRAME(frame)	0 */
/* OBSOLETE  */
/* OBSOLETE /* Define other aspects of the stack frame.  */ */
/* OBSOLETE  */
/* OBSOLETE /* An expression that tells us whether the function invocation represented */
/* OBSOLETE    by FI does not have a frame on the stack associated with it. */ */
/* OBSOLETE #define FRAMELESS_FUNCTION_INVOCATION(FI) \ */
/* OBSOLETE   (frameless_look_for_prologue (FI)) */
/* OBSOLETE  */
/* OBSOLETE /* Saved pc (i.e. return address).  */ */
/* OBSOLETE #define FRAME_SAVED_PC(fraim) \ */
/* OBSOLETE   (read_register_stack_integer ((fraim)->frame + (fraim)->rsize, 4)) */
/* OBSOLETE  */
/* OBSOLETE /* Local variables (i.e. LOC_LOCAL) are on the memory stack, with their */
/* OBSOLETE    offsets being relative to the memory stack pointer (high C) or */
/* OBSOLETE    saved_msp (gcc).  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) frame_locals_address (fi) */
/* OBSOLETE extern CORE_ADDR frame_locals_address (); */
/* OBSOLETE  */
/* OBSOLETE /* Return number of args passed to a frame. */
/* OBSOLETE    Can return -1, meaning no way to tell.  */ */
/* OBSOLETE /* We tried going to the effort of finding the tags word and getting */
/* OBSOLETE    the argcount field from it, to support debugging assembler code. */
/* OBSOLETE    Problem was, the "argcount" field never did hold the argument */
/* OBSOLETE    count.  */ */
/* OBSOLETE #define	FRAME_NUM_ARGS(fi) (-1) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_ADDRESS(fi) FRAME_LOCALS_ADDRESS (fi) */
/* OBSOLETE  */
/* OBSOLETE /* Return number of bytes at start of arglist that are not really args.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_SKIP 0 */
/* OBSOLETE  */
/* OBSOLETE /* Provide our own get_saved_register.  HAVE_REGISTER_WINDOWS is insufficient */
/* OBSOLETE    because registers get renumbered on the a29k without getting saved.  */ */
/* OBSOLETE  */
/* OBSOLETE struct frame_info; */
/* OBSOLETE void a29k_get_saved_register (char *raw_buffer, int *optimized, */
/* OBSOLETE 			      CORE_ADDR * addrp, struct frame_info *frame, */
/* OBSOLETE 			      int regnum, enum lval_type *lvalp); */
/* OBSOLETE #define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) \ */
/* OBSOLETE       a29k_get_saved_register (raw_buffer, optimized, addrp, frame, regnum, lval) */
/* OBSOLETE  */
/* OBSOLETE /* Call function stuff.  */ */
/* OBSOLETE /* *INDENT-OFF* */ */
/* OBSOLETE /* The dummy frame looks like this (see also the general frame picture */
/* OBSOLETE    above): */
/* OBSOLETE  */
/* OBSOLETE 					register stack */
/* OBSOLETE  */
/* OBSOLETE 	                	      |                |  frame for function */
/* OBSOLETE                	                      |   locals_sproc |  executing at time */
/* OBSOLETE                                       |________________|  of call_function. */
/* OBSOLETE                      		      |	               |  We must not disturb */
/* OBSOLETE                      		      |	args_out_sproc |  it. */
/* OBSOLETE         memory stack 		      |________________| */
/* OBSOLETE                      		      |____lr1_sproc___|<-+ */
/* OBSOLETE        |            |		      |__retaddr_sproc_|  | <-- gr1 (at start) */
/* OBSOLETE        |____________|<-msp 0 <-----------mfp_dummy_____|  | */
/* OBSOLETE        |            |  (at start)     |  save regs     |  | */
/* OBSOLETE        | arg_slop   |		      |  pc0,pc1       |  | */
/* OBSOLETE        |            |		      |  pc2,lr0 sproc |  | */
/* OBSOLETE        | (16 words) |		      | gr96-gr124     |  | */
/* OBSOLETE        |____________|<-msp 1--after   | sr160-sr162    |  | */
/* OBSOLETE        |            | PUSH_DUMMY_FRAME| sr128-sr135    |  | */
/* OBSOLETE        | struct ret |                 |________________|  | */
/* OBSOLETE        | 17+        |                 |                |  |  */
/* OBSOLETE        |____________|<- lrp           | args_out_dummy |  | */
/* OBSOLETE        | struct ret |		      |  (16 words)    |  | */
/* OBSOLETE        | 16         |		      |________________|  | */
/* OBSOLETE        | (16 words) |                 |____lr1_dummy___|--+ */
/* OBSOLETE        |____________|<- msp 2--after  |_retaddr_dummy__|<- gr1 after */
/* OBSOLETE        |            | struct ret      |                |   PUSH_DUMMY_FRAME */
/* OBSOLETE        | margs17+   | area allocated  |  locals_inf    | */
/* OBSOLETE        |            |                 |________________|    called */
/* OBSOLETE        |____________|<- msp 4--when   |                |    function's */
/* OBSOLETE        |            |   inf called    | args_out_inf   |    frame (set up */
/* OBSOLETE        | margs16    |                 |________________|    by called */
/* OBSOLETE        | (16 words) |                 |_____lr1_inf____|    function). */
/* OBSOLETE        |____________|<- msp 3--after  |       .        | */
/* OBSOLETE        |            |   args pushed   |       .        | */
/* OBSOLETE        |            |	              |       .        | */
/* OBSOLETE                                       |                | */
/* OBSOLETE  */
/* OBSOLETE    arg_slop: This area is so that when the call dummy adds 16 words to */
/* OBSOLETE       the msp, it won't end up larger than mfp_dummy (it is needed in the */
/* OBSOLETE       case where margs and struct_ret do not add up to at least 16 words). */
/* OBSOLETE    struct ret:  This area is allocated by GDB if the return value is more */
/* OBSOLETE       than 16 words.  struct ret_16 is not used on the a29k. */
/* OBSOLETE    margs:  Pushed by GDB.  The call dummy copies the first 16 words to */
/* OBSOLETE       args_out_dummy. */
/* OBSOLETE    retaddr_sproc:  Contains the PC at the time we call the function. */
/* OBSOLETE       set by PUSH_DUMMY_FRAME and read by POP_FRAME. */
/* OBSOLETE    retaddr_dummy:  This points to a breakpoint instruction in the dummy.  */ */
/* OBSOLETE /* *INDENT-ON* */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Rsize for dummy frame, in bytes.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Bytes for outgoing args, lr1, and retaddr.  */ */
/* OBSOLETE #define DUMMY_ARG (2 * 4 + 16 * 4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of special registers (sr128-) to save.  */ */
/* OBSOLETE #define DUMMY_SAVE_SR128 8 */
/* OBSOLETE /* Number of special registers (sr160-) to save.  */ */
/* OBSOLETE #define DUMMY_SAVE_SR160 3 */
/* OBSOLETE /* Number of general (gr96- or gr64-) registers to save.  */ */
/* OBSOLETE #define DUMMY_SAVE_GREGS 29 */
/* OBSOLETE  */
/* OBSOLETE #define DUMMY_FRAME_RSIZE \ */
/* OBSOLETE (4 /* mfp_dummy */     	  \ */
/* OBSOLETE  + 4 * 4  /* pc0, pc1, pc2, lr0 */  \ */
/* OBSOLETE  + DUMMY_SAVE_GREGS * 4   \ */
/* OBSOLETE  + DUMMY_SAVE_SR160 * 4	  \ */
/* OBSOLETE  + DUMMY_SAVE_SR128 * 4	  \ */
/* OBSOLETE  + DUMMY_ARG		  \ */
/* OBSOLETE  + 4 /* pad to doubleword */ ) */
/* OBSOLETE  */
/* OBSOLETE /* Push an empty stack frame, to record the current PC, etc.  */ */
/* OBSOLETE  */
/* OBSOLETE #define PUSH_DUMMY_FRAME push_dummy_frame() */
/* OBSOLETE extern void push_dummy_frame (); */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame, */
/* OBSOLETE    restoring all saved registers.  */ */
/* OBSOLETE  */
/* OBSOLETE #define POP_FRAME pop_frame() */
/* OBSOLETE extern void pop_frame (); */
/* OBSOLETE  */
/* OBSOLETE /* This sequence of words is the instructions */
/* OBSOLETE    mtsrim cr, 15 */
/* OBSOLETE    loadm 0, 0, lr2, msp     ; load first 16 words of arguments into registers */
/* OBSOLETE    add msp, msp, 16 * 4     ; point to the remaining arguments */
/* OBSOLETE    CONST_INSN: */
/* OBSOLETE    const lr0,inf                ; (replaced by       half of target addr) */
/* OBSOLETE    consth lr0,inf               ; (replaced by other half of target addr) */
/* OBSOLETE    calli lr0, lr0  */
/* OBSOLETE    aseq 0x40,gr1,gr1   ; nop */
/* OBSOLETE    BREAKPT_INSN: */
/* OBSOLETE    asneq 0x50,gr1,gr1  ; breakpoint     (replaced by local breakpoint insn) */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE #error "This file is broken.  GDB does not define HOST_BYTE_ORDER." */
/* OBSOLETE #if TARGET_BYTE_ORDER == HOST_BYTE_ORDER */
/* OBSOLETE #define BS(const)	const */
/* OBSOLETE #else */
/* OBSOLETE #define	BS(const)	(((const) & 0xff) << 24) |	\ */
/* OBSOLETE 			(((const) & 0xff00) << 8) |	\ */
/* OBSOLETE 			(((const) & 0xff0000) >> 8) |	\ */
/* OBSOLETE 			(((const) & 0xff000000) >> 24) */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Position of the "const" and blkt instructions within CALL_DUMMY in bytes. */ */
/* OBSOLETE #define CONST_INSN (3 * 4) */
/* OBSOLETE #define BREAKPT_INSN (7 * 4) */
/* OBSOLETE #define CALL_DUMMY {	\ */
/* OBSOLETE 		BS(0x0400870f),\ */
/* OBSOLETE 		BS(0x36008200|(MSP_HW_REGNUM)), \ */
/* OBSOLETE 		BS(0x15000040|(MSP_HW_REGNUM<<8)|(MSP_HW_REGNUM<<16)), \ */
/* OBSOLETE 		BS(0x03ff80ff),	\ */
/* OBSOLETE 		BS(0x02ff80ff),	\ */
/* OBSOLETE 		BS(0xc8008080),	\ */
/* OBSOLETE 		BS(0x70400101),	\ */
/* OBSOLETE 		BS(0x72500101)} */
/* OBSOLETE #define CALL_DUMMY_LENGTH (8 * 4) */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_START_OFFSET 0	/* Start execution at beginning of dummy */ */
/* OBSOLETE  */
/* OBSOLETE /* Helper macro for FIX_CALL_DUMMY.  WORDP is a long * which points to a */
/* OBSOLETE    word in target byte order; bits 0-7 and 16-23 of *WORDP are replaced with */
/* OBSOLETE    bits 0-7 and 8-15 of DATA (which is in host byte order).  */ */
/* OBSOLETE  */
/* OBSOLETE #if TARGET_BYTE_ORDER == BFD_ENDIAN_BIG */
/* OBSOLETE #define STUFF_I16(WORDP, DATA) \ */
/* OBSOLETE   { \ */
/* OBSOLETE     *((char *)(WORDP) + 3) = ((DATA) & 0xff);\ */
/* OBSOLETE     *((char *)(WORDP) + 1) = (((DATA) >> 8) & 0xff);\ */
/* OBSOLETE   } */
/* OBSOLETE #else /* Target is little endian.  */ */
/* OBSOLETE #define STUFF_I16(WORDP, DATA) \ */
/* OBSOLETE   { */
/* OBSOLETE *(char *) (WORDP) = ((DATA) & 0xff); */
/* OBSOLETE *((char *) (WORDP) + 2) = (((DATA) >> 8) & 0xff); */
/* OBSOLETE } */
/* OBSOLETE #endif /* Target is little endian.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Insert the specified number of args and function address */
/* OBSOLETE    into a call sequence of the above form stored at DUMMYNAME.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Currently this stuffs in the address of the function that we are calling. */
/* OBSOLETE    Since different a29k systems use different breakpoint instructions, it */
/* OBSOLETE    also stuffs BREAKPOINT in the right place (to avoid having to */
/* OBSOLETE    duplicate CALL_DUMMY in each tm-*.h file).  */ */
/* OBSOLETE  */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   \ */
/* OBSOLETE   {\ */
/* OBSOLETE     STUFF_I16((char *)dummyname + CONST_INSN, fun);		\ */
/* OBSOLETE     STUFF_I16((char *)dummyname + CONST_INSN + 4, fun >> 16);	\ */
/* OBSOLETE   /* FIXME  memcpy ((char *)(dummyname) + BREAKPT_INSN, break_insn, 4); */ \ */
/* OBSOLETE   } */
/* OBSOLETE  */
/* OBSOLETE /* a29k architecture has separate data & instruction memories -- wired to */
/* OBSOLETE    different pins on the chip -- and can't execute the data memory. */
/* OBSOLETE    Also, there should be space after text_end; */
/* OBSOLETE    we won't get a SIGSEGV or scribble on data space.  */ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_LOCATION AFTER_TEXT_END */
/* OBSOLETE  */
/* OBSOLETE /* Because of this, we need (as a kludge) to know the addresses of the */
/* OBSOLETE    text section.  */ */
/* OBSOLETE  */
/* OBSOLETE #define	NEED_TEXT_START_END 1 */
/* OBSOLETE  */
/* OBSOLETE /* How to translate register numbers in the .stab's into gdb's internal register */
/* OBSOLETE    numbers.  We don't translate them, but we warn if an invalid register */
/* OBSOLETE    number is seen.  Note that FIXME, we use the value "sym" as an implicit */
/* OBSOLETE    argument in printing the error message.  It happens to be available where */
/* OBSOLETE    this macro is used.  (This macro definition appeared in a late revision */
/* OBSOLETE    of gdb-3.91.6 and is not well tested.  Also, it should be a "complaint".) */ */
/* OBSOLETE  */
/* OBSOLETE #define	STAB_REG_TO_REGNUM(num) \ */
/* OBSOLETE 	(((num) > LR0_REGNUM + 127) \ */
/* OBSOLETE 	   ? fprintf(stderr, 	\ */
/* OBSOLETE 		"Invalid register number %d in symbol table entry for %s\n", \ */
/* OBSOLETE 	         (num), SYMBOL_SOURCE_NAME (sym)), (num)	\ */
/* OBSOLETE 	   : (num)) */
/* OBSOLETE  */
/* OBSOLETE extern enum a29k_processor_types */
/* OBSOLETE { */
/* OBSOLETE   a29k_unknown, */
/* OBSOLETE  */
/* OBSOLETE   /* Bit 0x400 of the CPS does *not* identify freeze mode, i.e. 29000, */
/* OBSOLETE      29030, etc.  */ */
/* OBSOLETE   a29k_no_freeze_mode, */
/* OBSOLETE  */
/* OBSOLETE   /* Bit 0x400 of the CPS does identify freeze mode, i.e. 29050.  */ */
/* OBSOLETE   a29k_freeze_mode */
/* OBSOLETE } */
/* OBSOLETE processor_type; */
/* OBSOLETE  */
/* OBSOLETE /* We need three arguments for a general frame specification for the */
/* OBSOLETE    "frame" or "info frame" command.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SETUP_ARBITRARY_FRAME(argc, argv) setup_arbitrary_frame (argc, argv) */
/* OBSOLETE extern struct frame_info *setup_arbitrary_frame (int, CORE_ADDR *); */
