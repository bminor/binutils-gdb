/* d30v-opc.c -- D30V opcode list
   Copyright 1997 Free Software Foundation, Inc.
   Written by Martin Hunt, Cygnus Support

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
2, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/d30v.h"


/* This table is sorted. */
/* If you add anything, it MUST be in alphabetical order */
/* The first field is the name the assembler uses when looking */
/* up orcodes.  The second field is the name the disassembler will use. */
/* This allows the assembler to assemble references to r63 (for example) */
/* or "sp".  The disassembler will always use the preferred form (sp) */
const struct pd_reg pre_defined_registers[] =
{
  { "a0", NULL, OPERAND_ACC+0 },
  { "a1", NULL, OPERAND_ACC+1 },
  { "bpc", NULL, OPERAND_CONTROL+2 },
  { "bpsw", NULL, OPERAND_CONTROL+3 },
  { "c", "c", OPERAND_FLAG+7 },
  { "cr0", "pc", OPERAND_CONTROL },
  { "cr1", "psw", OPERAND_CONTROL+1 },
  { "cr10", "mod_e", OPERAND_CONTROL+10 },
  { "cr11", "iba", OPERAND_CONTROL+11 },
  { "cr12", NULL, OPERAND_CONTROL+12 },
  { "cr13", NULL, OPERAND_CONTROL+13 },
  { "cr14", NULL, OPERAND_CONTROL+14 },
  { "cr15", NULL, OPERAND_CONTROL+15 },
  { "cr2", "bpc", OPERAND_CONTROL+2 },
  { "cr3", "bpsw", OPERAND_CONTROL+3 },
  { "cr4", NULL, OPERAND_CONTROL+4 },
  { "cr5", NULL, OPERAND_CONTROL+5 },
  { "cr6", "rpt_c", OPERAND_CONTROL+6 },
  { "cr7", "rpt_s", OPERAND_CONTROL+7 },
  { "cr8", "rpt_e", OPERAND_CONTROL+8 },
  { "cr9", "mod_s", OPERAND_CONTROL+9 },
  { "f0", NULL, OPERAND_FLAG+0 },
  { "f1", NULL, OPERAND_FLAG+1 },
  { "f2", NULL, OPERAND_FLAG+2 },
  { "f3", NULL, OPERAND_FLAG+3 },
  { "f4", "s", OPERAND_FLAG+4 },
  { "f5", "v", OPERAND_FLAG+5 },
  { "f6", "va", OPERAND_FLAG+6 },
  { "f7", "c", OPERAND_FLAG+7 },
  { "iba", NULL, OPERAND_CONTROL+11 },
  { "link", "r62", 62 },
  { "mod_e", NULL, OPERAND_CONTROL+10 },
  { "mod_s", NULL, OPERAND_CONTROL+9 },
  { "pc", NULL, OPERAND_CONTROL+0 },
  { "psw", NULL, OPERAND_CONTROL+1 },
  { "pswh", NULL, OPERAND_CONTROL+MAX_CONTROL_REG+2 },
  { "pswl", NULL, OPERAND_CONTROL+MAX_CONTROL_REG+1 },
  { "r0", "0", 0 },
  { "r1", NULL, 1 },
  { "r10", NULL, 10 },
  { "r11", NULL, 11 },
  { "r12", NULL, 12 },
  { "r13", NULL, 13 },
  { "r14", NULL, 14 },
  { "r15", NULL, 15 },
  { "r16", NULL, 16 },
  { "r17", NULL, 17 },
  { "r18", NULL, 18 },
  { "r19", NULL, 19 },
  { "r2", NULL, 2 },
  { "r20", NULL, 20 },
  { "r21", NULL, 21 },
  { "r22", NULL, 22 },
  { "r23", NULL, 23 },
  { "r24", NULL, 24 },
  { "r25", NULL, 25 },
  { "r26", NULL, 26 },
  { "r27", NULL, 27 },
  { "r28", NULL, 28 },
  { "r29", NULL, 29 },
  { "r3", NULL, 3 },
  { "r30", NULL, 30 },
  { "r31", NULL, 31 },
  { "r32", NULL, 32 },
  { "r33", NULL, 33 },
  { "r34", NULL, 34 },
  { "r35", NULL, 35 },
  { "r36", NULL, 36 },
  { "r37", NULL, 37 },
  { "r38", NULL, 38 },
  { "r39", NULL, 39 },
  { "r4", NULL, 4 },
  { "r40", NULL, 40 },
  { "r41", NULL, 41 },
  { "r42", NULL, 42 },
  { "r43", NULL, 43 },
  { "r44", NULL, 44 },
  { "r45", NULL, 45 },
  { "r46", NULL, 46 },
  { "r47", NULL, 47 },
  { "r48", NULL, 48 },
  { "r49", NULL, 49 },
  { "r5", NULL, 5 },
  { "r50", NULL, 50 },
  { "r51", NULL, 51 },
  { "r52", NULL, 52 },
  { "r53", NULL, 53 },
  { "r54", NULL, 54 },
  { "r55", NULL, 55 },
  { "r56", NULL, 56 },
  { "r57", NULL, 57 },
  { "r58", NULL, 58 },
  { "r59", NULL, 59 },
  { "r6", NULL, 6 },
  { "r60", NULL, 60 },
  { "r61", NULL, 61 },
  { "r62", "link", 62 },
  { "r63", "sp", 63 },
  { "r7", NULL, 7 },
  { "r8", NULL, 8 },
  { "r9", NULL, 9 },
  { "rpt_c", NULL, OPERAND_CONTROL+6 },
  { "rpt_e", NULL, OPERAND_CONTROL+8 },
  { "rpt_s", NULL, OPERAND_CONTROL+7 },
  { "s", NULL, OPERAND_FLAG+4 },
  { "sp", NULL, 63 },
  { "v", NULL, OPERAND_FLAG+5 },
  { "va", NULL, OPERAND_FLAG+6 },
};

int 
reg_name_cnt()
{
  return (sizeof(pre_defined_registers) / sizeof(struct pd_reg));
}

/* OPCODE TABLE */
/* The format of this table is defined in opcode/d30v.h */
const struct d30v_opcode d30v_opcode_table[] = {
  { "abs", IALU1, 0x8, { SHORT_U }, EITHER, 0, 0, 0 },
  { "add", IALU1, 0x0, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "add2h", IALU1, 0x1, { SHORT_A, LONG}, EITHER, 0, 0, 0 },
  { "addc", IALU1, 0x4, { SHORT_A, LONG }, EITHER, FLAG_C, FLAG_CVVA, 0 },
  { "addhlll", IALU1, 0x10, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhllh", IALU1, 0x11, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhlhl", IALU1, 0x12, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhlhh", IALU1, 0x13, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhhll", IALU1, 0x14, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhhlh", IALU1, 0x15, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhhhl", IALU1, 0x16, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "addhhhh", IALU1, 0x17, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "adds", IALU1, 0x6, { SHORT_A, LONG }, EITHER, 0, FLAG_CVVA, 0 },
  { "adds2h", IALU1, 0x7, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "and", LOGIC, 0x18, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "andfg", LOGIC, 0x8, { SHORT_F }, EITHER, FLAG_X, FLAG_X, 0 },
  { "avg", IALU1, 0xa, { SHORT_A, LONG}, EITHER, 0, 0, 0 },
  { "avg2h", IALU1, 0xb, { SHORT_A, LONG}, EITHER, 0, 0, 0 },
  { "bclr", LOGIC, 0x3, { SHORT_A }, EITHER, 0, 0, 0 },
  { "bnot", LOGIC, 0x1, { SHORT_A }, EITHER, 0, 0, 0 },
  { "bra", BRA, 0, { SHORT_B1, SHORT_B2, LONG_U }, MU, FLAG_JMP, 0, RELOC_PCREL },
  { "bratnz", BRA, 0x4, { SHORT_B3b, LONG_2b }, MU, FLAG_JMP, 0, RELOC_PCREL },
  { "bratzr", BRA, 0x4, { SHORT_B3, LONG_2 }, MU, FLAG_JMP, 0, RELOC_PCREL },
  { "bset", LOGIC, 0x2, { SHORT_A }, EITHER, 0, 0, 0 },
  { "bsr", BRA, 0x2, { SHORT_B1, SHORT_B2, LONG_U }, MU, FLAG_JSR, 0, RELOC_PCREL },
  { "bsrtnz", BRA, 0x6, { SHORT_B3b, LONG_2b }, MU, FLAG_JSR, 0, RELOC_PCREL },
  { "bsrtzr", BRA, 0x6, { SHORT_B3, LONG_2 }, MU, FLAG_JSR, 0, RELOC_PCREL },
  { "btst", LOGIC, 0, { SHORT_AF }, EITHER, 0, FLAG_X, 0 },
  { "cmp", LOGIC, 0xC, { SHORT_CMP, LONG_CMP }, EITHER, 0, FLAG_X, 0 },
  { "cmpu", LOGIC, 0xD, { SHORT_CMP, LONG_CMP }, EITHER, 0, FLAG_X, 0 },
  { "dbra", BRA, 0x10, { SHORT_B3, LONG_2 }, MU, FLAG_JMP, FLAG_RP, RELOC_PCREL },
  { "dbrai", BRA, 0x14, { SHORT_D2, LONG_D }, MU, FLAG_JMP, FLAG_RP, RELOC_PCREL },
  { "dbsr", BRA, 0x12, { SHORT_B3, LONG_2 }, MU, FLAG_JSR, FLAG_RP, RELOC_PCREL },
  { "dbsri", BRA, 0x16, { SHORT_D2, LONG_D }, MU, FLAG_JSR, FLAG_RP, RELOC_PCREL },
  { "djmp", BRA, 0x11, { SHORT_B3, LONG_2 }, MU, FLAG_JMP, FLAG_RP, RELOC_ABS },
  { "djmpi", BRA, 0x15, { SHORT_D2, LONG_D }, MU, FLAG_JMP, FLAG_RP, RELOC_ABS },
  { "djsr", BRA, 0x13, { SHORT_B3, LONG_2 }, MU, FLAG_JSR, FLAG_RP, RELOC_ABS },
  { "djsri", BRA, 0x17, { SHORT_D2, LONG_D }, MU, FLAG_JSR, FLAG_RP, RELOC_ABS },
  { "jmp", BRA, 0x1, { SHORT_B1, SHORT_B2, LONG_U }, MU, FLAG_JMP, 0, RELOC_ABS },
  { "jmptnz", BRA, 0x5, { SHORT_B3b, LONG_2b }, MU, FLAG_JMP, 0, RELOC_ABS },
  { "jmptzr", BRA, 0x5, { SHORT_B3, LONG_2 }, MU, FLAG_JMP, 0, RELOC_ABS },
  { "joinll", IALU1, 0xC, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "joinlh", IALU1, 0xD, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "joinhl", IALU1, 0xE, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "joinhh", IALU1, 0xF, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "jsr", BRA, 0x3, { SHORT_B1, SHORT_B2, LONG_U }, MU, FLAG_JSR, 0, RELOC_ABS },
  { "jsrtnz", BRA, 0x7, { SHORT_B3b, LONG_2b }, MU, FLAG_JSR, 0, RELOC_ABS },
  { "jsrtzr", BRA, 0x7, { SHORT_B3, LONG_2 }, MU, FLAG_JSR, 0, RELOC_ABS },
  { "ld2h", IMEM, 0x3, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ld2w", IMEM, 0x6, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ld4bh", IMEM, 0x5, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ld4bhu", IMEM, 0xd, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ldb", IMEM, 0, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ldbu", IMEM, 0x9, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ldh", IMEM, 0x2, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ldhh", IMEM, 0x1, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ldhu", IMEM, 0xa, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "ldw", IMEM, 0x4, { SHORT_M, LONG_M }, MU, FLAG_MEM, 0, 0 },
  { "mac0", IALU2, 0x14, { SHORT_A }, IU, 0, 0, 0 },
  { "mac1", IALU2, 0x14, { SHORT_A1 }, IU, 0, 0, 0 },
  { "macs0", IALU2, 0x15, { SHORT_A }, IU, 0, 0, 0 },
  { "macs1", IALU2, 0x15, { SHORT_A1 }, IU, 0, 0, 0 },
  { "moddec", IMEM, 0x7, { SHORT_MODDEC }, MU, 0, 0, 0 },
  { "modinc", IMEM, 0x7, { SHORT_MODINC }, MU, 0, 0, 0 },
  { "msub0", IALU2, 0x16, { SHORT_A }, IU, 0, 0, 0 },
  { "msub1", IALU2, 0x16, { SHORT_A1 }, IU, 0, 0, 0 },
  { "msubs0", IALU2, 0x17, { SHORT_A }, IU, 0, 0, 0 },
  { "msubs1", IALU2, 0x17, { SHORT_A1 }, IU, 0, 0, 0 },
  { "mul", IALU2, 0x10, { SHORT_A }, IU, 0, 0, 0 },
  { "mul2h", IALU2, 0, { SHORT_A }, IU, 0, 0, 0 },
  { "mulhxll", IALU2, 0x4, { SHORT_A }, IU, 0, 0, 0 },
  { "mulhxlh", IALU2, 0x5, { SHORT_A }, IU, 0, 0, 0 },
  { "mulhxhl", IALU2, 0x6, { SHORT_A }, IU, 0, 0, 0 },
  { "mulhxhh", IALU2, 0x7, { SHORT_A }, IU, 0, 0, 0 },
  { "mulx", IALU2, 0x18, { SHORT_AA }, IU, 0, 0, 0 },
  { "mulx2h", IALU2, 0x1, { SHORT_A }, IU, 0, 0, 0 },
  { "mulxs", IALU2, 0x19, { SHORT_AA }, IU, 0, 0, 0 },
  { "mvfacc", IALU2, 0x1f, { SHORT_RA }, IU, 0, 0, 0 },
  { "mvfsys", BRA, 0x1e, { SHORT_C1 }, MU, FLAG_ALL, FLAG_ALL, 0 },
  { "mvtacc", IALU2, 0xf, { SHORT_AA }, IU, 0, 0, 0 },
  { "mvtsys", BRA, 0xe, { SHORT_C2 }, MU, FLAG_ALL, FLAG_ALL, 0 },
  { "nop", BRA, 0xF, { SHORT_NONE }, EITHER, 0, 0, 0 },
  { "not", LOGIC, 0x19, { SHORT_U }, EITHER, 0, 0, 0 },
  { "notfg", LOGIC, 0x9, { SHORT_UF }, EITHER, FLAG_X, FLAG_X, 0 },
  { "or", LOGIC, 0x1a, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "orfg", LOGIC, 0xa, { SHORT_F }, EITHER, FLAG_X, FLAG_X, 0 },
  { "reit", BRA, 0x8, { SHORT_NONE }, MU, FLAG_SM, FLAG_SM, 0 },
  { "repeat", BRA, 0x18, { SHORT_D1, LONG_2 }, MU, FLAG_RP, FLAG_RP, 0 },
  { "repeati", BRA, 0x1a, { SHORT_D2, LONG_D }, MU, FLAG_RP, FLAG_RP, 0 },
  { "rot", LOGIC, 0x14, { SHORT_A }, EITHER, 0, 0, 0 },
  { "rot2h", LOGIC, 0x15, { SHORT_A }, EITHER, 0, 0, 0 },
  { "sat", IALU2, 0x8, { SHORT_A }, IU, 0, 0, 0 },
  { "sat2h", IALU2, 0x9, { SHORT_A }, IU, 0, 0, 0 },
  { "sathl", IALU2, 0x1c, { SHORT_A }, IU, 0, 0, 0 },
  { "sathh", IALU2, 0x1d, { SHORT_A }, IU, 0, 0, 0 },
  { "satz", IALU2, 0xa, { SHORT_A }, IU, 0, 0, 0 },
  { "satz2h", IALU2, 0xb, { SHORT_A }, IU, 0, 0, 0 },
  { "sra", LOGIC, 0x10, { SHORT_A }, EITHER, 0, 0, 0 },
  { "sra2h", LOGIC, 0x11, { SHORT_A }, EITHER, 0, 0, 0 },
  { "src", LOGIC, 0x16, { SHORT_A }, EITHER, 0, 0, 0 },
  { "srl", LOGIC, 0x12, { SHORT_A }, EITHER, 0, 0, 0 },
  { "srl2h", LOGIC, 0x12, { SHORT_A }, EITHER, 0, 0, 0 },
  { "st2h", IMEM, 0x13, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "st2w", IMEM, 0x16, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "st4hb", IMEM, 0x15, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "stb", IMEM, 0x10, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "sth", IMEM, 0x12, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "sthh", IMEM, 0x11, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "stw", IMEM, 0x14, { SHORT_M, LONG_M }, MU, 0, FLAG_MEM, 0 },
  { "sub", IALU1, 0x2, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "sub2h", IALU1, 0x3, { SHORT_A, LONG}, EITHER, 0, 0, 0 },
  { "subb", IALU1, 0x5, { SHORT_A, LONG}, EITHER, FLAG_C, FLAG_CVVA, 0 },
  { "subhlll", IALU1, 0x18, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhllh", IALU1, 0x19, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhlhl", IALU1, 0x1a, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhlhh", IALU1, 0x1b, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhhll", IALU1, 0x1c, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhhlh", IALU1, 0x1d, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhhhl", IALU1, 0x1e, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "subhhhh", IALU1, 0x1f, { SHORT_A, LONG}, EITHER, 0, FLAG_CVVA, 0 },
  { "trap", BRA, 0x9, { SHORT_B1, SHORT_T}, MU, 0, FLAG_SM, 0 },
  { "xor", LOGIC, 0x1b, { SHORT_A, LONG }, EITHER, 0, 0, 0 },
  { "xorfg", LOGIC, 0xb, { SHORT_F }, EITHER, FLAG_X, FLAG_X, 0 },
  { NULL, 0, 0, { 0 }, 0, 0, 0, 0 },
};


/* now define the operand types */
/* format is length, bits, position, flags */
const struct d30v_operand d30v_operand_table[] =
{
#define UNUSED	(0)
  { 0, 0, 0, 0 },
#define Ra	(UNUSED + 1)
  { 6, 6, 0, OPERAND_REG|OPERAND_DEST },
#define Rb	(Ra + 1)
  { 6, 6, 6, OPERAND_REG },
#define Rc	(Rb + 1)
  { 6, 6, 12, OPERAND_REG },
#define Aa	(Rc + 1)
  { 6, 1, 0, OPERAND_ACC|OPERAND_REG|OPERAND_DEST },
#define Ab	(Aa + 1)
  { 6, 1, 6, OPERAND_ACC|OPERAND_REG },
#define IMM5	(Ab + 1)
  { 6, 5, 12, OPERAND_NUM|OPERAND_SIGNED },
#define IMM5S3	(IMM5 + 1)
  { 6, 5, 12, OPERAND_NUM|OPERAND_SIGNED },
#define IMM6	(IMM5S3 + 1)
  { 6, 6, 12, OPERAND_NUM|OPERAND_SIGNED },
#define IMM12	(IMM6 + 1)
  { 12, 12, 6, OPERAND_NUM|OPERAND_SIGNED },
#define IMM12S3	(IMM12 + 1)
  { 12, 12, 6, OPERAND_NUM|OPERAND_SIGNED|OPERAND_SHIFT },
#define IMM18S3	(IMM12S3 + 1)
  { 18, 18, 12, OPERAND_NUM|OPERAND_SIGNED|OPERAND_SHIFT },
#define IMM32	(IMM18S3 + 1)
  { 32, 32, 0, OPERAND_NUM },
#define Fa	(IMM32 + 1)
  { 6, 3, 0, OPERAND_REG | OPERAND_FLAG | OPERAND_DEST },
#define Fb	(Fa + 1)
  { 6, 3, 6, OPERAND_REG | OPERAND_FLAG },
#define Fc	(Fb + 1)
  { 6, 3, 12, OPERAND_REG | OPERAND_FLAG },
#define ATSIGN	(Fc + 1)
  { 0, 0, 0, OPERAND_ATSIGN},
#define ATPAR	(ATSIGN + 1)	/* "@(" */
  { 0, 0, 0, OPERAND_ATPAR},
#define PLUS	(ATPAR + 1)	/* postincrement */
  { 0, 0, 0, OPERAND_PLUS},
#define MINUS	(PLUS + 1)	/* postdecrement */
  { 0, 0, 0, OPERAND_MINUS},
#define ATMINUS	(MINUS + 1)	/* predecrement */
  { 0, 0, 0, OPERAND_ATMINUS},
#define Ca	(ATMINUS + 1)	/* control register */
  { 6, 4, 0, OPERAND_REG|OPERAND_CONTROL|OPERAND_DEST},
#define Cb	(Ca + 1)	/* control register */
  { 6, 4, 6, OPERAND_REG|OPERAND_CONTROL},
#define CC	(Cb + 1)	/* condition code (CMPcc and CMPUcc) */
  { 3, 3, -3, OPERAND_NAME},
#define Fa2	(CC + 1)	/* flag register (CMPcc and CMPUcc) */
  { 3, 3, 0, OPERAND_REG|OPERAND_FLAG|OPERAND_DEST},
#define Fake	(Fa2 + 1)	/* place holder for "id" field in mvfsys and mvtsys */
  { 6, 2, 12, OPERAND_SPECIAL},
};

/* now we need to define the instruction formats */
const struct d30v_format d30v_format_table[] =
{
  { 0, 0, { 0 } },
  { SHORT_M, 0, { Ra, ATPAR, Rb, Rc } },	/* Ra,@(Rb,Rc) */
  { SHORT_M, 1, { Ra, ATPAR, Rb, PLUS, Rc } },	/* Ra,@(Rb+,Rc) */
  { SHORT_M, 2, { Ra, ATPAR, Rb, IMM6 } },	/* Ra,@(Rb,imm6) */
  { SHORT_M, 3, { Ra, ATPAR, Rb, MINUS, Rc } },	/* Ra,@(Rb-,Rc) */
  { SHORT_A, 0, { Ra, Rb, Rc } },		/* Ra,Rb,Rc */
  { SHORT_A, 2, { Ra, Rb, IMM6 } },		/* Ra,Rb,imm6 */
  { SHORT_B1, 0, { Rc } },			/* Rc */
  { SHORT_B2, 2, { IMM18S3 } },			/* imm18 */
  { SHORT_B3, 0, { Ra, Rc } },			/* Ra,Rc */
  { SHORT_B3, 2, { Ra, IMM12 } },		/* Ra,imm12 */
  { SHORT_B3b, 1, { Ra, Rc } },			/* Ra,Rc */
  { SHORT_B3b, 3, { Ra, IMM12 } },		/* Ra,imm12 */
  { SHORT_D1, 0, { Ra, Rc } },			/* Ra,Rc */
  { SHORT_D1, 2, { Ra, IMM12S3 } },		/* Ra,imm12s3 */
  { SHORT_D2, 0, { IMM6, Rc } },		/* imm6,Rc */
  { SHORT_D2, 2, { IMM6, IMM12S3 } },		/* imm6,imm12s3 */
  { SHORT_U, 0, { Ra, Rb } },			/* Ra,Rb */
  { SHORT_U, 2, { Ra, IMM12 } },		/* Ra,imm12 (repeat) */
  { SHORT_F, 0, { Fa, Fb, Fc } },		/* Fa,Fb,Fc  (orfg, xorfg) */
  { SHORT_F, 2, { Fa, Fb, IMM6 } },		/* Fa,Fb,imm6 */
  { SHORT_AF, 0, { Fa, Rb, Rc } },		/* Fa,Rb,Rc */
  { SHORT_AF, 2, { Fa, Rb, IMM6 } },		/* Fa,Rb,imm6 */
  { SHORT_T, 2, { IMM5S3 } },			/* imm5s3   (trap) */
  { SHORT_A5, 2, { Ra, Rb, IMM5 } },		/* Ra,Rb,imm5    (sat*) */
  { SHORT_CMP, 0, { CC, Fa2, Rb, Rc} },		/* CC  Fa2,Rb,Rc */
  { SHORT_CMP, 2, { CC, Fa2, Rb, IMM6} },	/* CC  Fa2,Rb,imm6 */
  { SHORT_A1, 1, { Ra, Rb, Rc } },		/* Ra,Rb,Rc for MAC where a=1 */
  { SHORT_A1, 3, { Ra, Rb, IMM6 } },		/* Ra,Rb,imm6 for MAC where a=1 */
  { SHORT_AA, 0, { Aa, Rb, Rc } },		/* Aa,Rb,Rc */
  { SHORT_AA, 2, { Aa, Rb, IMM6 } },		/* Aa,Rb,imm6 */
  { SHORT_RA, 0, { Ra, Ab, Rc } },		/* Ra,Ab,Rc */
  { SHORT_RA, 2, { Ra, Ab, IMM5 } },		/* Ra,Ab,imm5 */
  { SHORT_MODINC, 1, { Rb, IMM5 } },		/* Rb,imm5 (modinc) */
  { SHORT_MODDEC, 3, { Rb, IMM5 } },		/* Rb,imm5 (moddec) */
  { SHORT_C1, 2, { Ra, Cb, Fake } },		/* Ra,Cb (mvfsys) */
  { SHORT_C2, 2, { Ca, Rb, Fake } },		/* Ca,Rb (mvtsys) */
  { SHORT_UF, 0, { Fa, Fb } },			/* Fa,Fb  (notfg) */
  { SHORT_NONE, 0, { 0 } },			/* no operands (nop, reit) */
  { LONG, 2, { Ra, Rb, IMM32 } },		/* Ra,Rb,imm32 */
  { LONG_U, 2, { IMM32 } },			/* imm32 */
  { LONG_AF, 2, { Fa, Rb, IMM32 } },		/* Fa,Rb,imm32 */
  { LONG_CMP, 2, { CC, Fa2, Rb, IMM32} },	/* CC  Fa2,Rb,imm32 */
  { LONG_M, 2, { Ra, ATPAR, Rb, IMM32 } },	/* Ra,@(Rb,imm32) */
  { LONG_2, 2, { Ra, IMM32 } },			/* Ra,imm32 */
  { LONG_2b, 3, { Ra, IMM32 } },		/* Ra,imm32 */
  { LONG_D, 2, { IMM6, IMM32 } },		/* imm6,imm32 */
  { 0, 0, { 0 } },
};

const char *d30v_ecc_names[] =
{
  "al",
  "tx",
  "fx",
  "xt",
  "xf",
  "tt",
  "tf",
  "res"
};

const char *d30v_cc_names[] =
{
  "eq",
  "ne",
  "gt",
  "ge",
  "lt",
  "le",
  "ps",
  "ng",
  NULL
};
