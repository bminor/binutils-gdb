From uwvax!sequent!ogcvax!reed!keith@RUTGERS.EDU Thu Jul 23 21:46:44 1987
Received: by PREP.AI.MIT.EDU; Thu, 23 Jul 87 21:44:35 EDT
Received: by RUTGERS.EDU (5.54/1.14) with UUCP 
	id AA04584; Thu, 23 Jul 87 21:42:33 EDT
Received: from sequent.UUCP by spool.WISC.EDU; Thu, 23 Jul 87 20:36:20 CDT
Received: from reed.UUCP by ogcvax.OGC.EDU (5.51/OGC_4.6+)
		id AA05332; Thu, 23 Jul 87 13:31:52 PDT
Received: by reed.UUCP (5.51/5.17)
	id AA23265; Thu, 23 Jul 87 11:19:20 PDT
From: uwvax!sequent!ogcvax!reed!keith@RUTGERS.EDU (Keith Packard)
Message-Id: <8707231819.AA23265@reed.UUCP>
To: phr@prep.ai.mit.edu (Paul Rubin)
Subject: Re: gdb 
In-Reply-To: Your message of Thu, 23 Jul 87 02:06:52 EDT.
             <8707230603.AA11722@EDDIE.MIT.EDU> 
Date: Thu, 23 Jul 87 11:19:13 PDT
Status: R


Thanks much for the address -- the 2.1 sources that I have do not contain
any bug reporting address.  The only real bug that I found was in
write_register_bytes in findvar.c:

was:

  bcopy (myaddr, &registers[regbyte], len);
  if (have_inferior_p ())
    store_inferior_registers (0);

should be:

  bcopy (myaddr, &registers[regbyte], len);
  if (have_inferior_p ())
    store_inferior_registers (-1);

Other than that, most of the porting effort to the 32k was in removing
references to alloca - the 32k is adamant about not using alloca -- in fact
someone at tektronix wrote a replacement which accepted another argument
pointing to the function entry instruction so that the stack could be maimed
mercilessly...  I just replaced them all with malloc and used free at
judicious times.  It's not perfect but it worked fine.

I would upload gdb 2.3 if I could, however I am not on the arpa net.  I'll
probably end up sending GNU a tape.

It's a great debugger, thanks!

	keith packard
	tektronix!reed!keith

Here are the param files and instruction printer for the 32032:

#!/bin/sh
# shar:	Shell Archiver
#	Run the following text with /bin/sh to create:
#	m-merlin.h
#	n32k-opcode.h
#	n32k-pinsn.c
sed 's/^X//' << 'SHAR_EOF' > m-merlin.h
X/* Definitions to make GDB run on a merlin under utek 2.1
X   Copyright (C) 1986, 1987 Free Software Foundation, Inc.
X
XGDB is distributed in the hope that it will be useful, but WITHOUT ANY
XWARRANTY.  No author or distributor accepts responsibility to anyone
Xfor the consequences of using it or for whether it serves any
Xparticular purpose or works at all, unless he says so in writing.
XRefer to the GDB General Public License for full details.
X
XEveryone is granted permission to copy, modify and redistribute GDB,
Xbut only under the conditions described in the GDB General Public
XLicense.  A copy of this license is supposed to have been given to you
Xalong with GDB so you can know your rights and responsibilities.  It
Xshould be in a file named COPYING.  Among other things, the copyright
Xnotice and this notice must be preserved on all copies.
X
XIn other words, go ahead and share GDB, but don't try to stop
Xanyone else from sharing it farther.  Help stamp out software hoarding!
X*/
X
X#ifndef ns16000
X#define ns16000
X#endif
X
X# include	<machine/reg.h>
X
X/* Define this if the C compiler puts an underscore at the front
X   of external names before giving them to the linker.  */
X
X#define NAMES_HAVE_UNDERSCORE
X
X/* Offset from address of function to start of its code.
X   Zero on most machines.  */
X
X#define FUNCTION_START_OFFSET 0
X
X/* Advance PC across any function entry prologue instructions
X   to reach some "real" code.  */
X
X#define SKIP_PROLOGUE(pc)   				\
X{ register int op = read_memory_integer (pc, 1);	\
X  if (op == 0x82) { op = read_memory_integer (pc+2,1);  \
X  		    if ((op & 0x80) == 0) pc += 3;	\
X		    else if ((op & 0xc0) == 0x80) pc += 4;	\
X		    else pc += 6;			\
X		   }					\
X}
X
X/* Immediately after a function call, return the saved pc.
X   Can't always go through the frames for this because on some machines
X   the new frame is not set up until the new function executes
X   some instructions.  */
X
X#define SAVED_PC_AFTER_CALL(frame) \
X	read_memory_integer (read_register (SP_REGNUM), 4)
X
X/* This is the amount to subtract from u.u_ar0
X   to get the offset in the core file of the register values.  */
X
X#define KERNEL_U_ADDR (0xfef000)
X
X/* Address of end of stack space.  */
X
X#define STACK_END_ADDR (0x800000)
X
X/* Stack grows downward.  */
X
X#define INNER_THAN <
X
X/* Sequence of bytes for breakpoint instruction.  */
X
X#define BREAKPOINT {0xf2}
X
X/* Amount PC must be decremented by after a breakpoint.
X   This is often the number of bytes in BREAKPOINT
X   but not always.  */
X
X#define DECR_PC_AFTER_BREAK 0
X
X/* Nonzero if instruction at PC is a return instruction.  */
X
X#define ABOUT_TO_RETURN(pc) (read_memory_integer (pc, 1) == 0x12)
X
X/* Return 1 if P points to an invalid floating point value.  */
X
X#define INVALID_FLOAT(p) (0)
X
X/* Say how long (ordinary) registers are.  */
X
X#define REGISTER_TYPE long
X
X/* Number of machine registers */
X
X#define NUM_REGS		25
X
X#define NUM_GENERAL_REGS	8
X
X/* Initializer for an array of names of registers.
X   There should be NUM_REGS strings in this initializer.  */
X
X#define REGISTER_NAMES {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",	\
X			"pc", "sp", "fp", "ps",				\
X 			"fsr",						\
X 			"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",	\
X			"l0", "l1", "l2", "l3", "l4",			\
X 			}
X
X/* Register numbers of various important registers.
X   Note that some of these values are "real" register numbers,
X   and correspond to the general registers of the machine,
X   and some are "phony" register numbers which are too large
X   to be actual register numbers as far as the user is concerned
X   but do serve to get the desired values when passed to read_register.  */
X
X#define AP_REGNUM FP_REGNUM
X#define FP_REGNUM 10		/* Contains address of executing stack frame */
X#define SP_REGNUM 9		/* Contains address of top of stack */
X#define PC_REGNUM 8		/* Contains program counter */
X#define PS_REGNUM 11		/* Contains processor status */
X#define FPS_REGNUM 12		/* Floating point status register */
X#define FP0_REGNUM 13		/* Floating point register 0 */
X#define LP0_REGNUM 21		/* Double register 0 (same as FP0) */
X
X#define REGISTER_U_ADDR(addr, blockend, regno) \
X{ 									\
X  switch (regno) {							\
X  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:	\
X	addr = blockend + (R0 - regno) * sizeof (int); break;		\
X  case PC_REGNUM:							\
X  	addr = blockend + PC * sizeof (int); break;			\
X  case SP_REGNUM:							\
X  	addr = blockend + SP * sizeof (int); break;			\
X  case FP_REGNUM:							\
X  	addr = blockend + FP * sizeof (int); break;			\
X  case PS_REGNUM:							\
X  	addr = blockend + 12 * sizeof (int); break;			\
X  case FPS_REGNUM:							\
X  	addr = 108; break;						\
X  case FP0_REGNUM + 0: case FP0_REGNUM + 1:				\
X  case FP0_REGNUM + 2: case FP0_REGNUM + 3: 				\
X  case FP0_REGNUM + 4: case FP0_REGNUM + 5: 				\
X  case FP0_REGNUM + 6: case FP0_REGNUM + 7: 				\
X  	addr = 76 + (regno - FP0_REGNUM) * sizeof (float); break;	\
X  case LP0_REGNUM + 0: case LP0_REGNUM + 1:				\
X  case LP0_REGNUM + 2: case LP0_REGNUM + 3:				\
X  	addr = 76 + (regno - LP0_REGNUM) * sizeof (double); break;	\
X  default:								\
X  	printf ("bad argument to REGISTER_U_ADDR %d\n", regno);		\
X	abort ();							\
X  }									\
X}
X
X/* Total amount of space needed to store our copies of the machine's
X   register state, the array `registers'.  */
X#define REGISTER_BYTES ((NUM_REGS - 4) * sizeof (int) + 4 * sizeof (double))
X
X/* Index within `registers' of the first byte of the space for
X   register N.  */
X
X#define REGISTER_BYTE(N) ((N) >= LP0_REGNUM ? \
X	LP0_REGNUM * 4 + ((N) - LP0_REGNUM) * 8 : (N) * 4)
X
X/* Number of bytes of storage in the actual machine representation
X   for register N.  On the 32000, all regs are 4 bytes
X   except for the doubled floating registers. */
X
X#define REGISTER_RAW_SIZE(N) ((N) >= LP0_REGNUM ? 8 : 4)
X
X/* Number of bytes of storage in the program's representation
X   for register N.  On the 32000, all regs are 4 bytes
X   except for the doubled floating registers. */
X
X#define REGISTER_VIRTUAL_SIZE(N) ((N) >= LP0_REGNUM ? 8 : 4)
X
X/* Largest value REGISTER_RAW_SIZE can have.  */
X
X#define MAX_REGISTER_RAW_SIZE 8
X
X/* Largest value REGISTER_VIRTUAL_SIZE can have.  */
X
X#define MAX_REGISTER_VIRTUAL_SIZE 8
X
X/* Nonzero if register N requires conversion
X   from raw format to virtual format.  */
X
X#define REGISTER_CONVERTIBLE(N) 0
X
X/* Convert data from raw format for register REGNUM
X   to virtual format for register REGNUM.  */
X
X#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)	\
X  bcopy ((FROM), (TO), REGISTER_VIRTUAL_SIZE(REGNUM));
X
X/* Convert data from virtual format for register REGNUM
X   to raw format for register REGNUM.  */
X
X#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
X  bcopy ((FROM), (TO), REGISTER_VIRTUAL_SIZE(REGNUM));
X
X/* Return the GDB type object for the "standard" data type
X   of data in register N.  */
X
X#define REGISTER_VIRTUAL_TYPE(N) \
X	((N) >= FP0_REGNUM ?			\
X 		(N) >= LP0_REGNUM ?		\
X 			builtin_type_double	\
X		 	: builtin_type_float	\
X 		: builtin_type_int)	
X
X/* Describe the pointer in each stack frame to the previous stack frame
X   (its caller).  */
X
X/* FRAME_CHAIN takes a frame's nominal address
X   and produces the frame's chain-pointer.
X
X   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
X   and produces the nominal address of the caller frame.
X
X   However, if FRAME_CHAIN_VALID returns zero,
X   it means the given frame is the outermost one and has no caller.
X   In that case, FRAME_CHAIN_COMBINE is not used.  */
X
X/* In the case of the Merlin, the frame's nominal address is the FP value,
X   and at that address is saved previous FP value as a 4-byte word.  */
X
X#define FRAME_CHAIN(thisframe)  (read_memory_integer (thisframe, 4))
X
X#define FRAME_CHAIN_VALID(chain, thisframe) \
X  (chain != 0 && (FRAME_SAVED_PC (thisframe) >= first_object_file_end))
X
X#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)
X
X/* Define other aspects of the stack frame.  */
X
X#define FRAME_SAVED_PC(frame) (read_memory_integer (frame + 4, 4))
X
X/* compute base of arguments */
X#define FRAME_ARGS_ADDRESS(fi) ((fi).frame)
X
X#define FRAME_LOCALS_ADDRESS(fi) ((fi).frame)
X
X/* Return number of args passed to a frame.
X   Can return -1, meaning no way to tell.  */
X
X#define FRAME_NUM_ARGS(numargs, fi)				\
X{	CORE_ADDR pc;						\
X	int insn;						\
X	int addr_mode;						\
X	int width;						\
X								\
X	pc = FRAME_SAVED_PC (fi.frame);				\
X 	insn = read_memory_integer (pc,2);			\
X	addr_mode = (insn >> 11) & 0x1f;			\
X	insn = insn & 0x7ff;					\
X	if ((insn & 0x7fc) == 0x57c &&				\
X	    addr_mode == 0x14) { /* immediate */		\
X		if (insn == 0x57c) /* adjspb */			\
X			width = 1;				\
X		else if (insn == 0x57d) /* adjspw */		\
X			width = 2;				\
X		else if (insn == 0x57f) /* adjspd */		\
X			width = 4;				\
X		numargs = read_memory_integer (pc+2,width);	\
X		if (width > 1)					\
X			flip_bytes (&numargs, width);		\
X		numargs = - sign_extend (numargs, width*8) / 4;	\
X	} else {						\
X	 	numargs = -1;					\
X	}							\
X}
X
X/* Return number of bytes at start of arglist that are not really args.  */
X
X#define FRAME_ARGS_SKIP 8
X
X/* Put here the code to store, into a struct frame_saved_regs,
X   the addresses of the saved registers of frame described by FRAME_INFO.
X   This includes special registers such as pc and fp saved in special
X   ways in the stack frame.  sp is even more special:
X   the address we return for it IS the sp for the next frame.  */
X
X#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs) \
X{ 								\
X	int	regmask,regnum;					\
X	int	localcount;					\
X	CORE_ADDR	enter_addr;				\
X	CORE_ADDR	next_addr;				\
X								\
X	enter_addr = get_pc_function_start ((frame_info).pc);	\
X	regmask = read_memory_integer (enter_addr+1, 1);	\
X	localcount = n32k_localcount (enter_addr);		\
X	next_addr = (frame_info).frame + localcount;		\
X	for (regnum = 0; regnum < 8; regnum++, regmask >>= 1)	\
X		(frame_saved_regs).regs[regnum] = (regmask & 1) ?	\
X				(next_addr -= 4) : 0;		\
X	(frame_saved_regs).regs[SP_REGNUM] = (frame_info).frame + 4;	\
X	(frame_saved_regs).regs[PC_REGNUM] = (frame_info).frame + 4;	\
X	(frame_saved_regs).regs[FP_REGNUM] = (read_memory_integer	\
X					((frame_info).frame, 4));	\
X}
X
X/* Things needed for making the inferior call functions.  */
X
X/* Push an empty stack frame, to record the current PC, etc.  */
X
X#define PUSH_DUMMY_FRAME \
X{ register CORE_ADDR sp = read_register (SP_REGNUM);\
X  register int regnum;				    \
X  sp = push_word (sp, read_register (PC_REGNUM));   \
X  sp = push_word (sp, read_register (FP_REGNUM));   \
X  write_register (FP_REGNUM, sp);		    \
X  for (regnum = 0; regnum < 8; regnum++)  \
X    sp = push_word (sp, read_register (regnum));    \
X  write_register (SP_REGNUM, sp);  			\
X}
X
X/* Discard from the stack the innermost frame, restoring all registers.  */
X
X#define POP_FRAME  \
X{ register CORE_ADDR fp = read_register (FP_REGNUM);		 \
X  register int regnum;						 \
X  struct frame_saved_regs fsr;					 \
X  struct frame_info fi;						 \
X  fi = get_frame_info (fp);					 \
X  get_frame_saved_regs (&fi, &fsr);				 \
X  for (regnum = 0; regnum < 8; regnum++)			 \
X    if (fsr.regs[regnum])					 \
X      write_register (regnum, read_memory_integer (fsr.regs[regnum], 4)); \
X  write_register (FP_REGNUM, read_memory_integer (fp, 4));	 \
X  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));   \
X  write_register (SP_REGNUM, fp + 8);				 \
X}
X
X/* This sequence of words is the instructions
X     enter	0xff,0		82 ff 00
X     jsr	@0x00010203	7f ae c0 01 02 03
X     adjspd	0x69696969	7f a5 01 02 03 04
X     bpt			f2
X   Note this is 16 bytes.  */
X
X#define CALL_DUMMY { 0x7f00ff82, 0x0201c0ae, 0x01a57f03, 0xf2040302 }
X
X#define CALL_DUMMY_START_OFFSET	3
X#define CALL_DUMMY_LENGTH	16
X#define CALL_DUMMY_ADDR		5
X#define CALL_DUMMY_NARGS	11
X
X/* Insert the specified number of args and function address
X   into a call sequence of the above form stored at DUMMYNAME.  */
X
X#define FIX_CALL_DUMMY(dummyname, fun, nargs)   		\
X{								\
X	int	flipped;					\
X	flipped = fun | 0xc0000000;				\
X	flip_bytes (&flipped, 4);				\
X	*((int *) (((char *) dummyname)+CALL_DUMMY_ADDR)) = flipped;	\
X	flipped = - nargs * 4;					\
X	flip_bytes (&flipped, 4);				\
X	*((int *) (((char *) dummyname)+CALL_DUMMY_NARGS)) = flipped;	\
X}
X
X#ifdef notdef
X/* Interface definitions for kernel debugger KDB.  */
X
X/* Map machine fault codes into signal numbers.
X   First subtract 0, divide by 4, then index in a table.
X   Faults for which the entry in this table is 0
X   are not handled by KDB; the program's own trap handler
X   gets to handle then.  */
X
X#define FAULT_CODE_ORIGIN 0
X#define FAULT_CODE_UNITS 4
X#define FAULT_TABLE    \
X{ 0, SIGKILL, SIGSEGV, 0, 0, 0, 0, 0, \
X  0, 0, SIGTRAP, SIGTRAP, 0, 0, 0, 0, \
X  0, 0, 0, 0, 0, 0, 0, 0}
X
X/* Start running with a stack stretching from BEG to END.
X   BEG and END should be symbols meaningful to the assembler.
X   This is used only for kdb.  */
X
X#define INIT_STACK(beg, end)  \
X{ asm (".globl end");         \
X  asm ("movl $ end, sp");      \
X  asm ("clrl fp"); }
X
X/* Push the frame pointer register on the stack.  */
X#define PUSH_FRAME_PTR        \
X  asm ("pushl fp");
X
X/* Copy the top-of-stack to the frame pointer register.  */
X#define POP_FRAME_PTR  \
X  asm ("movl (sp), fp");
X
X/* After KDB is entered by a fault, push all registers
X   that GDB thinks about (all NUM_REGS of them),
X   so that they appear in order of ascending GDB register number.
X   The fault code will be on the stack beyond the last register.  */
X
X#define PUSH_REGISTERS        \
X{ asm ("pushl 8(sp)");        \
X  asm ("pushl 8(sp)");        \
X  asm ("pushal 0x14(sp)");    \
X  asm ("pushr $037777"); }
X
X/* Assuming the registers (including processor status) have been
X   pushed on the stack in order of ascending GDB register number,
X   restore them and return to the address in the saved PC register.  */
X
X#define POP_REGISTERS      \
X{ asm ("popr $037777");    \
X  asm ("subl2 $8,(sp)");   \
X  asm ("movl (sp),sp");    \
X  asm ("rei"); }
X#endif
SHAR_EOF
sed 's/^X//' << 'SHAR_EOF' > n32k-opcode.h
X/* n32k-opcode.h */
X
X#ifndef n32k_opcodeT
X#define n32k_opcodeT int
X#endif /* no n32k_opcodeT */
X
Xstruct not_wot			/* n32k opcode table: wot to do with this */
X				/* particular opcode */
X{
X	int		obits;	/* number of opcode bits */
X	int		ibits;	/* number of instruction bits */
X  	n32k_opcodeT	code;	/* op-code (may be > 8 bits!) */
X	char 		*args;	/* how to compile said opcode */
X};
X
Xstruct not			/* n32k opcode text */
X{
X  char *            name;	/* opcode name: lowercase string  [key]  */
X  struct not_wot    detail;	/* rest of opcode table          [datum] */
X};
X
X/* F : 32 bit float
X * L : 64 bit float
X * B : byte
X * W : word
X * D : double-word
X * Q : quad-word
X * d : displacement
X * q : quick
X * i : immediate (8 bits)
X * r : register number (3 bits)
X * p : displacement - pc relative addressing
X*/
Xstatic struct not
Xnotstrs[] =
X{
X{ "absf",	14,24,	0x35be,	"1F2F" },
X{ "absl",	14,24,	0x34be,	"1L2L" },
X{ "absb",	14,24,	0x304e, "1B2B" },
X{ "absw",	14,24,	0x314e, "1W2W" },
X{ "absd",	14,24,	0x334e, "1D2D" },
X{ "acbb",	 7,16,	0x4c,	"2B1q3p" },
X{ "addf",	14,24,	0x01be,	"1F2F" },
X{ "addl",	14,24,	0x00be, "1L2L" },
X{ "addb",	 6,16,	0x00,	"1B2B" },
X{ "addw",	 6,16,	0x01,	"1W2W" },
X{ "addd",	 6,16,	0x03,	"1D2D" },
X{ "addcb",	 6,16,	0x10,	"1B2B" },
X{ "addcw",	 6,16,	0x11,	"1W2W" },
X{ "addcd",	 6,16,	0x13,	"1D2D" },
X{ "addpb",	14,24,	0x3c4e,	"1B2B" },
X{ "addpw",	14,24,	0x3d4e,	"1W2W" },
X{ "addpd",	14,24,	0x3f4e,	"1D2D" },
X{ "addqb",	 7,16,	0x0c,	"2B1q" },
X{ "addqw",	 7,16,	0x0d,	"2W1q" },
X{ "addqd",	 7,16,	0x0f,	"2D1q" },
X{ "addr",	 6,16,	0x27,	"1D2D" },
X{ "adjspb",	11,16,	0x057c,	"1B" },
X{ "adjspw",	11,16,	0x057d,	"1W" },
X{ "adjspd",	11,16,	0x057f,	"1D" },
X{ "andb",	 6,16,	0x28,	"1B2B" },
X{ "andw",	 6,16,	0x29,	"1W2W" },
X{ "andd",	 6,16,	0x2b,	"1D2D" },
X{ "ashb",	14,24,	0x044e,	"1B2B" },
X{ "ashw",	14,24,	0x054e,	"1B2W" },
X{ "ashd",	14,24,	0x074e,	"1B2D" },
X{ "beq",	 8,8,	0x0a,	"1p" },
X{ "bne",	 8,8,	0x1a,	"1p" },
X{ "bcs",	 8,8,	0x2a,	"1p" },
X{ "bcc",	 8,8,	0x3a,	"1p" },
X{ "bhi",	 8,8,	0x4a,	"1p" },
X{ "bls",	 8,8,	0x5a,	"1p" },
X{ "bgt",	 8,8,	0x6a,	"1p" },
X{ "ble",	 8,8,	0x7a,	"1p" },
X{ "bfs",	 8,8,	0x8a,	"1p" },
X{ "bfc",	 8,8,	0x9a,	"1p" },
X{ "blo",	 8,8,	0xaa,	"1p" },
X{ "bhs",	 8,8,	0xba,	"1p" },
X{ "blt",	 8,8,	0xca,	"1p" },
X{ "bge",	 8,8,	0xda,	"1p" },
X{ "bicb",	 6,16,	0x08,	"1B2B" },
X{ "bicw",	 6,16,	0x09,	"1W2W" },
X{ "bicd",	 6,16,	0x0b,	"1D2D" },
X{ "bicpsrb",	11,16,	0x17c,	"1B" },
X{ "bicpsrw",	11,16,	0x17d,	"1W" },
X{ "bispsrb",	11,16,	0x37c,	"1B" },
X{ "bispsrw",	11,16,	0x37d,	"1W" },
X{ "bpt",	 8,8,	0xf2,	"" },
X{ "br",		 8,8,	0xea,	"1p" },
X{ "bsr",	 8,8,	0x02,	"1p" },
X{ "caseb",	11,16,	0x77c,	"1B" },
X{ "casew",	11,16,	0x77d,	"1W" },
X{ "cased",	11,16,	0x77f,	"1D" },
X{ "cbitb",	14,24,	0x084e,	"1B2D" },
X{ "cbitw",	14,24,	0x094e,	"1W2D" },
X{ "cbitd",	14,24,	0x0b4e,	"1D2D" },
X{ "cbitib",	14,24,	0x0c4e,	"1B2D" },
X{ "cbitiw",	14,24,	0x0d4e,	"1W2D" },
X{ "cbitid",	14,24,	0x0f4e,	"1D2D" },
X{ "checkb",	11,24,	0x0ee,	"2A3B1r" },
X{ "checkw",	11,24,	0x1ee,	"2A3B1r" },
X{ "checkd",	11,24,	0x3ee,	"2A3D1r" },
X{ "cmpf",	14,24,	0x09be,	"1F2F" },
X{ "cmpl",	14,24,	0x08be,	"1L2L" },
X{ "cmpb",	 6,16,	0x04,	"1B2B" },
X{ "cmpw",	 6,16,	0x05,	"1W2W" },
X{ "cmpd",	 6,16,	0x07,	"1D2D" },
X{ "cmpmb",	14,24,	0x04ce,	"1D2D3d" },
X{ "cmpmw",	14,24,	0x05ce,	"1D2D3d" },
X{ "cmpmd",	14,24,	0x07ce,	"1D2D3d" },
X{ "cmpqb",	 7,16,	0x1c,	"2B1q" },
X{ "cmpqw",	 7,16,	0x1d,	"2W1q" },
X{ "cmpqd",	 7,16,	0x1f,	"2D1q" },
X{ "cmpsb",	16,16,	0x040e,	"i" },
X{ "cmpsw",	16,16,	0x050e,	"i" },
X{ "cmpsd",	16,16,	0x070e,	"i" },
X{ "cmpst",	16,16,	0x840e,	"i" },
X{ "comb",	14,24,	0x344e,	"1B2B" },
X{ "comw",	14,24,	0x354e,	"1W2W" },
X{ "comd",	14,24,	0x374e,	"1D2D" },
X{ "cvtp",	11,24,	0x036e,	"2D3D1r" },
X{ "cxp",	 8,8,	0x22,	"1p" },
X{ "cxpd",	11,16,	0x07f,	"1D" },
X{ "deib",	14,24,	0x2cce,	"1B2W" },
X{ "deiw",	14,24,	0x2cce,	"1W2D" },
X{ "deid",	14,24,	0x2cce,	"1D2Q" },
X{ "dia",	 8,8,	0xc2,	"" },
X{ "divf",	14,24,	0x21be,	"1F2F" },
X{ "divl",	14,24,	0x20be,	"1L2L" },
X{ "divb",	14,24,	0x3cce,	"1B2B" },
X{ "divw",	14,24,	0x3dce,	"1W2W" },
X{ "divd",	14,24,	0x3fce,	"1D2D" },
X{ "enter",	 8,8,	0x82,	"1i2d" },
X{ "exit",	 8,8,	0x92,	"1i" },
X{ "extb",	11,24,	0x02e,	"2D3B1r4d" },
X{ "extw",	11,24,	0x12e,	"2D3W1r4d" },
X{ "extd",	11,24,	0x32e,	"2D3D1r4d" },
X{ "extsb",	14,24,	0x0cce,	"1D2B3i" },
X{ "extsw",	14,24,	0x0dce,	"1D2W3i" },
X{ "extsd",	14,24,	0x0fce,	"1D2D3i" },
X{ "ffsb",	14,24,	0x046e,	"1B2B" },
X{ "ffsw",	14,24,	0x056e,	"1W2B" },
X{ "ffsd",	14,24,	0x076e,	"1D2B" },
X{ "flag",	 8,8,	0xd2,	"" },
X{ "floorfb",	14,24,	0x3c3e,	"1F2B" },
X{ "floorfw",	14,24,	0x3d3e,	"1F2W" },
X{ "floorfd",	14,24,	0x3f3e,	"1F2D" },
X{ "floorlb",	14,24,	0x383e,	"1L2B" },
X{ "floorlw",	14,24,	0x393e,	"1L2W" },
X{ "floorld",	14,24,	0x3b3e,	"1L2D" },
X{ "ibitb",	14,24,	0x384e,	"1B2D" },
X{ "ibitw",	14,24,	0x394e,	"1W2D" },
X{ "ibitd",	14,24,	0x3b4e,	"1D2D" },
X{ "indexb",	11,24,	0x42e,	"2B3B1r" },
X{ "indexw",	11,24,	0x52e,	"2W3W1r" },
X{ "indexd",	11,24,	0x72e,	"2D3D1r" },
X{ "insb",	11,24,	0x0ae,	"2B3B1r4d" },
X{ "insw",	11,24,	0x1ae,	"2W3W1r4d" },
X{ "insd",	11,24,	0x3ae,	"2D3D1r4d" },
X{ "inssb",	14,24,	0x08ce,	"1B2D3i" },
X{ "inssw",	14,24,	0x09ce,	"1W2D3i" },
X{ "inssd",	14,24,	0x0bce,	"1D2D3i" },
X{ "jsr",	11,16,	0x67f,	"1A" },
X{ "jump",	11,16,	0x27f,	"1A" },
X{ "lfsr",	19,24,	0x00f3e,"1D" },
X{ "lmr",	15,24,	0x0b1e,	"2D1q" },
X{ "lprb",	 7,16,	0x6c,	"2B1q" },
X{ "lprw",	 7,16,	0x6d,	"2W1q" },
X{ "lprd",	 7,16,	0x6f,	"2D1q" },
X{ "lshb",	14,24,	0x144e,	"1B2B" },
X{ "lshw",	14,24,	0x154e,	"1B2W" },
X{ "lshd",	14,24,	0x174e,	"1B2D" },
X{ "meib",	14,24,	0x24ce,	"1B2W" },
X{ "meiw",	14,24,	0x25ce,	"1W2D" },
X{ "meid",	14,24,	0x27ce,	"1D2Q" },
X{ "modb",	14,24,	0x38ce,	"1B2B" },
X{ "modw",	14,24,	0x39ce,	"1W2W" },
X{ "modd",	14,24,	0x3bce,	"1D2D" },
X{ "movf",	14,24,	0x05be,	"1F2F" },
X{ "movl",	14,24,	0x04be,	"1L2L" },
X{ "movb",	 6,16,	0x14,	"1B2B" },
X{ "movw",	 6,16,	0x15,	"1W2W" },
X{ "movd",	 6,16,	0x17,	"1D2D" },
X{ "movbf",	14,24,	0x043e,	"1B2F" },
X{ "movwf",	14,24,	0x053e,	"1W2F" },
X{ "movdf",	14,24,	0x073e,	"1D2F" },
X{ "movbl",	14,24,	0x003e,	"1B2L" },
X{ "movwl",	14,24,	0x013e,	"1W2L" },
X{ "movdl",	14,24,	0x033e,	"1D2L" },
X{ "movfl",	14,24,	0x1b3e,	"1F2L" },
X{ "movlf",	14,24,	0x163e,	"1L2F" },
X{ "movmb",	14,24,	0x00ce,	"1D2D3d" },
X{ "movmw",	14,24,	0x00de,	"1D2D3d" },
X{ "movmd",	14,24,	0x00fe,	"1D2D3d" },
X{ "movqb",	 7,16,	0x5c,	"2B1q" },
X{ "movqw",	 7,16,	0x5d,	"2B1q" },
X{ "movqd",	 7,16,	0x5f,	"2B1q" },
X{ "movsb",	16,16,	0x000e,	"i" },
X{ "movsw",	16,16,	0x010e,	"i" },
X{ "movsd",	16,16,	0x030e,	"i" },
X{ "movst",	16,16,	0x800e,	"i" },
X{ "movsub",	14,24,	0x0cae,	"1A1A" },
X{ "movsuw",	14,24,	0x0dae,	"1A1A" },
X{ "movsud",	14,24,	0x0fae,	"1A1A" },
X{ "movusb",	14,24,	0x1cae,	"1A1A" },
X{ "movusw",	14,24,	0x1dae,	"1A1A" },
X{ "movusd",	14,24,	0x1fae,	"1A1A" },
X{ "movxbd",	14,24,	0x1cce,	"1B2D" },
X{ "movxwd",	14,24,	0x1dce,	"1W2D" },
X{ "movxbw",	14,24,	0x10ce,	"1B2W" },
X{ "movzbd",	14,24,	0x18ce,	"1B2D" },
X{ "movzwd",	14,24,	0x19ce,	"1W2D" },
X{ "movzbw",	14,24,	0x14ce,	"1B2W" },
X{ "mulf",	14,24,	0x31be,	"1F2F" },
X{ "mull",	14,24,	0x30be,	"1L2L" },
X{ "mulb",	14,24,	0x20ce, "1B2B" },
X{ "mulw",	14,24,	0x21ce, "1W2W" },
X{ "muld",	14,24,	0x23ce, "1D2D" },
X{ "negf",	14,24,	0x15be, "1F2F" },
X{ "negl",	14,24,	0x14be, "1L2L" },
X{ "negb",	14,24,	0x204e, "1B2B" },
X{ "negw",	14,24,	0x214e, "1W2W" },
X{ "negd",	14,24,	0x234e, "1D2D" },
X{ "nop",	 8,8,	0xa2,	"" },
X{ "notb",	14,24,	0x244e, "1B2B" },
X{ "notw",	14,24,	0x254e, "1W2W" },
X{ "notd",	14,24,	0x274e, "1D2D" },
X{ "orb",	 6,16,	0x18,	"1B1B" },
X{ "orw",	 6,16,	0x19,	"1W1W" },
X{ "ord",	 6,16,	0x1b,	"1D1D" },
X{ "quob",	14,24,	0x30ce,	"1B2B" },
X{ "quow",	14,24,	0x31ce,	"1W2W" },
X{ "quod",	14,24,	0x33ce,	"1D2D" },
X{ "rdval",	19,24,	0x0031e,"1A" },
X{ "remb",	14,24,	0x34ce,	"1B2B" },
X{ "remw",	14,24,	0x35ce,	"1W2W" },
X{ "remd",	14,24,	0x37ce,	"1D2D" },
X{ "restore",	 8,8,	0x72,	"1i" },
X{ "ret",	 8,8,	0x12,	"1d" },
X{ "reti",	 8,8,	0x52,	"" },
X{ "rett",	 8,8,	0x42,	"" },
X{ "rotb",	14,24,	0x004e,	"1B2B" },
X{ "rotw",	14,24,	0x014e,	"1B2W" },
X{ "rotd",	14,24,	0x034e,	"1B2D" },
X{ "roundfb",	14,24,	0x243e,	"1F2B" },
X{ "roundfw",	14,24,	0x253e,	"1F2W" },
X{ "roundfd",	14,24,	0x273e,	"1F2D" },
X{ "roundlb",	14,24,	0x203e,	"1L2B" },
X{ "roundlw",	14,24,	0x213e,	"1L2W" },
X{ "roundld",	14,24,	0x233e,	"1L2D" },
X{ "rxp",	 8,8,	0x32,	"1d" },
X{ "sCONDb",	 7,16,	0x3c,	"2B1q" },
X{ "sCONDw",	 7,16,	0x3d,	"2D1q" },
X{ "sCONDd",	 7,16,	0x3f,	"2D1q" },
X{ "save",	 8,8,	0x62,	"1i" },
X{ "sbitb",	14,24,	0x184e,	"1B2A" },
X{ "sbitw",	14,24,	0x194e,	"1W2A" },
X{ "sbitd",	14,24,	0x1b4e,	"1D2A" },
X{ "sbitib",	14,24,	0x1c4e,	"1B2A" },
X{ "sbitiw",	14,24,	0x1d4e,	"1W2A" },
X{ "sbitid",	14,24,	0x1f4e,	"1D2A" },
X{ "setcfg",	15,24,	0x0b0e,	"5D1q" },
X{ "sfsr",	14,24,	0x673e,	"5D1D" },
X{ "skpsb",	16,16,	0x0c0e,	"i" },
X{ "skpsw",	16,16,	0x0d0e,	"i" },
X{ "skpsd",	16,16,	0x0f0e, "i" },
X{ "skpst",	16,16,	0x8c0e,	"i" },
X{ "smr",	15,24,	0x0f1e,	"2D1q" },
X{ "sprb",	 7,16,	0x2c,	"2B1q" },
X{ "sprw",	 7,16,	0x2d,	"2W1q" },
X{ "sprd",	 7,16,	0x2f,	"2D1q" },
X{ "subf",	14,24,	0x11be,	"1F2F" },
X{ "subl",	14,24,	0x10be,	"1L2L" },
X{ "subb",	 6,16,	0x20,	"1B2B" },
X{ "subw",	 6,16,	0x21,	"1W2W" },
X{ "subd",	 6,16,	0x23,	"1D2D" },
X{ "subcb",	 6,16,	0x30,	"1B2B" },
X{ "subcw",	 6,16,	0x31,	"1W2W" },
X{ "subcd",	 6,16,	0x33,	"1D2D" },
X{ "subpb",	14,24,	0x2c4e,	"1B2B" },
X{ "subpw",	14,24,	0x2d4e,	"1W2W" },
X{ "subpd",	14,24,	0x2f4e,	"1D2D" },
X{ "svc",	 8,8,	0xe2,	"2i1i" }, /* not really, but unix uses it */
X{ "tbitb",	 6,16,	0x34,	"1B2A" },
X{ "tbitw",	 6,16,	0x35,	"1W2A" },
X{ "tbitd",	 6,16,	0x37,	"1D2A" },
X{ "truncfb",	14,24,	0x2c3e,	"1F2B" },
X{ "truncfw",	14,24,	0x2d3e,	"1F2W" },
X{ "truncfd",	14,24,	0x2f3e,	"1F2D" },
X{ "trunclb",	14,24,	0x283e,	"1L2B" },
X{ "trunclw",	14,24,	0x293e,	"1L2W" },
X{ "truncld",	14,24,	0x2b3e,	"1L2D" },
X{ "wait",	 8,8,	0xb2,	"" },
X{ "wrval",	19,24,	0x0071e,"1A" },
X{ "xorb",	 6,16,	0x38,	"1B2B" },
X{ "xorw",	 6,16,	0x39,	"1W2W" },
X{ "xord",	 6,16,	0x3b,	"1D2D" },
X};				/* notstrs */
X
X/* end: n32k.opcode.h */
X
X# define	MAX_ARGS	4
X# define	ARG_LEN		50
SHAR_EOF
sed 's/^X//' << 'SHAR_EOF' > n32k-pinsn.c
X/* Print 32000 instructions for GDB, the GNU debugger.
X   Copyright (C) 1986 Free Software Foundation, Inc.
X
XGDB is distributed in the hope that it will be useful, but WITHOUT ANY
XWARRANTY.  No author or distributor accepts responsibility to anyone
Xfor the consequences of using it or for whether it serves any
Xparticular purpose or works at all, unless he says so in writing.
XRefer to the GDB General Public License for full details.
X
XEveryone is granted permission to copy, modify and redistribute GDB,
Xbut only under the conditions described in the GDB General Public
XLicense.  A copy of this license is supposed to have been given to you
Xalong with GDB so you can know your rights and responsibilities.  It
Xshould be in a file named COPYING.  Among other things, the copyright
Xnotice and this notice must be preserved on all copies.
X
XIn other words, go ahead and share GDB, but don't try to stop
Xanyone else from sharing it farther.  Help stamp out software hoarding!
X*/
X
X#include <stdio.h>
X
X#include "defs.h"
X#include "param.h"
X#include "symtab.h"
X#include "n32k-opcode.h"
X
X/* 32000 instructions are never longer than this.  */
X#define MAXLEN 62
X
X/* Number of elements in the opcode table.  */
X#define NOPCODES (sizeof notstrs / sizeof notstrs[0])
X
Xextern char *reg_names[];
X
X#define NEXT_IS_ADDR	'|'
X
X/*
X * extract "count" bits starting "offset" bits
X * into buffer
X */
X
Xint
Xbit_extract (buffer, offset, count)
Xchar	*buffer;
Xint	offset;
Xint	count;
X{
X	int	result;
X	int	mask;
X	int	bit;
X
X	buffer += offset >> 3;
X	offset &= 7;
X	bit = 1;
X	result = 0;
X	while (count--) {
X		if ((*buffer & (1 << offset)))
X			result |= bit;
X		if (++offset == 8) {
X			offset = 0;
X			buffer++;
X		}
X		bit <<= 1;
X	}
X	return result;
X}
X
Xdouble
Xdbit_extract (buffer, offset, count)
X{
X	union {
X		struct {
X			int	low, high;
X		} ival;
X		double	dval;
X	} foo;
X
X	foo.ival.low = bit_extract (buffer, offset, 32);
X	foo.ival.high = bit_extract (buffer, offset+32, 32);
X	return foo.dval;
X}
X
Xsign_extend (value, bits)
X{
X	value = value & ((1 << bits) - 1);
X	return (value & (1 << (bits-1))) ?
X		(value | (~((1 << bits) - 1)))
X		: value;
X}
X
Xflip_bytes (ptr, count)
Xchar	*ptr;
Xint	count;
X{
X	char	tmp;
X
X	while (count > 0) {
X		tmp = *ptr;
X		ptr[0] = ptr[count-1];
X		ptr[count-1] = tmp;
X		ptr++;
X		count -= 2;
X	}
X}
X
X
X/* Print the 32000 instruction at address MEMADDR in debugged memory,
X   on STREAM.  Returns length of the instruction, in bytes.  */
X
Xint
Xprint_insn (memaddr, stream)
XCORE_ADDR memaddr;
XFILE *stream;
X{
X	unsigned char buffer[MAXLEN];
X	register int i;
X	register unsigned char *p;
X	register char *d;
X	unsigned short	first_word;
X	int	gen, disp;
X	int	ioffset;	/* bits into instruction */
X	int	aoffset;	/* bits into arguments */
X	char	arg_bufs[MAX_ARGS+1][ARG_LEN];
X	int	argnum;
X	int	maxarg;
X
X	read_memory (memaddr, buffer, MAXLEN);
X
X	first_word = *(unsigned short *) buffer;
X	for (i = 0; i < NOPCODES; i++) {
X		if ((first_word & ((1 << notstrs[i].detail.obits) - 1)) ==
X		    notstrs[i].detail.code)
X			break;
X	}
X
X	/* Handle undefined instructions.  */
X	if (i == NOPCODES) {
X		fprintf (stream, "0%o", buffer[0]);
X		return 1;
X	}
X
X	fprintf (stream, "%s", notstrs[i].name);
X
X	ioffset = notstrs[i].detail.ibits;
X	aoffset = notstrs[i].detail.ibits;
X	d = notstrs[i].detail.args;
X
X	if (*d) {
X		fputc ('\t', stream);
X
X		maxarg = 0;
X		while (*d)
X		{
X			argnum = *d - '1';
X			d++;
X			if (argnum > maxarg && argnum < MAX_ARGS)
X				maxarg = argnum;
X			ioffset = print_insn_arg (*d, ioffset, &aoffset, buffer,
X	 				memaddr, arg_bufs[argnum]);
X			d++;
X		}
X		for (argnum = 0; argnum <= maxarg; argnum++) {
X			CORE_ADDR	addr;
X			char		*ch, *index ();
X			for (ch = arg_bufs[argnum]; *ch;) {
X				if (*ch == NEXT_IS_ADDR) {
X					++ch;
X					addr = atoi (ch);
X					print_address (addr, stream);
X					while (*ch && *ch != NEXT_IS_ADDR)
X						++ch;
X					if (*ch)
X						++ch;
X				} else
X					putc (*ch++, stream);
X			}
X			if (argnum < maxarg)
X				fprintf (stream, ", ");
X		}
X	}
X	return aoffset / 8;
X}
X
Xprint_insn_arg (d, ioffset, aoffsetp, buffer, addr, result)
Xchar 	d;
Xint	ioffset, *aoffsetp;
Xchar	*buffer;
XCORE_ADDR addr;
Xchar	*result;
X{
X	int	addr_mode;
X	float	Fvalue;
X	double	Lvalue;
X	int	Ivalue;
X	int	disp1, disp2;
X	int	index;
X
X	switch (d) {
X	case 'F':
X	case 'L':
X	case 'B':
X	case 'W':
X	case 'D':
X	case 'A':
X		addr_mode = bit_extract (buffer, ioffset-5, 5);
X		ioffset -= 5;
X		switch (addr_mode) {
X		case 0x0: case 0x1: case 0x2: case 0x3:
X		case 0x4: case 0x5: case 0x6: case 0x7:
X			sprintf (result, "r%d", addr_mode);
X			break;
X		case 0x8: case 0x9: case 0xa: case 0xb:
X		case 0xc: case 0xd: case 0xe: case 0xf:
X			disp1 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "%d(r%d)", disp1, addr_mode & 7);
X			break;
X		case 0x10:
X		case 0x11:
X		case 0x12:
X			disp1 = get_displacement (buffer, aoffsetp);
X			disp2 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "%d(%d(%s))", disp2, disp1,
X				addr_mode==0x10?"fp":addr_mode==0x11?"sp":"sb");
X			break;
X		case 0x13:
X			sprintf (result, "reserved");
X			break;
X		case 0x14:
X			switch (d) {
X			case 'B':
X				Ivalue = bit_extract (buffer, *aoffsetp, 8);
X				Ivalue = sign_extend (Ivalue, 8);
X				*aoffsetp += 8;
X				sprintf (result, "$%d", Ivalue);
X				break;
X			case 'W':
X				Ivalue = bit_extract (buffer, *aoffsetp, 16);
X				flip_bytes (&Ivalue, 2);
X				*aoffsetp += 16;
X				Ivalue = sign_extend (Ivalue, 16);
X				sprintf (result, "$%d", Ivalue);
X				break;
X			case 'D':
X				Ivalue = bit_extract (buffer, *aoffsetp, 32);
X				flip_bytes (&Ivalue, 4);
X				*aoffsetp += 32;
X				sprintf (result, "$%d", Ivalue);
X				break;
X			case 'A':
X				Ivalue = bit_extract (buffer, *aoffsetp, 32);
X				flip_bytes (&Ivalue, 4);
X				*aoffsetp += 32;
X				sprintf (result, "$|%d|", Ivalue);
X				break;
X			case 'F':
X				Fvalue = (float) bit_extract
X 					(buffer, *aoffsetp, 32);
X				flip_bytes (&Fvalue, 4);
X				*aoffsetp += 32;
X				sprintf (result, "$%g", Fvalue);
X				break;
X			case 'L':
X				Lvalue = dbit_extract
X 					(buffer, *aoffsetp, 64);
X				flip_bytes (&Lvalue, 8);
X				*aoffsetp += 64;
X				sprintf (result, "$%g", Lvalue);
X				break;
X			}
X			break;
X		case 0x15:
X			disp1 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "@|%d|", disp1);
X			break;
X		case 0x16:
X			disp1 = get_displacement (buffer, aoffsetp);
X			disp2 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "EXT(%d) + %d", disp1, disp2);
X			break;
X		case 0x17:
X			sprintf (result, "tos");
X			break;
X		case 0x18:
X			disp1 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "%d(fp)", disp1);
X			break;
X		case 0x19:
X			disp1 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "%d(sp)", disp1);
X			break;
X		case 0x1a:
X			disp1 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "%d(sb)", disp1);
X			break;
X		case 0x1b:
X			disp1 = get_displacement (buffer, aoffsetp);
X			sprintf (result, "|%d|", addr + disp1);
X			break;
X		case 0x1c:
X		case 0x1d:
X		case 0x1e:
X		case 0x1f:
X			index = bit_extract (buffer, *aoffsetp, 8);
X			*aoffsetp += 8;
X			print_insn_arg (d, *aoffsetp, aoffsetp, buffer, addr,
X				result);
X			{
X 				static char	*ind[] = {"b", "w", "d", "q"};
X				char		*off;
X
X				off = result + strlen (result);
X				sprintf (off, "[r%d:%s]", index & 7,
X					ind[addr_mode & 3]);
X			}
X			break;
X		}
X		break;
X	case 'q':
X		Ivalue = bit_extract (buffer, ioffset-4, 4);
X		Ivalue = sign_extend (Ivalue, 4);
X		sprintf (result, "%d", Ivalue);
X		ioffset -= 4;
X		break;
X	case 'r':
X		Ivalue = bit_extract (buffer, ioffset-3, 3);
X		sprintf (result, "r%d", Ivalue&7);
X		ioffset -= 3;
X		break;
X	case 'd':
X		sprintf (result, "%d", get_displacement (buffer, aoffsetp));
X		break;
X	case 'p':
X		sprintf (result, "%c%d%c", NEXT_IS_ADDR, addr +
X 			get_displacement (buffer, aoffsetp),
X			NEXT_IS_ADDR);
X		break;
X	case 'i':
X		Ivalue = bit_extract (buffer, *aoffsetp, 8);
X		*aoffsetp += 8;
X		sprintf (result, "0x%x", Ivalue);
X		break;
X	}
X	return ioffset;
X}
X
Xget_displacement (buffer, aoffsetp)
Xchar	*buffer;
Xint	*aoffsetp;
X{
X	int	Ivalue;
X
X	Ivalue = bit_extract (buffer, *aoffsetp, 8);
X	switch (Ivalue & 0xc0) {
X	case 0x00:
X	case 0x40:
X		Ivalue = sign_extend (Ivalue, 7);
X		*aoffsetp += 8;
X		break;
X	case 0x80:
X		Ivalue = bit_extract (buffer, *aoffsetp, 16);
X		flip_bytes (&Ivalue, 2);
X		Ivalue = sign_extend (Ivalue, 14);
X		*aoffsetp += 16;
X		break;
X	case 0xc0:
X		Ivalue = bit_extract (buffer, *aoffsetp, 32);
X		flip_bytes (&Ivalue, 4);
X		Ivalue = sign_extend (Ivalue, 30);
X		*aoffsetp += 32;
X		break;
X	}
X	return Ivalue;
X}
X/*
X * return the number of locals in the current frame given a pc
X * pointing to the enter instruction
X */
Xn32k_localcount (enter_pc)
XCORE_ADDR	enter_pc;
X{
X	int	localtype, localcount;
X
X	localtype = read_memory_integer (enter_pc+2, 1);
X	if ((localtype & 0x80) == 0)
X		localcount = localtype;
X	else if ((localtype & 0xc0) == 0x80)
X		localcount =
X		    ((read_memory_integer (enter_pc+2, 1) & ~0xc0) << 8) |
X		    ((read_memory_integer (enter_pc+3, 1)));
X	else
X		localcount =
X		    ((read_memory_integer (enter_pc+2, 1) & ~0xc0) << 24) |
X		    ((read_memory_integer (enter_pc+3, 1)) << 16) |
X		    ((read_memory_integer (enter_pc+4, 1)) << 8 ) |
X		    ((read_memory_integer (enter_pc+5, 1)));
X	return localcount;
X}
SHAR_EOF
exit

