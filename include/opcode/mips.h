/* mips.h.  Mips opcode list for GDB, the GNU debugger.
   Copyright 1993 Free Software Foundation, Inc.
   Contributed by Ralph Campbell and OSF
   Commented and modified by Ian Lance Taylor, Cygnus Support

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* These are bit masks and shift counts to use to access the various
   fields of an instruction.  To retrieve the X field of an
   instruction, use the expression
	(i >> OP_SH_X) & OP_MASK_X
   To set the same field (to j), use
	i = (i &~ (OP_MASK_X << OP_SH_X)) | (j << OP_SH_X)

   Make sure you use fields that are appropriate for the instruction,
   of course.  

   The 'i' format uses OP, RS, RT and IMMEDIATE.  

   The 'j' format uses OP and TARGET.

   The 'r' format uses OP, RS, RT, RD, SHAMT and FUNCT.

   The 'b' format uses OP, RS, RT and DELTA.

   The floating point 'i' format uses OP, RS, RT and IMMEDIATE.

   The floating point 'r' format uses OP, FMT, FT, FS, FD and FUNCT.

   A breakpoint instruction uses OP, CODE and SPEC (10 bits of the
   breakpoint instruction are not defined; Kane says the breakpoint
   code field in BREAK is 20 bits; yet MIPS assemblers and debuggers
   only use ten bits).

   The syscall instruction uses SYSCALL.

   The general coprocessor instructions use COPZ.  */

#define OP_MASK_OP		0x3f
#define OP_SH_OP		26
#define OP_MASK_RS		0x1f
#define OP_SH_RS		21
#define OP_MASK_FMT		0x1f
#define OP_SH_FMT		21
#define OP_MASK_CODE		0x3ff
#define OP_SH_CODE		16
#define OP_MASK_RT		0x1f
#define OP_SH_RT		16
#define OP_MASK_FT		0x1f
#define OP_SH_FT		16
#define OP_MASK_RD		0x1f
#define OP_SH_RD		11
#define OP_MASK_FS		0x1f
#define OP_SH_FS		11
#define OP_MASK_SYSCALL		0xfffff
#define OP_SH_SYSCALL		6
#define OP_MASK_SHAMT		0x1f
#define OP_SH_SHAMT		6
#define OP_MASK_FD		0x1f
#define OP_SH_FD		6
#define OP_MASK_TARGET		0x3ffffff
#define OP_SH_TARGET		0
#define OP_MASK_COPZ		0x1ffffff
#define OP_SH_COPZ		0
#define OP_MASK_IMMEDIATE	0xffff
#define OP_SH_IMMEDIATE		0
#define OP_MASK_DELTA		0xffff
#define OP_SH_DELTA		0
#define OP_MASK_FUNCT		0x3f
#define OP_SH_FUNCT		0
#define OP_MASK_SPEC		0x3f
#define OP_SH_SPEC		0

/* This structure holds information for a particular instruction.  */

struct mips_opcode
{
  /* The name of the instruction.  */
  const char *name;
  /* A string describing the arguments for this instruction.  */
  const char *args;
  /* The basic opcode for the instruction.  When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  */
  unsigned long match;
  /* If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */
  unsigned long mask;
  /* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */
  unsigned long pinfo;
};

/* These are the characters which may appears in the args field of an
   instruction.  They appear in the order in which the fields appear
   when the instruction is used.  Commas and parentheses in the args
   string are ignored when assembling, and written into the output
   when disassembling.

   Each of these characters corresponds to a mask field defined above.

   "<" 5 bit shift amount (OP_*_SHAMT)
   "a" 26 bit target address (OP_*_TARGET)
   "b" 5 bit base register (OP_*_RS)
   "c" 10 bit breakpoint code (OP_*_CODE)
   "d" 5 bit destination register specifier (OP_*_RD)
   "i" 16 bit unsigned immediate (OP_*_IMMEDIATE)
   "j" 16 bit signed immediate (OP_*_DELTA)
   "o" 16 bit signed offset (OP_*_DELTA)
   "p" 16 bit PC relative branch target address (OP_*_DELTA)
   "r" 5 bit same register used as both source and target (OP_*_RS)
   "s" 5 bit source register specifier (OP_*_RS)
   "t" 5 bit target register (OP_*_RT)
   "u" 16 bit upper 16 bits of address (OP_*_IMMEDIATE)
   "v" 5 bit same register used as both source and destination (OP_*_RS)
   "w" 5 bit same register used as both target and destination (OP_*_RT)
   "C" 25 bit coprocessor function code (OP_*_COPZ)
   "B" 20 bit syscall function code (OP_*_SYSCALL)

   Floating point instructions:
   "D" 5 bit destination register (OP_*_FD)
   "S" 5 bit fs source 1 register (OP_*_FS)
   "T" 5 bit ft source 2 register (OP_*_FT)
   "V" 5 bit same register used as floating source and destination (OP_*_FS)
   "W" 5 bit same register used as floating target and destination (OP_*_FT)

   Coprocessor instructions:
   "E" 5 bit target register (OP_*_RT)
   "G" 5 bit destination register (OP_*_RD)

   Macro instructions:
   "I" 32 bit immediate
   "F" 64 bit floating point constant
*/

/* These are the bits which may be set in the pinfo field of an
   instructions, if it is not equal to INSN_MACRO.  */

/* Modifies the general purpose register in OP_*_RD.  */
#define INSN_WRITE_GPR_D            0x00000001
/* Modifies the general purpose register in OP_*_RS (FIXME: not used).  */
#define INSN_WRITE_GPR_S            0x00000002
/* Modifies the general purpose register in OP_*_RT.  */
#define INSN_WRITE_GPR_T            0x00000004
/* Modifies general purpose register 31.  */
#define INSN_WRITE_GPR_31           0x00000008
/* Modifies the floating point register in OP_*_FD.  */
#define INSN_WRITE_FPR_D            0x00000010
/* Modifies the floating point register in OP_*_FS (FIXME: not used).  */
#define INSN_WRITE_FPR_S            0x00000020
/* Modifies the floating point register in OP_*_FT.  */
#define INSN_WRITE_FPR_T            0x00000040
/* Reads the general purpose register in OP_*_RD (FIXME: not used).  */
#define INSN_READ_GPR_D             0x00000080
/* Reads the general purpose register in OP_*_RS.  */
#define INSN_READ_GPR_S             0x00000100
/* Reads the general purpose register in OP_*_RT.  */
#define INSN_READ_GPR_T             0x00000200
/* Reads general purpose register 31 (FIXME: not used).  */
#define INSN_READ_GPR_31            0x00000400
/* Reads the floating point register in OP_*_FD (FIXME: not used).  */
#define INSN_READ_FPR_D             0x00000800
/* Reads the floating point register in OP_*_FS.  */
#define INSN_READ_FPR_S             0x00001000
/* Reads the floating point register in OP_*_FT.  */
#define INSN_READ_FPR_T             0x00002000
/* Modifies coprocessor condition code.  */
#define INSN_WRITE_COND_CODE        0x00004000
/* Reads coprocessor condition code.  */
#define INSN_READ_COND_CODE         0x00008000
/* TLB operation.  */
#define INSN_TLB                    0x00010000
/* RFE (return from exception) instruction.  */
#define INSN_RFE                    0x00020000
/* Reads coprocessor register other than floating point register.  */
#define INSN_COP                    0x00040000
/* Instruction destination requires load delay.  */
#define INSN_LOAD_DELAY             0x00080000
/* Instruction has unconditional branch delay slot.  */
#define INSN_UNCOND_BRANCH_DELAY    0x00100000
/* Instruction has conditional branch delay slot.  */
#define INSN_COND_BRANCH_DELAY      0x00200000
/* Writes coprocessor register, requiring delay.  */
#define INSN_COPROC_DELAY           0x00400000
/* Reads the HI register.  */
#define INSN_READ_HI		    0x00800000
/* Reads the LO register.  */
#define INSN_READ_LO		    0x01000000
/* Modifies the HI register.  */
#define INSN_WRITE_HI		    0x02000000
/* Modifies the LO register.  */
#define INSN_WRITE_LO		    0x04000000
/* Takes a trap (FIXME: why is this interesting?).  */
#define INSN_TRAP                   0x08000000
/* R4000 instruction.  */
#define INSN_R4000	            0x80000000

/* Instruction is actually a macro.  It should be ignored by the
   disassembler, and requires special treatment by the assembler.  */
#define INSN_MACRO                  0xffffffff

/* This is a list of macro expanded instructions.
 *
 * _I appended means immediate
 * _A appended means address
 * _AB appended means address with base register
 * _D appended means floating point constant
 */
enum {
    M_ABS,
    M_ABSU,
    M_ADD_I,
    M_ADDU_I,
    M_AND_I,
    M_BEQ_I,
    M_BGE,
    M_BGE_I,
    M_BGEU,
    M_BGEU_I,
    M_BGT,
    M_BGT_I,
    M_BGTU,
    M_BGTU_I,
    M_BLE,
    M_BLE_I,
    M_BLEU,
    M_BLEU_I,
    M_BLT,
    M_BLT_I,
    M_BLTU,
    M_BLTU_I,
    M_BNE_I,
    M_DIV_3,
    M_DIV_3I,
    M_DIVU_3,
    M_DIVU_3I,
    M_L_DOB,
    M_L_DAB,
    M_LA,
    M_LA_AB,
    M_LB_A,
    M_LB_AB,
    M_LBU_A,
    M_LBU_AB,
    M_LD_A,
    M_LD_OB,
    M_LD_AB,
    M_LH_A,
    M_LH_AB,
    M_LHU_A,
    M_LHU_AB,
    M_LI,
    M_LI_D,
    M_LI_DD,
    M_LS_A,
    M_LW_A,
    M_LW_AB,
    M_LWC0_A,
    M_LWC0_AB,
    M_LWC1_A,
    M_LWC1_AB,
    M_LWC2_A,
    M_LWC2_AB,
    M_LWC3_A,
    M_LWC3_AB,
    M_LWL_A,
    M_LWL_AB,
    M_LWR_A,
    M_LWR_AB,
    M_MUL,
    M_MUL_I, 
    M_MULO,
    M_MULO_I, 
    M_MULOU,
    M_MULOU_I, 
    M_NOR_I,
    M_OR_I,
    M_REM_3,
    M_REM_3I,
    M_REMU_3,
    M_REMU_3I,
    M_ROL,
    M_ROL_I,
    M_ROR,
    M_ROR_I,
    M_S_DA,
    M_S_DOB,
    M_S_DAB,
    M_S_S,
    M_SD_A,
    M_SD_OB,
    M_SD_AB,
    M_SEQ,
    M_SEQ_I,
    M_SGE,
    M_SGE_I,
    M_SGEU,
    M_SGEU_I,
    M_SGT,
    M_SGT_I,
    M_SGTU,
    M_SGTU_I,
    M_SLE,
    M_SLE_I,
    M_SLEU,
    M_SLEU_I,
    M_SLT_I,
    M_SLTU_I,
    M_SNE,
    M_SNE_I,
    M_SB_A,
    M_SB_AB,
    M_SH_A,
    M_SH_AB,
    M_SW_A,
    M_SW_AB,
    M_SWC0_A,
    M_SWC0_AB,
    M_SWC1_A,
    M_SWC1_AB,
    M_SWC2_A,
    M_SWC2_AB,
    M_SWC3_A,
    M_SWC3_AB,
    M_SWL_A,
    M_SWL_AB,
    M_SWR_A,
    M_SWR_AB,
    M_SUB_I,
    M_SUBU_I,
    M_TRUNCWD,
    M_TRUNCWS,
    M_ULH,
    M_ULH_A,
    M_ULHU,
    M_ULHU_A,
    M_ULW,
    M_ULW_A,
    M_USH,
    M_USH_A,
    M_USW,
    M_USW_A,
    M_XOR_I
};

/* Short hand so the lines aren't too long. */

#define LDD     INSN_LOAD_DELAY
#define UBD     INSN_UNCOND_BRANCH_DELAY
#define CBD	INSN_COND_BRANCH_DELAY
#define COD     INSN_COPROC_DELAY

/* True if this instruction may require a delay slot.  */
#define ANY_DELAY (LDD|UBD|CBD|COD \
		   |INSN_READ_HI|INSN_READ_LO \
		   |INSN_READ_COND_CODE|INSN_WRITE_COND_CODE)

#define WR_d    INSN_WRITE_GPR_D
#define WR_t    INSN_WRITE_GPR_T
#define WR_31   INSN_WRITE_GPR_31       
#define WR_D    INSN_WRITE_FPR_D        
#define WR_T	INSN_WRITE_FPR_T
#define RD_s    INSN_READ_GPR_S         
#define RD_b    INSN_READ_GPR_S         
#define RD_t    INSN_READ_GPR_T         
#define RD_S    INSN_READ_FPR_S         
#define RD_T    INSN_READ_FPR_T         
#define WR_CC	INSN_WRITE_COND_CODE
#define RD_CC	INSN_READ_COND_CODE
#define RD_C0   INSN_COP
#define RD_C1	INSN_COP
#define RD_C2   INSN_COP
#define RD_C3   INSN_COP
#define WR_C0   INSN_COP
#define WR_C1	INSN_COP
#define WR_C2   INSN_COP
#define WR_C3   INSN_COP
#define WR_HI	INSN_WRITE_HI
#define WR_LO	INSN_WRITE_LO
#define RD_HI	INSN_READ_HI
#define RD_LO	INSN_READ_LO

/* The order of overloaded instructions matters.  Label arguments and
   register arguments look the same. Instructions that can have either
   for arguments must apear in the correct order in this table for the
   assembler to pick the right one. In other words, entries with
   immediate operands must apear after the same instruction with
   registers.

   Many instructions are short hand for other instructions (i.e., The
   jal <register> instruction is short for jalr <register>).  */

static const struct mips_opcode mips_opcodes[] = {
/* These instructions appear first so that the disassembler will find
   them first.  The assemblers uses a hash table based on the
   instruction name anyhow.  */
{"nop",     "",		0x00000000, 0xffffffff,	0		},
{"li",      "t,j",      0x24000000, 0xffe00000, WR_t		}, /* addiu */
{"li",	    "t,i",	0x34000000, 0xffe00000, WR_t		}, /* ori */
{"li",      "t,I",	0,    (int) M_LI,	INSN_MACRO	},
{"move",    "d,s",	0x00000021, 0xfc1f07ff,	WR_d|RD_s	}, /* addu */
{"b",       "p",	0x10000000, 0xffff0000,	UBD		}, /* beq 0,0 */
{"b",       "p",	0x40100000, 0xffff0000,	UBD		}, /* bgez 0 */
{"bal",     "p",	0x04110000, 0xffff0000,	UBD		}, /* bgezal 0 */

{"abs",     "d,v",	0,    (int) M_ABS,	INSN_MACRO	},
{"abs.s",   "D,V",	0x46000005, 0xffff003f,	WR_D|RD_S	},
{"abs.d",   "D,V",	0x46200005, 0xffff003f,	WR_D|RD_S	},
{"absu",    "d,s",	0,    (int) M_ABSU,	INSN_MACRO	},
{"add",     "d,v,t",	0x00000020, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"add",     "t,r,I",	0,    (int) M_ADD_I,	INSN_MACRO	},
{"add.s",   "D,V,T",	0x46000000, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"add.d",   "D,V,T",	0x46200000, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"addi",    "t,r,j",	0x20000000, 0xfc000000,	WR_t|RD_s	},
{"addiu",   "t,r,j",	0x24000000, 0xfc000000,	WR_t|RD_s	},
{"addu",    "d,v,t",	0x00000021, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"addu",    "t,r,I",	0,    (int) M_ADDU_I,	INSN_MACRO	},
{"and",     "d,v,t",	0x00000024, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"and",     "t,r,I",	0,    (int) M_AND_I,	INSN_MACRO	},
{"andi",    "t,r,i",	0x30000000, 0xfc000000,	WR_t|RD_s	},
/* b is at the top of the table.  */
/* bal is at the top of the table.  */
{"bc0f",    "p",	0x41000000, 0xffff0000,	CBD|RD_CC	},
{"bc1f",    "p",	0x45000000, 0xffff0000,	CBD|RD_CC	},
{"bc2f",    "p",	0x49000000, 0xffff0000,	CBD|RD_CC	},
{"bc3f",    "p",	0x4d000000, 0xffff0000,	CBD|RD_CC	},
{"bc0t",    "p",	0x41010000, 0xffff0000,	CBD|RD_CC	},
{"bc1t",    "p",	0x45010000, 0xffff0000,	CBD|RD_CC	},
{"bc2t",    "p",	0x49010000, 0xffff0000,	CBD|RD_CC	},
{"bc3t",    "p",	0x4d010000, 0xffff0000,	CBD|RD_CC	},
{"beq",     "s,t,p",	0x10000000, 0xfc000000,	CBD|RD_s|RD_t	},
{"beq",     "s,I,p",	0,    (int) M_BEQ_I,	INSN_MACRO	},
{"beqz",    "s,p",	0x10000000, 0xfc1f0000,	CBD|RD_s	},
{"bge",     "s,t,p",	0,    (int) M_BGE,	INSN_MACRO	},
{"bge",     "s,I,p",	0,    (int) M_BGE_I,	INSN_MACRO	},
{"bgeu",    "s,t,p",	0,    (int) M_BGEU,	INSN_MACRO	},
{"bgeu",    "s,I,p",	0,    (int) M_BGEU_I,	INSN_MACRO	},
{"bgez",    "s,p",	0x04010000, 0xfc1f0000,	CBD|RD_s	},
{"bgezal",  "s,p",	0x04110000, 0xfc1f0000,	CBD|RD_s	},
{"bgt",     "s,t,p",	0,    (int) M_BGT,	INSN_MACRO	},
{"bgt",     "s,I,p",	0,    (int) M_BGT_I,	INSN_MACRO	},
{"bgtu",    "s,t,p",	0,    (int) M_BGTU,	INSN_MACRO	},
{"bgtu",    "s,I,p",	0,    (int) M_BGTU_I,	INSN_MACRO	},
{"bgtz",    "s,p",	0x1c000000, 0xfc1f0000,	CBD|RD_s	},
{"ble",     "s,t,p",	0,    (int) M_BLE,	INSN_MACRO	},
{"ble",     "s,I,p",	0,    (int) M_BLE_I,	INSN_MACRO	},
{"bleu",    "s,t,p",	0,    (int) M_BLEU,	INSN_MACRO	},
{"bleu",    "s,I,p",	0,    (int) M_BLEU_I,	INSN_MACRO	},
{"blez",    "s,p",	0x18000000, 0xfc1f0000,	CBD|RD_s	},
{"blt",     "s,t,p",	0,    (int) M_BLT,	INSN_MACRO	},
{"blt",     "s,I,p",	0,    (int) M_BLT_I,	INSN_MACRO	},
{"bltu",    "s,t,p",	0,    (int) M_BLTU,	INSN_MACRO	},
{"bltu",    "s,I,p",	0,    (int) M_BLTU_I,	INSN_MACRO	},
{"bltz",    "s,p",	0x04000000, 0xfc1f0000,	CBD|RD_s	},
{"bltzal",  "s,p",	0x04100000, 0xfc1f0000,	CBD|RD_s	},
{"bne",     "s,t,p",	0x14000000, 0xfc000000,	CBD|RD_s|RD_t	},
{"bne",     "s,I,p",	0,    (int) M_BNE_I,	INSN_MACRO	},
{"bnez",    "s,p",	0x14000000, 0xfc1f0000,	CBD|RD_s	},
{"break",   "",		0x0000000d, 0xffffffff,	INSN_TRAP	},
{"break",   "c",	0x0000000d, 0xfc00003f,	INSN_TRAP	},
{"c.f.d",   "S,T",	0x46200030, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.f.s",   "S,T",	0x46000030, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.un.d",  "S,T",	0x46200031, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.un.s",  "S,T",	0x46000031, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.eq.d",  "S,T",	0x46200032, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.eq.s",  "S,T",	0x46000032, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ueq.d", "S,T",	0x46200033, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ueq.s", "S,T",	0x46000033, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.olt.d", "S,T",	0x46200034, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.olt.s", "S,T",	0x46000034, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ult.d", "S,T",	0x46200035, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ult.s", "S,T",	0x46000035, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ole.d", "S,T",	0x46200036, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ole.s", "S,T",	0x46000036, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ule.d", "S,T",	0x46200037, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ule.s", "S,T",	0x46000037, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.sf.d",  "S,T",	0x46200038, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.sf.s",  "S,T",	0x46000038, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ngle.d","S,T",	0x46200039, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ngle.s","S,T",	0x46000039, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.seq.d", "S,T",	0x4620003a, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.seq.s", "S,T",	0x4600003a, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ngl.d", "S,T",	0x4620003b, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ngl.s", "S,T",	0x4600003b, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.lt.d",  "S,T",	0x4620003c, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.lt.s",  "S,T",	0x4600003c, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.nge.d", "S,T",	0x4620003d, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.nge.s", "S,T",	0x4600003d, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.le.d",  "S,T",	0x4620003e, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.le.s",  "S,T",	0x4600003e, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ngt.d", "S,T",	0x4620003f, 0xffe007ff,	RD_S|RD_T|WR_CC	},
{"c.ngt.s", "S,T",	0x4600003f, 0xffe007ff,	RD_S|RD_T|WR_CC	},
#if 0
/* these are not very safe to use, no bounds checking. */
{"c0",      "I",	0x42000000, 0xfe000000,	0		},
{"c1",      "I",	0x46000000, 0xfe000000,	0		},
{"c2",      "I",	0x4a000000, 0xfe000000,	0		},
{"c3",      "I",	0x4e000000, 0xfe000000,	0		},
#endif
{"cfc0",    "t,G",	0x40400000, 0xffe007ff,	LDD|WR_t|RD_C0	},
{"cfc1",    "t,G",	0x44400000, 0xffe007ff,	LDD|WR_t|RD_C1	},
{"cfc1",    "t,S",	0x44400000, 0xffe007ff,	LDD|WR_t|RD_C1	},
{"cfc2",    "t,G",	0x48400000, 0xffe007ff,	LDD|WR_t|RD_C2	},
{"cfc3",    "t,G",	0x4c400000, 0xffe007ff,	LDD|WR_t|RD_C3	},
{"ctc0",    "t,G",	0x40c00000, 0xffe007ff,	COD|RD_t|WR_CC	},
{"ctc1",    "t,G",	0x44c00000, 0xffe007ff,	COD|RD_t|WR_CC	},
{"ctc1",    "t,S",	0x44c00000, 0xffe007ff,	COD|RD_t|WR_CC	},
{"ctc2",    "t,G",	0x48c00000, 0xffe007ff,	COD|RD_t|WR_CC	},
{"ctc3",    "t,G",	0x4cc00000, 0xffe007ff,	COD|RD_t|WR_CC	},
{"cvt.d.s", "D,S",	0x46000021, 0xffff003f,	WR_D|RD_S	},
{"cvt.d.w", "D,S",	0x46800021, 0xffff003f,	WR_D|RD_S	},
{"cvt.s.d", "D,S",	0x46200020, 0xffff003f,	WR_D|RD_S	},
{"cvt.s.w", "D,S",	0x46800020, 0xffff003f,	WR_D|RD_S	},
{"cvt.w.d", "D,S",	0x46200024, 0xffff003f,	WR_D|RD_S	},
{"cvt.w.s", "D,S",	0x46000024, 0xffff003f,	WR_D|RD_S	},
{"div",     "s,t",	0x0000001a, 0xfc00003f,	RD_s|RD_t|WR_HI|WR_LO	},
{"div",     "d,s,t",	0,    (int) M_DIV_3,	INSN_MACRO	},
{"div",     "d,v,I",	0,    (int) M_DIV_3I,	INSN_MACRO	},
{"div.d",   "D,V,T",	0x46200003, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"div.s",   "D,V,T",	0x46000003, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"divu",    "s,t",	0x0000001b, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO	},
{"divu",    "d,s,t",	0,    (int) M_DIVU_3,	INSN_MACRO	},
{"divu",    "d,v,I",	0,    (int) M_DIVU_3I,	INSN_MACRO	},
{"jr",      "s",	0x00000008, 0xfc1fffff,	UBD|RD_s	},
{"j",       "s",	0x00000008, 0xfc1fffff,	UBD|RD_s	},
{"j",       "a",	0x08000000, 0xfc000000,	UBD		},
{"jalr",    "s",	0x0000f809, 0xfc1fffff,	UBD|RD_s|WR_d	},
{"jalr",    "d,s",	0x00000009, 0xfc1f07ff,	UBD|RD_s|WR_d	},
{"jal",     "d,s",	0x00000009, 0xfc1f07ff,	UBD|RD_s|WR_d	},/* jalr */
{"jal",     "s",	0x0000f809, 0xfc1fffff,	UBD|RD_s|WR_d	},/* jalr $ra */
{"jal",     "a",	0x0c000000, 0xfc000000,	UBD|WR_31	},
{"l.d",     "T,o(b)",	0,    (int) M_L_DOB,	INSN_MACRO	},
{"l.d",     "T,A(b)",	0,    (int) M_L_DAB,	INSN_MACRO	},
{"la",      "t,A",	0,    (int) M_LA,	INSN_MACRO	},
{"la",      "t,A(b)",	0,    (int) M_LA_AB,	INSN_MACRO	},
{"lb",      "t,o(b)",	0x80000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lb",      "t,A(b)",	0,    (int) M_LB_AB,	INSN_MACRO	},
{"lbu",     "t,o(b)",	0x90000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lbu",     "t,A(b)",	0,    (int) M_LBU_AB,	INSN_MACRO	},
{"ld",      "t,o(b)",	0,    (int) M_LD_OB,	INSN_MACRO	},
{"ld",      "t,A(b)",	0,    (int) M_LD_AB,	INSN_MACRO	},
{"lh",      "t,o(b)",	0x84000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lh",      "t,A(b)",	0,    (int) M_LH_AB,	INSN_MACRO	},
{"lhu",     "t,o(b)",	0x94000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lhu",     "t,A(b)",	0,    (int) M_LHU_AB,	INSN_MACRO	},
/* li is at the start of the table.  */
{"li.d",    "t,F",	0,    (int) M_LI_D,	INSN_MACRO	},
{"li.d",    "S,F",	0,    (int) M_LI_DD,	INSN_MACRO	},
{"lui",     "t,u",	0x3c000000, 0xffe00000,	WR_t		},
{"lw",      "t,o(b)",	0x8c000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lw",      "t,A(b)",	0,    (int) M_LW_AB,	INSN_MACRO	},
{"lwc0",    "E,o(b)",	0xc0000000, 0xfc000000,	COD|RD_b|WR_CC	},
{"lwc0",    "E,A(b)",	0,    (int) M_LWC0_AB,	INSN_MACRO	},
{"lwc1",    "T,o(b)",	0xc4000000, 0xfc000000,	COD|RD_b|WR_T	},
{"lwc1",    "E,o(b)",	0xc4000000, 0xfc000000,	COD|RD_b|WR_T	},
{"lwc1",    "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"l.s",     "T,o(b)",	0xc4000000, 0xfc000000,	COD|RD_b|WR_T	}, /* lwc1 */
{"l.s",     "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"lwc2",    "E,o(b)",	0xc8000000, 0xfc000000,	COD|RD_b|WR_CC	},
{"lwc2",    "E,A(b)",	0,    (int) M_LWC2_AB,	INSN_MACRO	},
{"lwc3",    "E,o(b)",	0xcc000000, 0xfc000000,	COD|RD_b|WR_CC	},
{"lwc3",    "E,A(b)",	0,    (int) M_LWC3_AB,	INSN_MACRO	},
{"lwl",     "t,o(b)",	0x88000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lwl",     "t,A(b)",	0,    (int) M_LWL_AB,	INSN_MACRO	},
{"lwr",     "t,o(b)",	0x98000000, 0xfc000000,	LDD|RD_b|WR_t	},
{"lwr",     "t,A(b)",	0,    (int) M_LWR_AB,	INSN_MACRO	},
{"mfc0",    "t,G",	0x40000000, 0xffe007ff,	LDD|WR_t|RD_C0	},
{"mfc1",    "t,S",	0x44000000, 0xffe007ff,	LDD|WR_t|RD_S	},
{"mfc1",    "t,G",	0x44000000, 0xffe007ff,	LDD|WR_t|RD_S	},
{"mfc2",    "t,G",	0x48000000, 0xffe007ff,	LDD|WR_t|RD_C2	},
{"mfc3",    "t,G",	0x4c000000, 0xffe007ff,	LDD|WR_t|RD_C3	},
{"mfhi",    "d",	0x00000010, 0xffff07ff,	WR_d|RD_HI	},
{"mflo",    "d",	0x00000012, 0xffff07ff,	WR_d|RD_LO	},
{"mov.d",   "D,S",	0x46200006, 0xffff003f,	WR_D|RD_S	},
{"mov.s",   "D,S",	0x46000006, 0xffff003f,	WR_D|RD_S	},
/* move is at the top of the table.  */
{"mtc0",    "t,G",	0x40800000, 0xffe007ff,	COD|RD_t|WR_C0|WR_CC	},
{"mtc1",    "t,S",	0x44800000, 0xffe007ff,	COD|RD_t|WR_D	},
{"mtc1",    "t,G",	0x44800000, 0xffe007ff,	COD|RD_t|WR_D	},
{"mtc2",    "t,G",	0x48800000, 0xffe007ff,	COD|RD_t|WR_C2|WR_CC	},
{"mtc3",    "t,G",	0x4c800000, 0xffe007ff,	COD|RD_t|WR_C3|WR_CC	},
{"mthi",    "s",	0x00000011, 0xfc1fffff,	RD_s|WR_HI	},
{"mtlo",    "s",	0x00000013, 0xfc1fffff,	RD_s|WR_LO	},
{"mul.d",   "D,V,T",	0x46200002, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"mul.s",   "D,V,T",	0x46000002, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"mul",     "d,v,t",	0,    (int) M_MUL,	INSN_MACRO	},
{"mul",     "d,v,I",	0,    (int) M_MUL_I,	INSN_MACRO	},
{"mulo",    "d,v,t",	0,    (int) M_MULO,	INSN_MACRO	},
{"mulo",    "d,v,I",	0,    (int) M_MULO_I,	INSN_MACRO	},
{"mulou",   "d,v,t",	0,    (int) M_MULOU,	INSN_MACRO	},
{"mulou",   "d,v,I",	0,    (int) M_MULOU_I,	INSN_MACRO	},
{"mult",    "s,t",	0x00000018, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO	},
{"multu",   "s,t",	0x00000019, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO	},
{"neg",     "d,w",	0x00000022, 0xffe007ff,	WR_d|RD_t	}, /* sub 0 */
{"negu",    "d,w",	0x00000023, 0xffe007ff,	WR_d|RD_t	}, /* subu 0 */
{"neg.d",   "D,V",	0x46200007, 0xffff003f,	WR_D|RD_S	},
{"neg.s",   "D,V",	0x46000007, 0xffff003f,	WR_D|RD_S	},
/* nop is at the start of the table.  */
{"nor",     "d,v,t",	0x00000027, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"nor",     "d,v,I",	0,    (int) M_NOR_I,	INSN_MACRO	},
{"not",     "d,v",	0x00000027, 0xfc0007ff,	WR_d|RD_s|RD_t	}, /* nor d,s,zero */
{"or",      "d,v,t",	0x00000025, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"or",      "t,r,I",	0,    (int) M_OR_I,	INSN_MACRO	},
{"ori",     "t,r,i",	0x34000000, 0xfc000000,	WR_t|RD_s	},
{"rem",     "d,v,t",	0,    (int) M_REM_3,	INSN_MACRO	},
{"rem",     "d,v,I",	0,    (int) M_REM_3I,	INSN_MACRO	},
{"remu",    "d,v,t",	0,    (int) M_REMU_3,	INSN_MACRO	},
{"remu",    "d,v,I",	0,    (int) M_REMU_3I,	INSN_MACRO	},
{"rfe",     "",		0x42000010, 0xffffffff,	INSN_RFE	},
{"rol",     "d,v,t",	0,    (int) M_ROL,	INSN_MACRO	},
{"rol",     "d,v,I",	0,    (int) M_ROL_I,	INSN_MACRO	},
{"ror",     "d,v,t",	0,    (int) M_ROR,	INSN_MACRO	},
{"ror",     "d,v,I",	0,    (int) M_ROR_I,	INSN_MACRO	},
{"s.d",     "T,o(b)",	0,    (int) M_S_DOB,	INSN_MACRO	},
{"s.d",     "T,A(b)",	0,    (int) M_S_DAB,	INSN_MACRO	},
{"sb",      "t,o(b)",	0xa0000000, 0xfc000000,	RD_t|RD_b	},
{"sb",      "t,A(b)",	0,    (int) M_SB_AB,	INSN_MACRO	},
{"sd",      "t,o(b)",	0,    (int) M_SD_OB,	INSN_MACRO	},
{"sd",      "t,A(b)",	0,    (int) M_SD_AB,	INSN_MACRO	},
{"seq",     "d,v,t",	0,    (int) M_SEQ,	INSN_MACRO	},
{"seq",     "d,v,I",	0,    (int) M_SEQ_I,	INSN_MACRO	},
{"sge",     "d,v,t",	0,    (int) M_SGE,	INSN_MACRO	},
{"sge",     "d,v,I",	0,    (int) M_SGE_I,	INSN_MACRO	},
{"sgeu",    "d,v,t",	0,    (int) M_SGEU,	INSN_MACRO	},
{"sgeu",    "d,v,I",	0,    (int) M_SGEU_I,	INSN_MACRO	},
{"sgt",     "d,v,t",	0,    (int) M_SGT,	INSN_MACRO	},
{"sgt",     "d,v,I",	0,    (int) M_SGT_I,	INSN_MACRO	},
{"sgtu",    "d,v,t",	0,    (int) M_SGTU,	INSN_MACRO	},
{"sgtu",    "d,v,I",	0,    (int) M_SGTU_I,	INSN_MACRO	},
{"sh",      "t,o(b)",	0xa4000000, 0xfc000000,	RD_t|RD_b	},
{"sh",      "t,A(b)",	0,    (int) M_SH_AB,	INSN_MACRO	},
{"sle",     "d,v,t",	0,    (int) M_SLE,	INSN_MACRO	},
{"sle",     "d,v,I",	0,    (int) M_SLE_I,	INSN_MACRO	},
{"sleu",    "d,v,t",	0,    (int) M_SLEU,	INSN_MACRO	},
{"sleu",    "d,v,I",	0,    (int) M_SLEU_I,	INSN_MACRO	},
{"sllv",    "d,t,s",	0x00000004, 0xfc0007ff,	WR_d|RD_t|RD_s	},
{"sll",     "d,w,s",	0x00000004, 0xfc0007ff,	WR_d|RD_t|RD_s	}, /* sllv */
{"sll",     "d,w,<",	0x00000000, 0xffe0003f,	WR_d|RD_t	},
{"slt",     "d,v,t",	0x0000002a, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"slt",     "d,v,I",	0,    (int) M_SLT_I,	INSN_MACRO	},
{"slti",    "t,r,j",	0x28000000, 0xfc000000,	WR_t|RD_s	},
{"sltiu",   "t,r,j",	0x2c000000, 0xfc000000,	WR_t|RD_s	},
{"sltu",    "d,v,t",	0x0000002b, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"sltu",    "d,v,I",	0,    (int) M_SLTU_I,	INSN_MACRO	},
{"sne",     "d,v,t",	0,    (int) M_SNE,	INSN_MACRO	},
{"sne",     "d,v,I",	0,    (int) M_SNE_I,	INSN_MACRO	},
{"srav",    "d,t,s",	0x00000007, 0xfc0007ff,	WR_d|RD_t|RD_s	},
{"sra",     "d,w,s",	0x00000007, 0xfc0007ff,	WR_d|RD_t|RD_s	}, /* srav */
{"sra",     "d,w,<",	0x00000003, 0xffe0003f,	WR_d|RD_t	},
{"srlv",    "d,t,s",	0x00000006, 0xfc0007ff,	WR_d|RD_t|RD_s	},
{"srl",     "d,w,s",	0x00000006, 0xfc0007ff,	WR_d|RD_t|RD_s	}, /* srlv */
{"srl",     "d,w,<",	0x00000002, 0xffe0003f,	WR_d|RD_t	},
{"sub",     "d,v,t",	0x00000022, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"sub",     "d,v,I",	0,    (int) M_SUB_I,	INSN_MACRO	},
{"sub.d",   "D,V,T",	0x46200001, 0xffe0003f,	WR_D|RD_S|RD_T	},     
{"sub.s",   "D,V,T",	0x46000001, 0xffe0003f,	WR_D|RD_S|RD_T	},
{"subu",    "d,v,t",	0x00000023, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"subu",    "d,v,I",	0,    (int) M_SUBU_I,	INSN_MACRO	},
{"sw",      "t,o(b)",	0xac000000, 0xfc000000,	RD_t|RD_b	},
{"sw",      "t,A(b)",	0,    (int) M_SW_AB,	INSN_MACRO	},
{"swc0",    "E,o(b)",	0xe0000000, 0xfc000000,	RD_C0|RD_b	},
{"swc0",    "E,A(b)",	0,    (int) M_SWC0_AB,	INSN_MACRO	},
{"swc1",    "T,o(b)",	0xe4000000, 0xfc000000,	RD_T|RD_b	},
{"swc1",    "E,o(b)",	0xe4000000, 0xfc000000,	RD_T|RD_b	},
{"swc1",    "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"s.s",     "T,o(b)",	0xe4000000, 0xfc000000,	RD_T|RD_b	}, /* swc1 */
{"s.s",     "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"swc2",    "E,o(b)",	0xe8000000, 0xfc000000,	RD_C2|RD_b	},
{"swc2",    "E,A(b)",	0,    (int) M_SWC2_AB,	INSN_MACRO	},
{"swc3",    "E,o(b)",	0xec000000, 0xfc000000,	RD_C3|RD_b	},
{"swc3",    "E,A(b)",	0,    (int) M_SWC3_AB,	INSN_MACRO	},
{"swl",     "t,o(b)",	0xa8000000, 0xfc000000,	RD_t|RD_b	},
{"swl",     "t,A(b)",	0,    (int) M_SWL_AB,	INSN_MACRO	},
{"swr",     "t,o(b)",	0xb8000000, 0xfc000000,	RD_t|RD_b	},
{"swr",     "t,A(b)",	0,    (int) M_SWR_AB,	INSN_MACRO	},
{"syscall", "",		0x0000000c, 0xffffffff,	INSN_TRAP	},
{"syscall", "B",	0x0000000c, 0xfc00003f,	INSN_TRAP	},
{"tlbp",    "",		0x42000008, 0xffffffff,	INSN_TLB	},
{"tlbr",    "",		0x42000001, 0xffffffff,	INSN_TLB	},
{"tlbwi",   "",		0x42000002, 0xffffffff,	INSN_TLB	},
{"tlbwr",   "",		0x42000006, 0xffffffff,	INSN_TLB	},
{"trunc.w.d", "D,S,t",	0,    (int) M_TRUNCWD,	INSN_MACRO	},
{"trunc.w.s", "D,S,t",	0,    (int) M_TRUNCWS,	INSN_MACRO	},
{"ulh",     "t,o(b)",	0,    (int) M_ULH,	INSN_MACRO	},
{"ulh",     "t,A",	0,    (int) M_ULH_A,	INSN_MACRO	},
{"ulhu",    "t,o(b)",	0,    (int) M_ULHU,	INSN_MACRO	},
{"ulhu",    "t,A",	0,    (int) M_ULHU_A,	INSN_MACRO	},
{"ulw",     "t,o(b)",	0,    (int) M_ULW,	INSN_MACRO	},
{"ulw",     "t,A",	0,    (int) M_ULW_A,	INSN_MACRO	},
{"ush",     "t,o(b)",	0,    (int) M_USH,	INSN_MACRO	},
{"ush",     "t,A",	0,    (int) M_USH_A,	INSN_MACRO	},
{"usw",     "t,o(b)",	0,    (int) M_USW,	INSN_MACRO	},
{"usw",     "t,A",	0,    (int) M_USW_A,	INSN_MACRO	},
{"xor",     "d,v,t",	0x00000026, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"xor",     "t,r,I",	0,    (int) M_XOR_I,	INSN_MACRO	},
{"xori",    "t,r,i",	0x38000000, 0xfc000000,	WR_t|RD_s	},
};

#define NUMOPCODES          (sizeof(mips_opcodes)/sizeof(*mips_opcodes))
