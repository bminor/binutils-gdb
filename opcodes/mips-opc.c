/* mips.h.  Mips opcode list for GDB, the GNU debugger.
   Copyright 1993, 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
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
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/mips.h"

/* Short hand so the lines aren't too long.  */

#define LDD     INSN_LOAD_MEMORY_DELAY
#define LCD	INSN_LOAD_COPROC_DELAY
#define UBD     INSN_UNCOND_BRANCH_DELAY
#define CBD	INSN_COND_BRANCH_DELAY
#define COD     INSN_COPROC_MOVE_DELAY
#define CLD	INSN_COPROC_MEMORY_DELAY
#define CBL	INSN_COND_BRANCH_LIKELY
#define TRAP	INSN_TRAP
#define SM	INSN_STORE_MEMORY

#define WR_d    INSN_WRITE_GPR_D
#define WR_t    INSN_WRITE_GPR_T
#define WR_31   INSN_WRITE_GPR_31       
#define WR_D    INSN_WRITE_FPR_D        
#define WR_T	INSN_WRITE_FPR_T
#define WR_S	INSN_WRITE_FPR_S
#define RD_s    INSN_READ_GPR_S         
#define RD_b    INSN_READ_GPR_S         
#define RD_t    INSN_READ_GPR_T         
#define RD_S    INSN_READ_FPR_S         
#define RD_T    INSN_READ_FPR_T         
#define RD_R	INSN_READ_FPR_R
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

#define I1	INSN_ISA1
#define I2	INSN_ISA2
#define I3	INSN_ISA3
#define I4	INSN_ISA4
#define P3	INSN_4650
#define L1	INSN_4010
#define V1      INSN_4100
#define T3      INSN_3900

/* start-sanitize-r5900 */
#define T5	INSN_5900
/* end-sanitize-r5900 */

#define G1      (T3                   \
/* start-sanitize-r5900 */            \
                 | T5                 \
/* end-sanitize-r5900 */              \
                 )


/* The order of overloaded instructions matters.  Label arguments and
   register arguments look the same. Instructions that can have either
   for arguments must apear in the correct order in this table for the
   assembler to pick the right one. In other words, entries with
   immediate operands must apear after the same instruction with
   registers.

   Many instructions are short hand for other instructions (i.e., The
   jal <register> instruction is short for jalr <register>).  */

const struct mips_opcode mips_builtin_opcodes[] = {
/* These instructions appear first so that the disassembler will find
   them first.  The assemblers uses a hash table based on the
   instruction name anyhow.  */
/* name,	args,	mask,		match,	pinfo */
{"nop",     "",		0x00000000, 0xffffffff,	0,		I1	},
{"li",      "t,j",      0x24000000, 0xffe00000, WR_t,		I1	}, /* addiu */
{"li",	    "t,i",	0x34000000, 0xffe00000, WR_t,		I1	}, /* ori */
{"li",      "t,I",	0,    (int) M_LI,	INSN_MACRO		},
{"move",    "d,s",	0x0000002d, 0xfc1f07ff, WR_d|RD_s,	I3	},/* daddu */
{"move",    "d,s",	0x00000021, 0xfc1f07ff, WR_d|RD_s,	I1	},/* addu */
{"move",    "d,s",	0x00000025, 0xfc1f07ff,	WR_d|RD_s,	I1	},/* or */
{"b",       "p",	0x10000000, 0xffff0000,	UBD,		I1	},/* beq 0,0 */
{"b",       "p",	0x04010000, 0xffff0000,	UBD,		I1	},/* bgez 0 */
{"bal",     "p",	0x04110000, 0xffff0000,	UBD|WR_31,	I1	},/* bgezal 0*/

{"abs",     "d,v",	0,    (int) M_ABS,	INSN_MACRO		},
{"abs.s",   "D,V",	0x46000005, 0xffff003f,	WR_D|RD_S|FP_S,	I1	},
{"abs.d",   "D,V",	0x46200005, 0xffff003f,	WR_D|RD_S|FP_D,	I1	},
{"add",     "d,v,t",	0x00000020, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"add",     "t,r,I",	0,    (int) M_ADD_I,	INSN_MACRO		},
{"add.s",   "D,V,T",	0x46000000, 0xffe0003f,	WR_D|RD_S|RD_T|FP_S,	I1},
{"add.d",   "D,V,T",	0x46200000, 0xffe0003f,	WR_D|RD_S|RD_T|FP_D,	I1},
{"addi",    "t,r,j",	0x20000000, 0xfc000000,	WR_t|RD_s,	I1	},
{"addiu",   "t,r,j",	0x24000000, 0xfc000000,	WR_t|RD_s,	I1	},
{"addu",    "d,v,t",	0x00000021, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"addu",    "t,r,I",	0,    (int) M_ADDU_I,	INSN_MACRO		},
{"and",     "d,v,t",	0x00000024, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"and",     "t,r,I",	0,    (int) M_AND_I,	INSN_MACRO		},
{"andi",    "t,r,i",	0x30000000, 0xfc000000,	WR_t|RD_s,	I1	},
/* b is at the top of the table.  */
/* bal is at the top of the table.  */
{"bc0f",    "p",	0x41000000, 0xffff0000,	CBD|RD_CC,	I1	},
{"bc0fl",   "p",	0x41020000, 0xffff0000,	CBL|RD_CC,	I2|T3	},
{"bc1f",    "p",	0x45000000, 0xffff0000,	CBD|RD_CC|FP_S,	I1	},
{"bc1f",    "N,p",	0x45000000, 0xffe30000,	CBD|RD_CC|FP_S,	I4	},
{"bc1fl",   "p",	0x45020000, 0xffff0000,	CBL|RD_CC|FP_S,	I2|T3	},
{"bc1fl",   "N,p",	0x45020000, 0xffe30000,	CBL|RD_CC|FP_S,	I4	},
{"bc2f",    "p",	0x49000000, 0xffff0000,	CBD|RD_CC,	I1	},
{"bc2fl",   "p",	0x49020000, 0xffff0000,	CBL|RD_CC,	I2|T3	},
{"bc3f",    "p",	0x4d000000, 0xffff0000,	CBD|RD_CC,	I1	},
{"bc3fl",   "p",	0x4d020000, 0xffff0000,	CBL|RD_CC,	I2|T3	},
{"bc0t",    "p",	0x41010000, 0xffff0000,	CBD|RD_CC,	I1	},
{"bc0tl",   "p",	0x41030000, 0xffff0000,	CBL|RD_CC,	I2|T3	},
{"bc1t",    "p",	0x45010000, 0xffff0000,	CBD|RD_CC|FP_S,	I1	},
{"bc1t",    "N,p",	0x45010000, 0xffe30000,	CBD|RD_CC|FP_S,	I4	},
{"bc1tl",   "p",	0x45030000, 0xffff0000,	CBL|RD_CC|FP_S,	I2|T3	},
{"bc1tl",   "N,p",	0x45030000, 0xffe30000,	CBL|RD_CC|FP_S,	I4	},
{"bc2t",    "p",	0x49010000, 0xffff0000,	CBD|RD_CC,	I1	},
{"bc2tl",   "p",	0x49030000, 0xffff0000,	CBL|RD_CC,	I2|T3	},
{"bc3t",    "p",	0x4d010000, 0xffff0000,	CBD|RD_CC,	I1	},
{"bc3tl",   "p",	0x4d030000, 0xffff0000,	CBL|RD_CC,	I2|T3	},
{"beqz",    "s,p",	0x10000000, 0xfc1f0000,	CBD|RD_s,	I1	},
{"beqzl",   "s,p",	0x50000000, 0xfc1f0000,	CBL|RD_s,	I2	},
{"beq",     "s,t,p",	0x10000000, 0xfc000000,	CBD|RD_s|RD_t,	I1	},
{"beq",     "s,I,p",	0,    (int) M_BEQ_I,	INSN_MACRO		},
{"beql",    "s,t,p",	0x50000000, 0xfc000000,	CBL|RD_s|RD_t,	I2|T3	},
{"beql",    "s,I,p",	2,    (int) M_BEQL_I,	INSN_MACRO		},
{"bge",     "s,t,p",	0,    (int) M_BGE,	INSN_MACRO		},
{"bge",     "s,I,p",	0,    (int) M_BGE_I,	INSN_MACRO		},
{"bgel",    "s,t,p",	2,    (int) M_BGEL,	INSN_MACRO		},
{"bgel",    "s,I,p",	2,    (int) M_BGEL_I,	INSN_MACRO		},
{"bgeu",    "s,t,p",	0,    (int) M_BGEU,	INSN_MACRO		},
{"bgeu",    "s,I,p",	0,    (int) M_BGEU_I,	INSN_MACRO		},
{"bgeul",   "s,t,p",	2,    (int) M_BGEUL,	INSN_MACRO		},
{"bgeul",   "s,I,p",	2,    (int) M_BGEUL_I,	INSN_MACRO		},
{"bgez",    "s,p",	0x04010000, 0xfc1f0000,	CBD|RD_s,	I1	},
{"bgezl",   "s,p",	0x04030000, 0xfc1f0000,	CBL|RD_s,	I2|T3	},
{"bgezal",  "s,p",	0x04110000, 0xfc1f0000,	CBD|RD_s|WR_31,	I1	},
{"bgezall", "s,p",	0x04130000, 0xfc1f0000,	CBL|RD_s,	I2|T3	},
{"bgt",     "s,t,p",	0,    (int) M_BGT,	INSN_MACRO		},
{"bgt",     "s,I,p",	0,    (int) M_BGT_I,	INSN_MACRO		},
{"bgtl",    "s,t,p",	2,    (int) M_BGTL,	INSN_MACRO		},
{"bgtl",    "s,I,p",	2,    (int) M_BGTL_I,	INSN_MACRO		},
{"bgtu",    "s,t,p",	0,    (int) M_BGTU,	INSN_MACRO		},
{"bgtu",    "s,I,p",	0,    (int) M_BGTU_I,	INSN_MACRO		},
{"bgtul",   "s,t,p",	2,    (int) M_BGTUL,	INSN_MACRO		},
{"bgtul",   "s,I,p",	2,    (int) M_BGTUL_I,	INSN_MACRO		},
{"bgtz",    "s,p",	0x1c000000, 0xfc1f0000,	CBD|RD_s,	I1	},
{"bgtzl",   "s,p",	0x5c000000, 0xfc1f0000,	CBL|RD_s,	I2|T3	},
{"ble",     "s,t,p",	0,    (int) M_BLE,	INSN_MACRO		},
{"ble",     "s,I,p",	0,    (int) M_BLE_I,	INSN_MACRO		},
{"blel",    "s,t,p",	2,    (int) M_BLEL,	INSN_MACRO		},
{"blel",    "s,I,p",	2,    (int) M_BLEL_I,	INSN_MACRO		},
{"bleu",    "s,t,p",	0,    (int) M_BLEU,	INSN_MACRO		},
{"bleu",    "s,I,p",	0,    (int) M_BLEU_I,	INSN_MACRO		},
{"bleul",   "s,t,p",	2,    (int) M_BLEUL,	INSN_MACRO		},
{"bleul",   "s,I,p",	2,    (int) M_BLEUL_I,	INSN_MACRO		},
{"blez",    "s,p",	0x18000000, 0xfc1f0000,	CBD|RD_s,	I1	},
{"blezl",   "s,p",	0x58000000, 0xfc1f0000,	CBL|RD_s,	I2|T3	},
{"blt",     "s,t,p",	0,    (int) M_BLT,	INSN_MACRO		},
{"blt",     "s,I,p",	0,    (int) M_BLT_I,	INSN_MACRO		},
{"bltl",    "s,t,p",	2,    (int) M_BLTL,	INSN_MACRO		},
{"bltl",    "s,I,p",	2,    (int) M_BLTL_I,	INSN_MACRO		},
{"bltu",    "s,t,p",	0,    (int) M_BLTU,	INSN_MACRO		},
{"bltu",    "s,I,p",	0,    (int) M_BLTU_I,	INSN_MACRO		},
{"bltul",   "s,t,p",	2,    (int) M_BLTUL,	INSN_MACRO		},
{"bltul",   "s,I,p",	2,    (int) M_BLTUL_I,	INSN_MACRO		},
{"bltz",    "s,p",	0x04000000, 0xfc1f0000,	CBD|RD_s,	I1	},
{"bltzl",   "s,p",	0x04020000, 0xfc1f0000,	CBL|RD_s,	I2|T3	},
{"bltzal",  "s,p",	0x04100000, 0xfc1f0000,	CBD|RD_s|WR_31,	I1	},
{"bltzall", "s,p",	0x04120000, 0xfc1f0000,	CBL|RD_s,	I2|T3	},
{"bnez",    "s,p",	0x14000000, 0xfc1f0000,	CBD|RD_s,	I1	},
{"bnezl",   "s,p",	0x54000000, 0xfc1f0000,	CBL|RD_s,	I2	},
{"bne",     "s,t,p",	0x14000000, 0xfc000000,	CBD|RD_s|RD_t,	I1	},
{"bne",     "s,I,p",	0,    (int) M_BNE_I,	INSN_MACRO		},
{"bnel",    "s,t,p",	0x54000000, 0xfc000000,	CBL|RD_s|RD_t, 	I2|T3	},
{"bnel",    "s,I,p",	2,    (int) M_BNEL_I,	INSN_MACRO		},
{"break",   "",		0x0000000d, 0xffffffff,	TRAP,		I1	},
{"break",   "c",	0x0000000d, 0xfc00003f,	TRAP,		I1	},
{"c.f.d",   "S,T",	0x46200030, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.f.d",   "M,S,T",	0x46200030, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.f.s",   "S,T",	0x46000030, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.f.s",   "M,S,T",	0x46000030, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.un.d",  "S,T",	0x46200031, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.un.d",  "M,S,T",	0x46200031, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.un.s",  "S,T",	0x46000031, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.un.s",  "M,S,T",	0x46000031, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.eq.d",  "S,T",	0x46200032, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.eq.d",  "M,S,T",	0x46200032, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.eq.s",  "S,T",	0x46000032, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.eq.s",  "M,S,T",	0x46000032, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ueq.d", "S,T",	0x46200033, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ueq.d", "M,S,T",	0x46200033, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ueq.s", "S,T",	0x46000033, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ueq.s", "M,S,T",	0x46000033, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.olt.d", "S,T",	0x46200034, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.olt.d", "M,S,T",	0x46200034, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.olt.s", "S,T",	0x46000034, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.olt.s", "M,S,T",	0x46000034, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ult.d", "S,T",	0x46200035, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ult.d", "M,S,T",	0x46200035, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ult.s", "S,T",	0x46000035, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ult.s", "M,S,T",	0x46000035, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ole.d", "S,T",	0x46200036, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ole.d", "M,S,T",	0x46200036, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ole.s", "S,T",	0x46000036, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ole.s", "M,S,T",	0x46000036, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ule.d", "S,T",	0x46200037, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ule.d", "M,S,T",	0x46200037, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ule.s", "S,T",	0x46000037, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ule.s", "M,S,T",	0x46000037, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.sf.d",  "S,T",	0x46200038, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.sf.d",  "M,S,T",	0x46200038, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.sf.s",  "S,T",	0x46000038, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.sf.s",  "M,S,T",	0x46000038, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ngle.d","S,T",	0x46200039, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ngle.d","M,S,T",	0x46200039, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ngle.s","S,T",	0x46000039, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ngle.s","M,S,T",	0x46000039, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.seq.d", "S,T",	0x4620003a, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.seq.d", "M,S,T",	0x4620003a, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.seq.s", "S,T",	0x4600003a, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.seq.s", "M,S,T",	0x4600003a, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ngl.d", "S,T",	0x4620003b, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ngl.d", "M,S,T",	0x4620003b, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ngl.s", "S,T",	0x4600003b, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ngl.s", "M,S,T",	0x4600003b, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.lt.d",  "S,T",	0x4620003c, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.lt.d",  "M,S,T",	0x4620003c, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.lt.s",  "S,T",	0x4600003c, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.lt.s",  "M,S,T",	0x4600003c, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.nge.d", "S,T",	0x4620003d, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.nge.d", "M,S,T",	0x4620003d, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.nge.s", "S,T",	0x4600003d, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.nge.s", "M,S,T",	0x4600003d, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.le.d",  "S,T",	0x4620003e, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.le.d",  "M,S,T",	0x4620003e, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.le.s",  "S,T",	0x4600003e, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.le.s",  "M,S,T",	0x4600003e, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"c.ngt.d", "S,T",	0x4620003f, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_D,	I1	},
{"c.ngt.d", "M,S,T",	0x4620003f, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_D,	I4	},
{"c.ngt.s", "S,T",	0x4600003f, 0xffe007ff,	RD_S|RD_T|WR_CC|FP_S,	I1	},
{"c.ngt.s", "M,S,T",	0x4600003f, 0xffe000ff,	RD_S|RD_T|WR_CC|FP_S,	I4	},
{"cache",   "k,o(b)",	0xbc000000, 0xfc000000, RD_b,		I3	},
{"ceil.l.d", "D,S",	0x4620000a, 0xffff003f, WR_D|RD_S|FP_D,	I3	},
{"ceil.l.s", "D,S",	0x4600000a, 0xffff003f, WR_D|RD_S|FP_S,	I3	},
{"ceil.w.d", "D,S",	0x4620000e, 0xffff003f, WR_D|RD_S|FP_D,	I2	},
{"ceil.w.s", "D,S",	0x4600000e, 0xffff003f, WR_D|RD_S|FP_S,	I2	},
{"cfc0",    "t,G",	0x40400000, 0xffe007ff,	LCD|WR_t|RD_C0,	I1	},
{"cfc1",    "t,G",	0x44400000, 0xffe007ff,	LCD|WR_t|RD_C1|FP_S,	I1	},
{"cfc1",    "t,S",	0x44400000, 0xffe007ff,	LCD|WR_t|RD_C1|FP_S,	I1	},
{"cfc2",    "t,G",	0x48400000, 0xffe007ff,	LCD|WR_t|RD_C2,	I1	},
{"cfc3",    "t,G",	0x4c400000, 0xffe007ff,	LCD|WR_t|RD_C3,	I1	},
{"ctc0",    "t,G",	0x40c00000, 0xffe007ff,	COD|RD_t|WR_CC,	I1	},
{"ctc1",    "t,G",	0x44c00000, 0xffe007ff,	COD|RD_t|WR_CC|FP_S,	I1	},
{"ctc1",    "t,S",	0x44c00000, 0xffe007ff,	COD|RD_t|WR_CC|FP_S,	I1	},
{"ctc2",    "t,G",	0x48c00000, 0xffe007ff,	COD|RD_t|WR_CC,	I1	},
{"ctc3",    "t,G",	0x4cc00000, 0xffe007ff,	COD|RD_t|WR_CC,	I1	},
{"cvt.d.l", "D,S",	0x46a00021, 0xffff003f,	WR_D|RD_S|FP_D,	I3	},
{"cvt.d.s", "D,S",	0x46000021, 0xffff003f,	WR_D|RD_S|FP_D|FP_S,	I1	},
{"cvt.d.w", "D,S",	0x46800021, 0xffff003f,	WR_D|RD_S|FP_D,	I1	},
{"cvt.l.d", "D,S",	0x46200025, 0xffff003f,	WR_D|RD_S|FP_D,	I3	},
{"cvt.l.s", "D,S",	0x46000025, 0xffff003f,	WR_D|RD_S|FP_S,	I3	},
{"cvt.s.l", "D,S",	0x46a00020, 0xffff003f,	WR_D|RD_S|FP_S,	I3	},
{"cvt.s.d", "D,S",	0x46200020, 0xffff003f,	WR_D|RD_S|FP_S|FP_D,	I1	},
{"cvt.s.w", "D,S",	0x46800020, 0xffff003f,	WR_D|RD_S|FP_S,	I1	},
{"cvt.w.d", "D,S",	0x46200024, 0xffff003f,	WR_D|RD_S|FP_D,	I1	},
{"cvt.w.s", "D,S",	0x46000024, 0xffff003f,	WR_D|RD_S|FP_S,	I1	},
{"dabs",    "d,v",	3,    (int) M_DABS,	INSN_MACRO	},
{"dadd",    "d,v,t",	0x0000002c, 0xfc0007ff, WR_d|RD_s|RD_t,	I3	},
{"dadd",    "t,r,I",	3,    (int) M_DADD_I,	INSN_MACRO	},
{"daddi",   "t,r,j",	0x60000000, 0xfc000000, WR_t|RD_s,	I3	},
{"daddiu",  "t,r,j",	0x64000000, 0xfc000000, WR_t|RD_s,	I3	},
{"daddu",   "d,v,t",	0x0000002d, 0xfc0007ff, WR_d|RD_s|RD_t,	I3	},
{"daddu",   "t,r,I",	3,    (int) M_DADDU_I,	INSN_MACRO	},
/* dctr and dctw are used on the r5000.  */
{"dctr",    "o(b)",	0xbc050000, 0xfc1f0000, RD_b,	I3	},
{"dctw",    "o(b)",	0xbc090000, 0xfc1f0000, RD_b,	I3	},
{"deret",   "",         0x4200001f, 0xffffffff,    0,	T3	},
/* For ddiv, see the comments about div.  */
{"ddiv",    "z,s,t",	0x0000001e, 0xfc00ffff, RD_s|RD_t|WR_HI|WR_LO,	I3	},
{"ddiv",    "d,v,t",	3,    (int) M_DDIV_3,	INSN_MACRO	},
{"ddiv",    "d,v,I",	3,    (int) M_DDIV_3I,	INSN_MACRO	},
/* For ddivu, see the comments about div.  */
{"ddivu",   "z,s,t",	0x0000001f, 0xfc00ffff, RD_s|RD_t|WR_HI|WR_LO,	I3	},
{"ddivu",   "d,v,t",	3,    (int) M_DDIVU_3,	INSN_MACRO	},
{"ddivu",   "d,v,I",	3,    (int) M_DDIVU_3I,	INSN_MACRO	},
/* The MIPS assembler treats the div opcode with two operands as
   though the first operand appeared twice (the first operand is both
   a source and a destination).  To get the div machine instruction,
   you must use an explicit destination of $0.  */
{"div",     "z,s,t",	0x0000001a, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	I1	},
{"div",     "z,t",	0x0000001a, 0xffe0ffff,	RD_s|RD_t|WR_HI|WR_LO,	I1	},
{"div",     "d,v,t",	0,    (int) M_DIV_3,	INSN_MACRO	},
{"div",     "d,v,I",	0,    (int) M_DIV_3I,	INSN_MACRO	},
  /* start-sanitize-r5900 */
{"div1",    "s,t",	0x7000001a, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	T5	},
  /* end-sanitize-r5900 */
{"div.d",   "D,V,T",	0x46200003, 0xffe0003f,	WR_D|RD_S|RD_T|FP_D,	I1	},
{"div.s",   "D,V,T",	0x46000003, 0xffe0003f,	WR_D|RD_S|RD_T|FP_S,	I1	},
/* For divu, see the comments about div.  */
{"divu",    "z,s,t",	0x0000001b, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	I1	},
{"divu",    "z,t",	0x0000001b, 0xffe0ffff,	RD_s|RD_t|WR_HI|WR_LO,	I1	},
{"divu",    "d,v,t",	0,    (int) M_DIVU_3,	INSN_MACRO	},
{"divu",    "d,v,I",	0,    (int) M_DIVU_3I,	INSN_MACRO	},
  /* start-sanitize-r5900 */
{"divu1",   "s,t",	0x7000001b, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	T5	},
  /* end-sanitize-r5900 */
{"dla",     "t,A(b)",	3,    (int) M_DLA_AB,	INSN_MACRO	},
{"dli",     "t,j",      0x24000000, 0xffe00000, WR_t,	I3	}, /* addiu */
{"dli",	    "t,i",	0x34000000, 0xffe00000, WR_t,	I3	}, /* ori */
{"dli",     "t,I",	3,    (int) M_DLI,	INSN_MACRO	},
{"dmadd16", "s,t",      0x00000029, 0xfc00ffff, RD_s|RD_t|WR_LO|RD_LO,	V1	},
{"dmfc0",   "t,G",	0x40200000, 0xffe007ff, LCD|WR_t|RD_C0,	I3	},
{"dmtc0",   "t,G",	0x40a00000, 0xffe007ff, COD|RD_t|WR_C0|WR_CC,	I3	},
{"dmfc1",   "t,S",	0x44200000, 0xffe007ff, LCD|WR_t|RD_S|FP_S,	I3	},
{"dmtc1",   "t,S",	0x44a00000, 0xffe007ff, COD|RD_t|WR_S|FP_S,	I3	},
{"dmul",    "d,v,t",	3,    (int) M_DMUL,	INSN_MACRO	},
{"dmul",    "d,v,I",	3,    (int) M_DMUL_I,	INSN_MACRO	},
{"dmulo",   "d,v,t",	3,    (int) M_DMULO,	INSN_MACRO	},
{"dmulo",   "d,v,I",	3,    (int) M_DMULO_I,	INSN_MACRO	},
{"dmulou",  "d,v,t",	3,    (int) M_DMULOU,	INSN_MACRO	},
{"dmulou",  "d,v,I",	3,    (int) M_DMULOU_I,	INSN_MACRO	},
{"dmult",   "s,t",	0x0000001c, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	I3	},
{"dmultu",  "s,t",	0x0000001d, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	I3	},
{"dneg",    "d,w",	0x0000002e, 0xffe007ff,	WR_d|RD_t,	I3	}, /* dsub 0 */
{"dnegu",   "d,w",	0x0000002f, 0xffe007ff,	WR_d|RD_t,	I3	}, /* dsubu 0*/
{"drem",    "z,s,t",	0x0000001e, 0xfc00ffff, RD_s|RD_t|WR_HI|WR_LO,	I3	},
{"drem",    "d,v,t",	3,    (int) M_DREM_3,	INSN_MACRO	},
{"drem",    "d,v,I",	3,    (int) M_DREM_3I,	INSN_MACRO	},
{"dremu",   "z,s,t",	0x0000001f, 0xfc00ffff, RD_s|RD_t|WR_HI|WR_LO,	I3	},
{"dremu",   "d,v,t",	3,    (int) M_DREMU_3,	INSN_MACRO	},
{"dremu",   "d,v,I",	3,    (int) M_DREMU_3I,	INSN_MACRO	},
{"dsllv",   "d,t,s",	0x00000014, 0xfc0007ff,	WR_d|RD_t|RD_s,	I3	},
{"dsll32",  "d,w,<",	0x0000003c, 0xffe0003f, WR_d|RD_t,	I3	},
{"dsll",    "d,w,s",	0x00000014, 0xfc0007ff,	WR_d|RD_t|RD_s,	I3	}, /* dsllv */
{"dsll",    "d,w,>",	0x0000003c, 0xffe0003f, WR_d|RD_t,	I3	}, /* dsll32 */
{"dsll",    "d,w,<",	0x00000038, 0xffe0003f,	WR_d|RD_t,	I3	},
{"dsrav",   "d,t,s",	0x00000017, 0xfc0007ff,	WR_d|RD_t|RD_s,	I3	},
{"dsra32",  "d,w,<",	0x0000003f, 0xffe0003f, WR_d|RD_t,	I3	},
{"dsra",    "d,w,s",	0x00000017, 0xfc0007ff,	WR_d|RD_t|RD_s,	I3	}, /* dsrav */
{"dsra",    "d,w,>",	0x0000003f, 0xffe0003f, WR_d|RD_t,	I3	}, /* dsra32 */
{"dsra",    "d,w,<",	0x0000003b, 0xffe0003f,	WR_d|RD_t,	I3	},
{"dsrlv",   "d,t,s",	0x00000016, 0xfc0007ff,	WR_d|RD_t|RD_s,	I3	},
{"dsrl32",  "d,w,<",	0x0000003e, 0xffe0003f, WR_d|RD_t,	I3	},
{"dsrl",    "d,w,s",	0x00000016, 0xfc0007ff,	WR_d|RD_t|RD_s,	I3	}, /* dsrlv */
{"dsrl",    "d,w,>",	0x0000003e, 0xffe0003f, WR_d|RD_t,	I3	}, /* dsrl32 */
{"dsrl",    "d,w,<",	0x0000003a, 0xffe0003f,	WR_d|RD_t,	I3	},
{"dsub",    "d,v,t",	0x0000002e, 0xfc0007ff,	WR_d|RD_s|RD_t,	I3	},
{"dsub",    "d,v,I",	3,    (int) M_DSUB_I,	INSN_MACRO	},
{"dsubu",   "d,v,t",	0x0000002f, 0xfc0007ff,	WR_d|RD_s|RD_t,	I3	},
{"dsubu",   "d,v,I",	3,    (int) M_DSUBU_I,	INSN_MACRO	},
{"eret",    "",		0x42000018, 0xffffffff,	0,	I3	},
{"floor.l.d", "D,S",	0x4620000b, 0xffff003f, WR_D|RD_S|FP_D,	I3	},
{"floor.l.s", "D,S",	0x4600000b, 0xffff003f, WR_D|RD_S|FP_S,	I3	},
{"floor.w.d", "D,S",	0x4620000f, 0xffff003f, WR_D|RD_S|FP_D,	I2	},
{"floor.w.s", "D,S",	0x4600000f, 0xffff003f, WR_D|RD_S|FP_S,	I2	},
{"flushi",  "",		0xbc010000, 0xffffffff, 0,		L1	},
{"flushd",  "",		0xbc020000, 0xffffffff, 0, L1		},
{"flushid", "",		0xbc030000, 0xffffffff, 0, L1		},
{"hibernate","",        0x42000023, 0xffffffff,	0, V1	},
{"jr",      "s",	0x00000008, 0xfc1fffff,	UBD|RD_s,	I1	},
{"j",       "s",	0x00000008, 0xfc1fffff,	UBD|RD_s,	I1	}, /* jr */
/* SVR4 PIC code requires special handling for j, so it must be a
   macro.  */
{"j",	    "a",	0,     (int) M_J_A,	INSN_MACRO	},
/* This form of j is used by the disassembler and internally by the
   assembler, but will never match user input (because the line above
   will match first).  */
{"j",       "a",	0x08000000, 0xfc000000,	UBD,		I1	},
{"jalr",    "s",	0x0000f809, 0xfc1fffff,	UBD|RD_s|WR_d,	I1	},
{"jalr",    "d,s",	0x00000009, 0xfc1f07ff,	UBD|RD_s|WR_d,	I1	},
/* SVR4 PIC code requires special handling for jal, so it must be a
   macro.  */
{"jal",     "d,s",	0,     (int) M_JAL_2,	INSN_MACRO	},
{"jal",     "s",	0,     (int) M_JAL_1,	INSN_MACRO	},
{"jal",     "a",	0,     (int) M_JAL_A,	INSN_MACRO	},
/* This form of jal is used by the disassembler and internally by the
   assembler, but will never match user input (because the line above
   will match first).  */
{"jal",     "a",	0x0c000000, 0xfc000000,	UBD|WR_31,	I1	},
  /* jalx really should only be avaliable if mips16 is available,
     but for now make it I1. */
{"jalx",    "a",	0x74000000, 0xfc000000, UBD|WR_31,	I1      },
{"la",      "t,A(b)",	0,    (int) M_LA_AB,	INSN_MACRO	},
{"lb",      "t,o(b)",	0x80000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lb",      "t,A(b)",	0,    (int) M_LB_AB,	INSN_MACRO	},
{"lbu",     "t,o(b)",	0x90000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lbu",     "t,A(b)",	0,    (int) M_LBU_AB,	INSN_MACRO	},
{"ld",	    "t,o(b)",   0xdc000000, 0xfc000000, WR_t|RD_b,	I3	},
{"ld",      "t,o(b)",	0,    (int) M_LD_OB,	INSN_MACRO	},
{"ld",      "t,A(b)",	0,    (int) M_LD_AB,	INSN_MACRO	},
{"ldc1",    "T,o(b)",	0xd4000000, 0xfc000000, CLD|RD_b|WR_T|FP_D,	I2	},
{"ldc1",    "E,o(b)",	0xd4000000, 0xfc000000, CLD|RD_b|WR_T|FP_D,	I2	},
{"ldc1",    "T,A(b)",	2,    (int) M_LDC1_AB,	INSN_MACRO	},
{"ldc1",    "E,A(b)",	2,    (int) M_LDC1_AB,	INSN_MACRO	},
{"l.d",     "T,o(b)",	0xd4000000, 0xfc000000, CLD|RD_b|WR_T|FP_D,	I2	}, /* ldc1 */
{"l.d",     "T,o(b)",	0,    (int) M_L_DOB,	INSN_MACRO	},
{"l.d",     "T,A(b)",	0,    (int) M_L_DAB,	INSN_MACRO	},
{"ldc2",    "E,o(b)",	0xd8000000, 0xfc000000, CLD|RD_b|WR_CC,	I2	},
{"ldc2",    "E,A(b)",	2,    (int) M_LDC2_AB,	INSN_MACRO	},
{"ldc3",    "E,o(b)",	0xdc000000, 0xfc000000, CLD|RD_b|WR_CC,	I2	},
{"ldc3",    "E,A(b)",	2,    (int) M_LDC3_AB,	INSN_MACRO	},
{"ldl",	    "t,o(b)",	0x68000000, 0xfc000000, LDD|WR_t|RD_b,	I3	},
{"ldl",	    "t,A(b)",	3,    (int) M_LDL_AB,	INSN_MACRO	},
{"ldr",	    "t,o(b)",	0x6c000000, 0xfc000000, LDD|WR_t|RD_b,	I3	},
{"ldr",     "t,A(b)",	3,    (int) M_LDR_AB,	INSN_MACRO	},
{"ldxc1",   "D,t(b)",	0x4c000001, 0xfc00f83f, LDD|WR_D|RD_t|RD_b,	I4	},
{"lh",      "t,o(b)",	0x84000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lh",      "t,A(b)",	0,    (int) M_LH_AB,	INSN_MACRO	},
{"lhu",     "t,o(b)",	0x94000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lhu",     "t,A(b)",	0,    (int) M_LHU_AB,	INSN_MACRO	},
/* li is at the start of the table.  */
{"li.d",    "t,F",	0,    (int) M_LI_D,	INSN_MACRO	},
{"li.d",    "T,L",	0,    (int) M_LI_DD,	INSN_MACRO	},
{"li.s",    "t,f",	0,    (int) M_LI_S,	INSN_MACRO	},
{"li.s",    "T,l",	0,    (int) M_LI_SS,	INSN_MACRO	},
{"ll",	    "t,o(b)",	0xc0000000, 0xfc000000, LDD|RD_b|WR_t,	I2	},
{"ll",	    "t,A(b)",	2,    (int) M_LL_AB,	INSN_MACRO	},
{"lld",	    "t,o(b)",	0xd0000000, 0xfc000000, LDD|RD_b|WR_t,	I3	},
{"lld",     "t,A(b)",	3,    (int) M_LLD_AB,	INSN_MACRO	},
{"lui",     "t,u",	0x3c000000, 0xffe00000,	WR_t,		I1	},
  /* start-sanitize-r5900 */
{"lq",	    "t,o(b)",	0x78000000, 0xfc000000,	WR_t|RD_b,	T5	},
  /* end-sanitize-r5900 */
{"lw",      "t,o(b)",	0x8c000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lw",      "t,A(b)",	0,    (int) M_LW_AB,	INSN_MACRO	},
{"lwc0",    "E,o(b)",	0xc0000000, 0xfc000000,	CLD|RD_b|WR_CC,	I1	},
{"lwc0",    "E,A(b)",	0,    (int) M_LWC0_AB,	INSN_MACRO	},
{"lwc1",    "T,o(b)",	0xc4000000, 0xfc000000,	CLD|RD_b|WR_T|FP_S,	I1	},
{"lwc1",    "E,o(b)",	0xc4000000, 0xfc000000,	CLD|RD_b|WR_T|FP_S,	I1	},
{"lwc1",    "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"lwc1",    "E,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"l.s",     "T,o(b)",	0xc4000000, 0xfc000000,	CLD|RD_b|WR_T|FP_S,	I1	}, /* lwc1 */
{"l.s",     "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"lwc2",    "E,o(b)",	0xc8000000, 0xfc000000,	CLD|RD_b|WR_CC,	I1	},
{"lwc2",    "E,A(b)",	0,    (int) M_LWC2_AB,	INSN_MACRO	},
{"lwc3",    "E,o(b)",	0xcc000000, 0xfc000000,	CLD|RD_b|WR_CC,	I1	},
{"lwc3",    "E,A(b)",	0,    (int) M_LWC3_AB,	INSN_MACRO	},
{"lwl",     "t,o(b)",	0x88000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lwl",     "t,A(b)",	0,    (int) M_LWL_AB,	INSN_MACRO	},
{"lcache",  "t,o(b)",	0x88000000, 0xfc000000,	LDD|RD_b|WR_t,	I2	}, /* same */
{"lcache",  "t,A(b)",	2,    (int) M_LWL_AB,	INSN_MACRO	}, /* as lwl */
{"lwr",     "t,o(b)",	0x98000000, 0xfc000000,	LDD|RD_b|WR_t,	I1	},
{"lwr",     "t,A(b)",	0,    (int) M_LWR_AB,	INSN_MACRO	},
{"flush",   "t,o(b)",	0x98000000, 0xfc000000,	LDD|RD_b|WR_t,	I2	}, /* same */
{"flush",   "t,A(b)",	2,    (int) M_LWR_AB,	INSN_MACRO	}, /* as lwr */
{"lwu",     "t,o(b)",	0x9c000000, 0xfc000000,	LDD|RD_b|WR_t,	I3	},
{"lwu",     "t,A(b)",	3,    (int) M_LWU_AB,	INSN_MACRO	},
{"lwxc1",   "D,t(b)",	0x4c000000, 0xfc00f83f, LDD|WR_D|RD_t|RD_b,	I4	},
{"mad",	    "s,t",	0x70000000, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO|RD_HI|RD_LO,	P3	},
{"madu",    "s,t",	0x70000001, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO|RD_HI|RD_LO,	P3	},
{"addciu",  "t,r,j",	0x70000000, 0xfc000000,	WR_t|RD_s,L1	},
{"madd.d",  "D,R,S,T",	0x4c000021, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_D,	I4	},
{"madd.s",  "D,R,S,T",	0x4c000020, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_S,	I4	},
/* start-sanitize-r5900 */
{"madd.s",  "D,R,S,T",	0x4c000020, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_S,	T5	},
  /* end-sanitize-r5900 */
{"madd",    "s,t",	0x0000001c, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		L1	},
{"madd",    "s,t",	0x70000000, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		T3	},
{"madd",    "d,s,t",	0x70000000, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d,	T3	},
  /* start-sanitize-r5900 */
{"madd",    "s,t",	0x70000000, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		T5	},
{"madd",    "d,s,t",	0x70000000, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d,	T5	},
{"madd1",   "s,t",	0x70000020, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		T5	},
{"madd1",   "d,s,t",	0x70000020, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d,	T5	},
  /* end-sanitize-r5900 */
{"maddu",   "s,t",	0x0000001d, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		L1	},
{"maddu",   "s,t",	0x70000001, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		T3	},
{"maddu",   "d,s,t",	0x70000001, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d,	T3	},
  /* start-sanitize-r5900 */
{"maddu",   "s,t",	0x70000001, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		T5	},
{"maddu",   "d,s,t",	0x70000001, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d,	T5	},
{"maddu1",  "s,t",	0x70000021, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,		T5	},
{"maddu1",  "d,s,t",	0x70000021, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d,	T5	},
  /* end-sanitize-r5900 */
{"madd16",  "s,t",      0x00000028, 0xfc00ffff, RD_s|RD_t|WR_HI|WR_LO|RD_HI|RD_LO,	V1	},
{"mfc0",    "t,G",	0x40000000, 0xffe007ff,	LCD|WR_t|RD_C0,	I1	},
{"mfc1",    "t,S",	0x44000000, 0xffe007ff,	LCD|WR_t|RD_S|FP_S,	I1	},
{"mfc1",    "t,G",	0x44000000, 0xffe007ff,	LCD|WR_t|RD_S|FP_S,	I1	},
{"mfc2",    "t,G",	0x48000000, 0xffe007ff,	LCD|WR_t|RD_C2,	I1	},
{"mfc3",    "t,G",	0x4c000000, 0xffe007ff,	LCD|WR_t|RD_C3,	I1	},
{"mfhi",    "d",	0x00000010, 0xffff07ff,	WR_d|RD_HI,	I1	},
  /* start-sanitize-r5900 */
{"mfhi1",   "d",	0x70000010, 0xffff07ff,	WR_d|RD_HI,	T5	},
  /* end-sanitize-r5900 */
{"mflo",    "d",	0x00000012, 0xffff07ff,	WR_d|RD_LO,	I1	},
  /* start-sanitize-r5900 */
{"mflo1",   "d",	0x70000012, 0xffff07ff,	WR_d|RD_LO,	T5	},
{"mfsa",    "d",        0x00000028, 0xffff07ff, WR_d,	T5	},
  /* end-sanitize-r5900 */
{"mov.d",   "D,S",	0x46200006, 0xffff003f,	WR_D|RD_S|FP_D,	I1	},
{"mov.s",   "D,S",	0x46000006, 0xffff003f,	WR_D|RD_S|FP_S,	I1	},
{"movf",    "d,s,N",	0x00000001, 0xfc0307ff, WR_d|RD_s|RD_CC|FP_D|FP_S,	I4	},
{"movf.d",  "D,S,N",	0x46200011, 0xffe3003f, WR_D|RD_S|RD_CC|FP_D,	I4	},
{"movf.s",  "D,S,N",	0x46000011, 0xffe3003f, WR_D|RD_S|RD_CC|FP_S,	I4	},
{"movn",    "d,v,t",	0x0000000b, 0xfc0007ff,	WR_d|RD_s|RD_t,	I4	},
  /* start-sanitize-r5900 */
{"movn",    "d,v,t",	0x0000000b, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
  /* end-sanitize-r5900 */
{"ffc",     "d,v",	0x0000000b, 0xfc0007ff,	WR_d|RD_s,L1	},
{"movn.d",  "D,S,t",	0x46200013, 0xffe0003f, WR_D|RD_S|RD_t|FP_D,	I4	},
{"movn.s",  "D,S,t",	0x46000013, 0xffe0003f, WR_D|RD_S|RD_t|FP_S,	I4	},
{"movt",    "d,s,N",	0x00010001, 0xfc0307ff, WR_d|RD_s|RD_CC,	I4	},
{"movt.d",  "D,S,N",	0x46210011, 0xffe3003f, WR_D|RD_S|RD_CC|FP_D,	I4	},
{"movt.s",  "D,S,N",	0x46010011, 0xffe3003f, WR_D|RD_S|RD_CC|FP_S,	I4	},
{"movz",    "d,v,t",	0x0000000a, 0xfc0007ff,	WR_d|RD_s|RD_t,	I4	},
  /* start-sanitize-r5900 */
{"movz",    "d,v,t",	0x0000000a, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
  /* end-sanitize-r5900 */
{"ffs",     "d,v",	0x0000000a, 0xfc0007ff,	WR_d|RD_s,L1	},
{"movz.d",  "D,S,t",	0x46200012, 0xffe0003f, WR_D|RD_S|RD_t|FP_D,	I4	},
{"movz.s",  "D,S,t",	0x46000012, 0xffe0003f, WR_D|RD_S|RD_t|FP_S,	I4	},
/* move is at the top of the table.  */
{"msub.d",  "D,R,S,T",	0x4c000029, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_D,	I4	},
{"msub.s",  "D,R,S,T",	0x4c000028, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_S,	I4	},
/* start-sanitize-r5900 */
{"msub.s",  "D,R,S,T",	0x4c000028, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_S,	T5	},
/* end-sanitize-r5900 */
{"msub",    "s,t",	0x0000001e, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,L1	},
{"msubu",   "s,t",	0x0000001f, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,L1	},
{"mtc0",    "t,G",	0x40800000, 0xffe007ff,	COD|RD_t|WR_C0|WR_CC,	I1	},
{"mtc1",    "t,S",	0x44800000, 0xffe007ff,	COD|RD_t|WR_S|FP_S,	I1	},
{"mtc1",    "t,G",	0x44800000, 0xffe007ff,	COD|RD_t|WR_S|FP_S,	I1	},
{"mtc2",    "t,G",	0x48800000, 0xffe007ff,	COD|RD_t|WR_C2|WR_CC,	I1	},
{"mtc3",    "t,G",	0x4c800000, 0xffe007ff,	COD|RD_t|WR_C3|WR_CC,	I1	},
{"mthi",    "s",	0x00000011, 0xfc1fffff,	RD_s|WR_HI,	I1	},
  /* start-sanitize-r5900 */
{"mthi1",   "s",	0x70000011, 0xfc1fffff,	RD_s|WR_HI,	T5	},
  /* end-sanitize-r5900 */
{"mtlo",    "s",	0x00000013, 0xfc1fffff,	RD_s|WR_LO,	I1	},
  /* start-sanitize-r5900 */
{"mtlo1",   "s",	0x70000013, 0xfc1fffff,	RD_s|WR_LO,	T5	},
{"mtsa",    "s",        0x00000029, 0xfc1fffff, RD_s,	T5	},
{"mtsab",   "s,j",	0x04180000, 0xfc1f0000, RD_s,	T5	},
{"mtsah",   "s,j",	0x04190000, 0xfc1f0000, RD_s,	T5	},
  /* end-sanitize-r5900 */
{"mul.d",   "D,V,T",	0x46200002, 0xffe0003f,	WR_D|RD_S|RD_T|FP_D,	I1	},
{"mul.s",   "D,V,T",	0x46000002, 0xffe0003f,	WR_D|RD_S|RD_T|FP_S,	I1	},
{"mul",     "d,v,t",	0x70000002, 0xfc0007ff, WR_d|RD_s|RD_t|WR_HI|WR_LO,	P3	},
{"mul",     "d,v,t",	0,    (int) M_MUL,	INSN_MACRO	},
{"mul",     "d,v,I",	0,    (int) M_MUL_I,	INSN_MACRO	},
{"mulo",    "d,v,t",	0,    (int) M_MULO,	INSN_MACRO	},
{"mulo",    "d,v,I",	0,    (int) M_MULO_I,	INSN_MACRO	},
{"mulou",   "d,v,t",	0,    (int) M_MULOU,	INSN_MACRO	},
{"mulou",   "d,v,I",	0,    (int) M_MULOU_I,	INSN_MACRO	},
{"mult",    "s,t",	0x00000018, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	    I1},
{"mult",    "d,s,t",	0x00000018, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d, G1},
  /* start-sanitize-r5900 */
{"mult1",   "d,s,t",	0x70000018, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d, T5},
  /* end-sanitize-r5900 */
{"multu",   "s,t",	0x00000019, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	    I1},
{"multu",   "d,s,t",	0x00000019, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d, G1},
  /* start-sanitize-r5900 */
{"multu1",  "d,s,t",	0x70000019, 0xfc0007ff,	RD_s|RD_t|WR_HI|WR_LO|WR_d, T5},
  /* end-sanitize-r5900 */
{"neg",     "d,w",	0x00000022, 0xffe007ff,	WR_d|RD_t,	I1	}, /* sub 0 */
{"negu",    "d,w",	0x00000023, 0xffe007ff,	WR_d|RD_t,	I1	}, /* subu 0 */
{"neg.d",   "D,V",	0x46200007, 0xffff003f,	WR_D|RD_S|FP_D,	I1	},
{"neg.s",   "D,V",	0x46000007, 0xffff003f,	WR_D|RD_S|FP_S,	I1	},
{"nmadd.d", "D,R,S,T",	0x4c000031, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_D,	I4	},
{"nmadd.s", "D,R,S,T",	0x4c000030, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_S,	I4	},
{"nmsub.d", "D,R,S,T",	0x4c000039, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_D,	I4	},
{"nmsub.s", "D,R,S,T",	0x4c000038, 0xfc00003f, RD_R|RD_S|RD_T|WR_D|FP_S,	I4	},
/* nop is at the start of the table.  */
{"nor",     "d,v,t",	0x00000027, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"nor",     "t,r,I",	0,    (int) M_NOR_I,	INSN_MACRO	},
{"not",     "d,v",	0x00000027, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},/*nor d,s,0*/
{"or",      "d,v,t",	0x00000025, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"or",      "t,r,I",	0,    (int) M_OR_I,	INSN_MACRO	},
{"ori",     "t,r,i",	0x34000000, 0xfc000000,	WR_t|RD_s,	I1	},

  /* start-sanitize-r5900 */
{"pabsh",   "d,t",	0x70000168, 0xffe007ff,	WR_d|RD_t,	T5	},
{"pabsw",   "d,t",	0x70000068, 0xffe007ff,	WR_d|RD_t,	T5	},
{"paddb",   "d,v,t",	0x70000208, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"paddh",   "d,v,t",	0x70000108, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"paddw",   "d,v,t",	0x70000008, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"paddsb",  "d,v,t",	0x70000608, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"paddsh",  "d,v,t",	0x70000508, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"paddsw",  "d,v,t",	0x70000408, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"paddub",  "d,v,t",	0x70000628, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"padduh",  "d,v,t",	0x70000528, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"padduw",  "d,v,t",	0x70000428, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"padsbh",  "d,v,t",	0x70000128, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pand",    "d,v,t",	0x70000489, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pceqb",   "d,v,t",	0x700002a8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pceqh",   "d,v,t",	0x700001a8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pceqw",   "d,v,t",	0x700000a8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"pcgtb",   "d,v,t",	0x70000288, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pcgth",   "d,v,t",	0x70000188, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pcgtw",   "d,v,t",	0x70000088, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"pcpyh",   "d,t",	0x700006e9, 0xffe007ff,	WR_d|RD_t,	T5	},

{"pcpyld",  "d,v,t",	0x70000389, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pcpyud",  "d,v,t",	0x700003a9, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"pdivbw",   "s,t",	0x70000749, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pdivuw",   "s,t",	0x70000369, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pdivw",    "s,t",	0x70000349, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	T5	},

{"pexch",    "d,t",	0x700006a9, 0xffe007ff,	WR_d|RD_t,	T5	},
{"pexcw",    "d,t",	0x700007a9, 0xffe007ff,	WR_d|RD_t,	T5	},
{"pexeh",    "d,t",	0x70000689, 0xffe007ff,	WR_d|RD_t,	T5	},
{"pexoh",    "d,t",	0x70000689, 0xffe007ff,	WR_d|RD_t,	T5	},
{"pexew",    "d,t",	0x70000789, 0xffe007ff,	WR_d|RD_t,	T5	},
{"pexow",    "d,t",	0x70000789, 0xffe007ff,	WR_d|RD_t,	T5	},

{"pext5",    "d,t",	0x70000788, 0xffe007ff,	WR_d|RD_t,	T5	},

{"pextlb",   "d,v,t",	0x70000688, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pextlh",   "d,v,t",	0x70000588, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pextlw",   "d,v,t",	0x70000488, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pextub",   "d,v,t",	0x700006a8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pextuh",   "d,v,t",	0x700005a8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pextuw",   "d,v,t",	0x700004a8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"phmaddh",  "d,v,t",	0x70000449, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"phmsubh",  "d,v,t",	0x70000549, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},

{"pinth",    "d,v,t",	0x70000289, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pinteh",   "d,v,t",	0x700002a9, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pintoh",   "d,v,t",	0x700002a9, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"plzcw",    "d,v",	0x70000004, 0xfc1f07ff,	WR_d|RD_s,	T5	},

{"pmaddh",   "d,v,t",	0x70000409, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pmadduw",  "d,v,t",	0x70000029, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pmaddw",   "d,v,t",	0x70000009, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},

{"pmaxh",    "d,v,t",	0x700001c8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pmaxw",    "d,v,t",	0x700000c8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"pmfhi",    "d",	0x70000209, 0xffff07ff,	WR_d|RD_HI,	T5	},
{"pmflo",    "d",	0x70000249, 0xffff07ff,	WR_d|RD_LO,	T5	},

{"pmfhl.lw", "d",	0x70000030, 0xffff07ff,	WR_d|RD_LO|RD_HI,	T5	},
{"pmfhl.uw", "d",	0x70000070, 0xffff07ff,	WR_d|RD_LO|RD_HI,	T5	},
{"pmfhl.slw","d",	0x700000b0, 0xffff07ff,	WR_d|RD_LO|RD_HI,	T5	},
{"pmfhl.lh", "d",	0x700000f0, 0xffff07ff,	WR_d|RD_LO|RD_HI,	T5	},
{"pmfhl.sh", "d",	0x70000130, 0xffff07ff,	WR_d|RD_LO|RD_HI,	T5	},

{"pminh",    "d,v,t",	0x700001e8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"pminw",    "d,v,t",	0x700000e8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"pmsubh",   "d,v,t",	0x70000509, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pmsubw",   "d,v,t",	0x70000109, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},

{"pmthi",    "v",	0x70000229, 0xfc1fffff,	WR_HI|RD_s,	T5	},
{"pmtlo",    "v",	0x70000269, 0xfc1fffff,	WR_LO|RD_s,	T5	},

{"pmthl.lw", "v",	0x70000031, 0xfc1fffff,	WR_HI|WR_LO|RD_s,	T5	},

{"pmulth",   "d,v,t",	0x70000709, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pmultuw",  "d,v,t",	0x70000329, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},
{"pmultw",   "d,v,t",	0x70000309, 0xfc0007ff,	WR_d|RD_s|RD_t|WR_HI|WR_LO,	T5	},

{"pnor",    "d,v,t",	0x700004e9, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"por",     "d,v,t",	0x700004a9, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"ppac5",   "d,t",	0x700007c8, 0xffe007ff,	WR_d|RD_t,	T5	},

{"ppacb",   "d,v,t",	0x700006c8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"ppach",   "d,v,t",	0x700005c8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"ppacw",   "d,v,t",	0x700004c8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"prevh",   "d,t",	0x700006c9, 0xffe007ff,	WR_d|RD_t,	T5	},
{"prot3w",  "d,t",	0x700007c9, 0xffe007ff,	WR_d|RD_t,	T5	},

{"psllh",  "d,t,<",	0x70000034, 0xffe0003f,	WR_d|RD_t,	T5	},
{"psllvw", "d,t,s",	0x70000089, 0xfc0007ff,	WR_d|RD_t|RD_s,	T5	},
{"psllw",  "d,t,<",	0x7000003c, 0xffe0003f,	WR_d|RD_t,	T5	},

{"psrah",  "d,t,<",	0x70000037, 0xffe0003f,	WR_d|RD_t,	T5	},
{"psravw", "d,t,s",	0x700000e9, 0xfc0007ff,	WR_d|RD_t|RD_s,	T5	},
{"psraw",  "d,t,<",	0x7000003f, 0xffe0003f,	WR_d|RD_t,	T5	},

{"psrlh",  "d,t,<",	0x70000036, 0xffe0003f,	WR_d|RD_t,	T5	},
{"psrlvw", "d,t,s",	0x700000c9, 0xfc0007ff,	WR_d|RD_t|RD_s,	T5	},
{"psrlw",  "d,t,<",	0x7000003e, 0xffe0003f,	WR_d|RD_t,	T5	},

{"psubb",  "d,v,t",	0x70000248, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubh",  "d,v,t",	0x70000148, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubsb", "d,v,t",	0x70000648, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubsh", "d,v,t",	0x70000548, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubsw", "d,v,t",	0x70000448, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubub", "d,v,t",	0x70000668, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubuh", "d,v,t",	0x70000568, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubuw", "d,v,t",	0x70000468, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
{"psubw",  "d,v,t",	0x70000048, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},

{"pxor",   "d,v,t",	0x700004c9, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
  /* end-sanitize-r5900 */

{"pref",    "k,o(b)",	0xcc000000, 0xfc000000, RD_b,	I4	},
{"prefx",   "h,t(b)",	0x4c00000f, 0xfc0007ff, RD_b|RD_t,	I4	},

  /* start-sanitize-r5900 */
{"qfsrv",   "d,v,t",	0x700006e8, 0xfc0007ff,	WR_d|RD_s|RD_t,	T5	},
  /* end-sanitize-r5900 */

{"recip.d", "D,S",	0x46200015, 0xffff003f, WR_D|RD_S|FP_D,	I4	},
{"recip.s", "D,S",	0x46000015, 0xffff003f, WR_D|RD_S|FP_S,	I4	},
{"rem",     "z,s,t",	0x0000001a, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	I1 },
{"rem",     "d,v,t",	0,    (int) M_REM_3,	INSN_MACRO	},
{"rem",     "d,v,I",	0,    (int) M_REM_3I,	INSN_MACRO	},
{"remu",    "z,s,t",	0x0000001b, 0xfc00ffff,	RD_s|RD_t|WR_HI|WR_LO,	I1 },
{"remu",    "d,v,t",	0,    (int) M_REMU_3,	INSN_MACRO	},
{"remu",    "d,v,I",	0,    (int) M_REMU_3I,	INSN_MACRO	},
{"rfe",     "",		0x42000010, 0xffffffff,	0,	I1|T3		},
{"rol",     "d,v,t",	0,    (int) M_ROL,	INSN_MACRO	},
{"rol",     "d,v,I",	0,    (int) M_ROL_I,	INSN_MACRO	},
{"ror",     "d,v,t",	0,    (int) M_ROR,	INSN_MACRO	},
{"ror",     "d,v,I",	0,    (int) M_ROR_I,	INSN_MACRO	},
{"round.l.d", "D,S",	0x46200008, 0xffff003f, WR_D|RD_S|FP_D,	I3	},
{"round.l.s", "D,S",	0x46000008, 0xffff003f, WR_D|RD_S|FP_S,	I3	},
{"round.w.d", "D,S",	0x4620000c, 0xffff003f, WR_D|RD_S|FP_D,	I2	},
{"round.w.s", "D,S",	0x4600000c, 0xffff003f, WR_D|RD_S|FP_S,	I2	},
{"rsqrt.d", "D,S",	0x46200016, 0xffff003f, WR_D|RD_S|FP_D,	I4	},
{"rsqrt.s", "D,S",	0x46000016, 0xffff003f, WR_D|RD_S|FP_S,	I4	},
/* start-sanitize-r5900 */
{"rsqrt.s", "D,S",	0x46000016, 0xffff003f, WR_D|RD_S|FP_S,	T5	},
/* end-sanitize-r5900 */
{"sb",      "t,o(b)",	0xa0000000, 0xfc000000,	SM|RD_t|RD_b,	I1	},
{"sb",      "t,A(b)",	0,    (int) M_SB_AB,	INSN_MACRO	},
{"sc",	    "t,o(b)",	0xe0000000, 0xfc000000, SM|RD_t|WR_t|RD_b,	I2	},
{"sc",	    "t,A(b)",	2,    (int) M_SC_AB,	INSN_MACRO	},
{"scd",	    "t,o(b)",	0xf0000000, 0xfc000000, SM|RD_t|WR_t|RD_b,	I3	},
{"scd",	    "t,A(b)",	3,    (int) M_SCD_AB,	INSN_MACRO	},
{"sd",	    "t,o(b)",	0xfc000000, 0xfc000000,	SM|RD_t|RD_b,	I3	},
{"sd",      "t,o(b)",	0,    (int) M_SD_OB,	INSN_MACRO	},
{"sd",      "t,A(b)",	0,    (int) M_SD_AB,	INSN_MACRO	},
{"sdbbp",   "",		0x0000000e, 0xffffffff,	TRAP,           T3	},
{"sdbbp",   "c",	0x0000000e, 0xfc00003f,	TRAP,		T3	},
{"sdc1",    "T,o(b)",	0xf4000000, 0xfc000000, SM|RD_T|RD_b|FP_D,	I2	},
{"sdc1",    "E,o(b)",	0xf4000000, 0xfc000000, SM|RD_T|RD_b|FP_D,	I2	},
{"sdc1",    "T,A(b)",	2,    (int) M_SDC1_AB,	INSN_MACRO	},
{"sdc1",    "E,A(b)",	2,    (int) M_SDC1_AB,	INSN_MACRO	},
{"sdc2",    "E,o(b)",	0xf8000000, 0xfc000000, SM|RD_C2|RD_b,	I2	},
{"sdc2",    "E,A(b)",	2,    (int) M_SDC2_AB,	INSN_MACRO	},
{"sdc3",    "E,o(b)",	0xfc000000, 0xfc000000, SM|RD_C3|RD_b,	I2	},
{"sdc3",    "E,A(b)",	2,    (int) M_SDC3_AB,	INSN_MACRO	},
{"s.d",     "T,o(b)",	0xf4000000, 0xfc000000, SM|RD_T|RD_b,	I2	},
{"s.d",     "T,o(b)",	0,    (int) M_S_DOB,	INSN_MACRO	},
{"s.d",     "T,A(b)",	0,    (int) M_S_DAB,	INSN_MACRO	},
{"sdl",     "t,o(b)",	0xb0000000, 0xfc000000,	SM|RD_t|RD_b,	I3	},
{"sdl",     "t,A(b)",	3,    (int) M_SDL_AB,	INSN_MACRO	},
{"sdr",     "t,o(b)",	0xb4000000, 0xfc000000,	SM|RD_t|RD_b,	I3	},
{"sdr",     "t,A(b)",	3,    (int) M_SDR_AB,	INSN_MACRO	},
{"sdxc1",   "S,t(b)",   0x4c000009, 0xfc0007ff, SM|RD_S|RD_t|RD_b,	I4	},
{"selsl",   "d,v,t",	0x00000005, 0xfc0007ff,	WR_d|RD_s|RD_t,L1	},
{"selsr",   "d,v,t",	0x00000001, 0xfc0007ff,	WR_d|RD_s|RD_t,L1	},
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
{"sh",      "t,o(b)",	0xa4000000, 0xfc000000,	SM|RD_t|RD_b,	I1	},
{"sh",      "t,A(b)",	0,    (int) M_SH_AB,	INSN_MACRO	},
{"sle",     "d,v,t",	0,    (int) M_SLE,	INSN_MACRO	},
{"sle",     "d,v,I",	0,    (int) M_SLE_I,	INSN_MACRO	},
{"sleu",    "d,v,t",	0,    (int) M_SLEU,	INSN_MACRO	},
{"sleu",    "d,v,I",	0,    (int) M_SLEU_I,	INSN_MACRO	},
{"sllv",    "d,t,s",	0x00000004, 0xfc0007ff,	WR_d|RD_t|RD_s,	I1	},
{"sll",     "d,w,s",	0x00000004, 0xfc0007ff,	WR_d|RD_t|RD_s,	I1	}, /* sllv */
{"sll",     "d,w,<",	0x00000000, 0xffe0003f,	WR_d|RD_t,	I1	},
{"slt",     "d,v,t",	0x0000002a, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"slt",     "d,v,I",	0,    (int) M_SLT_I,	INSN_MACRO	},
{"slti",    "t,r,j",	0x28000000, 0xfc000000,	WR_t|RD_s,	I1	},
{"sltiu",   "t,r,j",	0x2c000000, 0xfc000000,	WR_t|RD_s,	I1	},
{"sltu",    "d,v,t",	0x0000002b, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"sltu",    "d,v,I",	0,    (int) M_SLTU_I,	INSN_MACRO	},
{"sne",     "d,v,t",	0,    (int) M_SNE,	INSN_MACRO	},
{"sne",     "d,v,I",	0,    (int) M_SNE_I,	INSN_MACRO	},
  /* start-sanitize-r5900 */
{"sq",	    "t,o(b)",	0x7c000000, 0xfc000000,	SM|RD_t|RD_b,	T5	},
  /* end-sanitize-r5900 */
{"sqrt.d",  "D,S",	0x46200004, 0xffff003f, WR_D|RD_S|FP_D,	I2	},
{"sqrt.s",  "D,S",	0x46000004, 0xffff003f, WR_D|RD_S|FP_S,	I2	},
{"srav",    "d,t,s",	0x00000007, 0xfc0007ff,	WR_d|RD_t|RD_s,	I1	},
{"sra",     "d,w,s",	0x00000007, 0xfc0007ff,	WR_d|RD_t|RD_s,	I1	}, /* srav */
{"sra",     "d,w,<",	0x00000003, 0xffe0003f,	WR_d|RD_t,	I1	},
{"srlv",    "d,t,s",	0x00000006, 0xfc0007ff,	WR_d|RD_t|RD_s,	I1	},
{"srl",     "d,w,s",	0x00000006, 0xfc0007ff,	WR_d|RD_t|RD_s,	I1	}, /* srlv */
{"srl",     "d,w,<",	0x00000002, 0xffe0003f,	WR_d|RD_t,	I1	},
{"standby", "",         0x42000021, 0xffffffff,	0,		V1	},
{"sub",     "d,v,t",	0x00000022, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"sub",     "d,v,I",	0,    (int) M_SUB_I,	INSN_MACRO	},
{"sub.d",   "D,V,T",	0x46200001, 0xffe0003f,	WR_D|RD_S|RD_T|FP_D,	I1	},     
{"sub.s",   "D,V,T",	0x46000001, 0xffe0003f,	WR_D|RD_S|RD_T|FP_S,	I1	},
{"subu",    "d,v,t",	0x00000023, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"subu",    "d,v,I",	0,    (int) M_SUBU_I,	INSN_MACRO	},
{"suspend", "",         0x42000022, 0xffffffff,	0,		V1	},
{"sw",      "t,o(b)",	0xac000000, 0xfc000000,	SM|RD_t|RD_b,	I1	},
{"sw",      "t,A(b)",	0,    (int) M_SW_AB,	INSN_MACRO	},
{"swc0",    "E,o(b)",	0xe0000000, 0xfc000000,	SM|RD_C0|RD_b,	I1	},
{"swc0",    "E,A(b)",	0,    (int) M_SWC0_AB,	INSN_MACRO	},
{"swc1",    "T,o(b)",	0xe4000000, 0xfc000000,	SM|RD_T|RD_b|FP_S,	I1	},
{"swc1",    "E,o(b)",	0xe4000000, 0xfc000000,	SM|RD_T|RD_b|FP_S,	I1	},
{"swc1",    "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"swc1",    "E,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"s.s",     "T,o(b)",	0xe4000000, 0xfc000000,	SM|RD_T|RD_b|FP_S,	I1	}, /* swc1 */
{"s.s",     "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"swc2",    "E,o(b)",	0xe8000000, 0xfc000000,	SM|RD_C2|RD_b,	I1	},
{"swc2",    "E,A(b)",	0,    (int) M_SWC2_AB,	INSN_MACRO	},
{"swc3",    "E,o(b)",	0xec000000, 0xfc000000,	SM|RD_C3|RD_b,	I1	},
{"swc3",    "E,A(b)",	0,    (int) M_SWC3_AB,	INSN_MACRO	},
{"swl",     "t,o(b)",	0xa8000000, 0xfc000000,	SM|RD_t|RD_b,	I1	},
{"swl",     "t,A(b)",	0,    (int) M_SWL_AB,	INSN_MACRO	},
{"scache",  "t,o(b)",	0xa8000000, 0xfc000000,	RD_t|RD_b,	I2	}, /* same */
{"scache",  "t,A(b)",	2,    (int) M_SWL_AB,	INSN_MACRO	}, /* as swl */
{"swr",     "t,o(b)",	0xb8000000, 0xfc000000,	SM|RD_t|RD_b,	I1	},
{"swr",     "t,A(b)",	0,    (int) M_SWR_AB,	INSN_MACRO	},
{"invalidate", "t,o(b)",0xb8000000, 0xfc000000,	RD_t|RD_b,	I2	}, /* same */
{"invalidate", "t,A(b)",2,    (int) M_SWR_AB,	INSN_MACRO	}, /* as swr */
{"swxc1",   "S,t(b)",   0x4c000008, 0xfc0007ff, SM|RD_S|RD_t|RD_b,	I4	},
{"sync",    "",		0x0000000f, 0xffffffff,	0,		I2	},
{"syscall", "",		0x0000000c, 0xffffffff,	TRAP,	I1		},
{"syscall", "B",	0x0000000c, 0xfc00003f,	TRAP,	I1		},
{"teqi",    "s,j",	0x040c0000, 0xfc1f0000, RD_s|TRAP,	I2	},
{"teq",	    "s,t",	0x00000034, 0xfc00003f, RD_s|RD_t|TRAP,	I2	},
{"teq",     "s,j",	0x040c0000, 0xfc1f0000, RD_s|TRAP,	I2	}, /* teqi */
{"teq",     "s,I",	2,    (int) M_TEQ_I,	INSN_MACRO	},
{"tgei",    "s,j",	0x04080000, 0xfc1f0000, RD_s|TRAP,	I2	},
{"tge",	    "s,t",	0x00000030, 0xfc00003f,	RD_s|RD_t|TRAP,	I2	},
{"tge",     "s,j",	0x04080000, 0xfc1f0000, RD_s|TRAP,	I2	}, /* tgei */
{"tge",	    "s,I",	2,    (int) M_TGE_I,    INSN_MACRO	},
{"tgeiu",   "s,j",	0x04090000, 0xfc1f0000, RD_s|TRAP,	I2		},
{"tgeu",    "s,t",	0x00000031, 0xfc00003f, RD_s|RD_t|TRAP,	I2	 },
{"tgeu",    "s,j",	0x04090000, 0xfc1f0000, RD_s|TRAP,	I2		}, /* tgeiu */
{"tgeu",    "s,I",	2,    (int) M_TGEU_I,	INSN_MACRO	},
{"tlbp",    "",		0x42000008, 0xffffffff,	INSN_TLB,	I1	},
{"tlbr",    "",		0x42000001, 0xffffffff,	INSN_TLB,	I1	},
{"tlbwi",   "",		0x42000002, 0xffffffff,	INSN_TLB,	I1	},
{"tlbwr",   "",		0x42000006, 0xffffffff,	INSN_TLB,	I1	},
{"tlti",    "s,j",	0x040a0000, 0xfc1f0000,	RD_s|TRAP,	I2		},
{"tlt",     "s,t",	0x00000032, 0xfc00003f, RD_s|RD_t|TRAP,	I2	 },
{"tlt",     "s,j",	0x040a0000, 0xfc1f0000,	RD_s|TRAP,	I2		}, /* tlti */
{"tlt",     "s,I",	2,    (int) M_TLT_I,	INSN_MACRO	},
{"tltiu",   "s,j",	0x040b0000, 0xfc1f0000, RD_s|TRAP,	I2		},
{"tltu",    "s,t",	0x00000033, 0xfc00003f, RD_s|RD_t|TRAP,	I2	 },
{"tltu",    "s,j",	0x040b0000, 0xfc1f0000, RD_s|TRAP,	I2		}, /* tltiu */
{"tltu",    "s,I",	2,    (int) M_TLTU_I,	INSN_MACRO	},
{"tnei",    "s,j",	0x040e0000, 0xfc1f0000, RD_s|TRAP,	I2		},
{"tne",     "s,t",	0x00000036, 0xfc00003f, RD_s|RD_t|TRAP,	I2	 },
{"tne",     "s,j",	0x040e0000, 0xfc1f0000, RD_s|TRAP,	I2		}, /* tnei */
{"tne",     "s,I",	2,    (int) M_TNE_I,	INSN_MACRO	},
{"trunc.l.d", "D,S",	0x46200009, 0xffff003f, WR_D|RD_S|FP_D,	I3	},
{"trunc.l.s", "D,S",	0x46000009, 0xffff003f,	WR_D|RD_S|FP_S,	I3	},
{"trunc.w.d", "D,S",	0x4620000d, 0xffff003f, WR_D|RD_S|FP_D,	I2	},
{"trunc.w.d", "D,S,x",	0x4620000d, 0xffff003f, WR_D|RD_S|FP_D,	I2	},
{"trunc.w.d", "D,S,t",	0,    (int) M_TRUNCWD,	INSN_MACRO	},
{"trunc.w.s", "D,S",	0x4600000d, 0xffff003f,	WR_D|RD_S|FP_S,	I2	},
{"trunc.w.s", "D,S,x",	0x4600000d, 0xffff003f,	WR_D|RD_S|FP_S,	I2	},
{"trunc.w.s", "D,S,t",	0,    (int) M_TRUNCWS,	INSN_MACRO	},
{"uld",     "t,o(b)",	3,    (int) M_ULD,	INSN_MACRO	},
{"uld",     "t,A(b)",	3,    (int) M_ULD_A,	INSN_MACRO	},
{"ulh",     "t,o(b)",	0,    (int) M_ULH,	INSN_MACRO	},
{"ulh",     "t,A(b)",	0,    (int) M_ULH_A,	INSN_MACRO	},
{"ulhu",    "t,o(b)",	0,    (int) M_ULHU,	INSN_MACRO	},
{"ulhu",    "t,A(b)",	0,    (int) M_ULHU_A,	INSN_MACRO	},
{"ulw",     "t,o(b)",	0,    (int) M_ULW,	INSN_MACRO	},
{"ulw",     "t,A(b)",	0,    (int) M_ULW_A,	INSN_MACRO	},
{"usd",     "t,o(b)",	3,    (int) M_USD,	INSN_MACRO	},
{"usd",     "t,A(b)",	3,    (int) M_USD_A,	INSN_MACRO	},
{"ush",     "t,o(b)",	0,    (int) M_USH,	INSN_MACRO	},
{"ush",     "t,A(b)",	0,    (int) M_USH_A,	INSN_MACRO	},
{"usw",     "t,o(b)",	0,    (int) M_USW,	INSN_MACRO	},
{"usw",     "t,A(b)",	0,    (int) M_USW_A,	INSN_MACRO	},
{"xor",     "d,v,t",	0x00000026, 0xfc0007ff,	WR_d|RD_s|RD_t,	I1	},
{"xor",     "t,r,I",	0,    (int) M_XOR_I,	INSN_MACRO	},
{"xori",    "t,r,i",	0x38000000, 0xfc000000,	WR_t|RD_s,	I1	},
{"wait",    "",		0x42000020, 0xffffffff,	TRAP,	I3	},
{"waiti",   "",		0x42000020, 0xffffffff,	TRAP,	L1	},
{"wb", 	    "o(b)",	0xbc040000, 0xfc1f0000, SM|RD_b,	L1	},
/* No hazard protection on coprocessor instructions--they shouldn't
   change the state of the processor and if they do it's up to the
   user to put in nops as necessary.  These are at the end so that the
   disasembler recognizes more specific versions first.  */
{"c0",      "C",	0x42000000, 0xfe000000,	0,	I1		},
{"c1",      "C",	0x46000000, 0xfe000000,	0,	I1		},
{"c2",      "C",	0x4a000000, 0xfe000000,	0,	I1		},
{"c3",      "C",	0x4e000000, 0xfe000000,	0,	I1		},
{"cop0",     "C",	0,    (int) M_COP0,	    INSN_MACRO	},
{"cop1",     "C",	0,    (int) M_COP1,	    INSN_MACRO	},
{"cop2",     "C",	0,    (int) M_COP2,	    INSN_MACRO	},
{"cop3",     "C",	0,    (int) M_COP3,	    INSN_MACRO	},
};

#define MIPS_NUM_OPCODES \
	((sizeof mips_builtin_opcodes) / (sizeof (mips_builtin_opcodes[0])))
const int bfd_mips_num_builtin_opcodes = MIPS_NUM_OPCODES;

/* const removed from the following to allow for dynamic extensions to the 
 * built-in instruction set. */
struct mips_opcode *mips_opcodes =
  (struct mips_opcode *) mips_builtin_opcodes;
int bfd_mips_num_opcodes = MIPS_NUM_OPCODES;
#undef MIPS_NUM_OPCODES

