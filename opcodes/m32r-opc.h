/* Instruction description for m32r.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef m32r_OPC_H
#define m32r_OPC_H

#define CGEN_ARCH m32r
/* Given symbol S, return m32r_cgen_<s>.  */
#define CGEN_SYM(s) CGEN_CAT3 (m32r,_cgen_,s)

#define CGEN_WORD_BITSIZE 32
#define CGEN_DEFAULT_INSN_BITSIZE 32
#define CGEN_BASE_INSN_BITSIZE 32
#define CGEN_MAX_INSN_BITSIZE 32
#define CGEN_DEFAULT_INSN_SIZE (CGEN_DEFAULT_INSN_BITSIZE / 8)
#define CGEN_BASE_INSN_SIZE (CGEN_BASE_INSN_BITSIZE / 8)
#define CGEN_MAX_INSN_SIZE (CGEN_MAX_INSN_BITSIZE / 8)
#define CGEN_INT_INSN

/* +1 because the first entry is reserved (null) */
#define CGEN_NUM_INSNS (127 + 1)
#define CGEN_NUM_OPERANDS (21)

/* Number of non-boolean attributes. */
#define CGEN_MAX_INSN_ATTRS 0
#define CGEN_MAX_OPERAND_ATTRS 0

/* FIXME: Need to compute CGEN_MAX_SYNTAX_BYTES.  */

/* CGEN_MNEMONIC_OPERANDS is defined if mnemonics have operands.
   e.g. In "b,a foo" the ",a" is an operand.  If mnemonics have operands
   we can't hash on everything up to the space.  */
#define CGEN_MNEMONIC_OPERANDS

/* Number of architecture variants.  */
#define MAX_MACHS 1

/* Enums.  */

/* Enum declaration for insn format enums.  */
typedef enum insn_op1 {
  OP1_0 = 0, OP1_1 = 1, OP1_2 = 2, OP1_3 = 3,
  OP1_4 = 4, OP1_5 = 5, OP1_6 = 6, OP1_7 = 7,
  OP1_8 = 8, OP1_9 = 9, OP1_10 = 10, OP1_11 = 11,
  OP1_12 = 12, OP1_13 = 13, OP1_14 = 14, OP1_15 = 15
} INSN_OP1;

/* Enum declaration for op2 enums.  */
typedef enum insn_op2 {
  OP2_0 = 0, OP2_1 = 1, OP2_2 = 2, OP2_3 = 3,
  OP2_4 = 4, OP2_5 = 5, OP2_6 = 6, OP2_7 = 7,
  OP2_8 = 8, OP2_9 = 9, OP2_10 = 10, OP2_11 = 11,
  OP2_12 = 12, OP2_13 = 13, OP2_14 = 14, OP2_15 = 15
} INSN_OP2;

/* Enum declaration for m32r operand types.  */
typedef enum cgen_operand_type {
  M32R_OPERAND_PC = 0, M32R_OPERAND_SR = 1, M32R_OPERAND_DR = 2, M32R_OPERAND_SRC1 = 3,
  M32R_OPERAND_SRC2 = 4, M32R_OPERAND_SCR = 5, M32R_OPERAND_DCR = 6, M32R_OPERAND_SIMM8 = 7,
  M32R_OPERAND_SIMM16 = 8, M32R_OPERAND_UIMM4 = 9, M32R_OPERAND_UIMM5 = 10, M32R_OPERAND_UIMM16 = 11,
  M32R_OPERAND_HI16 = 12, M32R_OPERAND_SLO16 = 13, M32R_OPERAND_ULO16 = 14, M32R_OPERAND_UIMM24 = 15,
  M32R_OPERAND_DISP8 = 16, M32R_OPERAND_DISP16 = 17, M32R_OPERAND_DISP24 = 18, M32R_OPERAND_CONDBIT = 19,
  M32R_OPERAND_ACCUM = 20
} CGEN_OPERAND_TYPE;

/* Non-boolean attributes.  */

/* Enum declaration for machine type selection.  */
typedef enum mach_attr {
  MACH_M32R = 0
} MACH_ATTR;

/* Operand and instruction attribute indices.  */

/* Enum declaration for cgen_operand attrs.  */
typedef enum cgen_operand_attr {
  CGEN_OPERAND_ABS_ADDR, CGEN_OPERAND_FAKE, CGEN_OPERAND_NEGATIVE, CGEN_OPERAND_PC,
  CGEN_OPERAND_PCREL_ADDR, CGEN_OPERAND_RELAX, CGEN_OPERAND_RELOC, CGEN_OPERAND_SIGN_OPT,
  CGEN_OPERAND_UNSIGNED
} CGEN_OPERAND_ATTR;

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_ALIAS, CGEN_INSN_COND_CTI, CGEN_INSN_FILL_SLOT, CGEN_INSN_RELAX,
  CGEN_INSN_RELAX_BC, CGEN_INSN_RELAX_BL, CGEN_INSN_RELAX_BNC, CGEN_INSN_RELAX_BRA,
  CGEN_INSN_RELAXABLE, CGEN_INSN_UNCOND_CTI
} CGEN_INSN_ATTR;

/* Insn types are used by the simulator.  */
/* Enum declaration for m32r instruction types.  */
typedef enum cgen_insn_type {
  M32R_INSN_ILLEGAL, M32R_INSN_ADD, M32R_INSN_ADD3, M32R_INSN_AND,
  M32R_INSN_AND3, M32R_INSN_OR, M32R_INSN_OR3, M32R_INSN_XOR,
  M32R_INSN_XOR3, M32R_INSN_ADDI, M32R_INSN_ADDV, M32R_INSN_ADDV3,
  M32R_INSN_ADDX, M32R_INSN_BC8, M32R_INSN_BC8_S, M32R_INSN_BC24,
  M32R_INSN_BC24_L, M32R_INSN_BEQ, M32R_INSN_BEQZ, M32R_INSN_BGEZ,
  M32R_INSN_BGTZ, M32R_INSN_BLEZ, M32R_INSN_BLTZ, M32R_INSN_BNEZ,
  M32R_INSN_BL8, M32R_INSN_BL8_S, M32R_INSN_BL24, M32R_INSN_BL24_L,
  M32R_INSN_BNC8, M32R_INSN_BNC8_S, M32R_INSN_BNC24, M32R_INSN_BNC24_L,
  M32R_INSN_BNE, M32R_INSN_BRA8, M32R_INSN_BRA8_S, M32R_INSN_BRA24,
  M32R_INSN_BRA24_L, M32R_INSN_CMP, M32R_INSN_CMPI, M32R_INSN_CMPU,
  M32R_INSN_CMPUI, M32R_INSN_DIV, M32R_INSN_DIVU, M32R_INSN_REM,
  M32R_INSN_REMU, M32R_INSN_JL, M32R_INSN_JMP, M32R_INSN_LD,
  M32R_INSN_LD_2, M32R_INSN_LD_D, M32R_INSN_LD_D2, M32R_INSN_LDB,
  M32R_INSN_LDB_2, M32R_INSN_LDB_D, M32R_INSN_LDB_D2, M32R_INSN_LDH,
  M32R_INSN_LDH_2, M32R_INSN_LDH_D, M32R_INSN_LDH_D2, M32R_INSN_LDUB,
  M32R_INSN_LDUB_2, M32R_INSN_LDUB_D, M32R_INSN_LDUB_D2, M32R_INSN_LDUH,
  M32R_INSN_LDUH_2, M32R_INSN_LDUH_D, M32R_INSN_LDUH_D2, M32R_INSN_LD_PLUS,
  M32R_INSN_LD24, M32R_INSN_LDI8, M32R_INSN_LDI8A, M32R_INSN_LDI16,
  M32R_INSN_LDI16A, M32R_INSN_LOCK, M32R_INSN_MACHI, M32R_INSN_MACLO,
  M32R_INSN_MACWHI, M32R_INSN_MACWLO, M32R_INSN_MUL, M32R_INSN_MULHI,
  M32R_INSN_MULLO, M32R_INSN_MULWHI, M32R_INSN_MULWLO, M32R_INSN_MV,
  M32R_INSN_MVFACHI, M32R_INSN_MVFACLO, M32R_INSN_MVFACMI, M32R_INSN_MVFC,
  M32R_INSN_MVTACHI, M32R_INSN_MVTACLO, M32R_INSN_MVTC, M32R_INSN_NEG,
  M32R_INSN_NOP, M32R_INSN_NOT, M32R_INSN_RAC, M32R_INSN_RACH,
  M32R_INSN_RTE, M32R_INSN_SETH, M32R_INSN_SLL, M32R_INSN_SLL3,
  M32R_INSN_SLLI, M32R_INSN_SRA, M32R_INSN_SRA3, M32R_INSN_SRAI,
  M32R_INSN_SRL, M32R_INSN_SRL3, M32R_INSN_SRLI, M32R_INSN_ST,
  M32R_INSN_ST_2, M32R_INSN_ST_D, M32R_INSN_ST_D2, M32R_INSN_STB,
  M32R_INSN_STB_2, M32R_INSN_STB_D, M32R_INSN_STB_D2, M32R_INSN_STH,
  M32R_INSN_STH_2, M32R_INSN_STH_D, M32R_INSN_STH_D2, M32R_INSN_ST_PLUS,
  M32R_INSN_ST_MINUS, M32R_INSN_SUB, M32R_INSN_SUBV, M32R_INSN_SUBX,
  M32R_INSN_TRAP, M32R_INSN_UNLOCK, M32R_INSN_PUSH, M32R_INSN_POP,
  M32R_INSN_MAX
} CGEN_INSN_TYPE;

/* Index of `illegal' insn place holder.  */
#define CGEN_INSN_ILLEGAL M32R_INSN_ILLEGAL
/* Total number of insns in table.  */
#define CGEN_MAX_INSNS ((int) M32R_INSN_MAX)

/* cgen.h uses things we just defined.  */
#include "opcode/cgen.h"

/* This struct records data prior to insertion or after extraction.  */
typedef struct cgen_fields 
{
  long f_nil;
  long f_op1;
  long f_op2;
  long f_cond;
  long f_r1;
  long f_r2;
  long f_simm8;
  long f_simm16;
  long f_shift_op2;
  long f_uimm4;
  long f_uimm5;
  long f_uimm16;
  long f_uimm24;
  long f_hi16;
  long f_disp8;
  long f_disp16;
  long f_disp24;
  int length;
} CGEN_FIELDS;

/* Attributes.  */
extern const CGEN_ATTR_TABLE m32r_cgen_operand_attr_table[];
extern const CGEN_ATTR_TABLE m32r_cgen_insn_attr_table[];

extern CGEN_KEYWORD m32r_cgen_opval_mach;
extern CGEN_KEYWORD m32r_cgen_opval_h_gr;
extern CGEN_KEYWORD m32r_cgen_opval_h_cr;

#define CGEN_INIT_PARSE() \
{\
}
#define CGEN_INIT_INSERT() \
{\
}
#define CGEN_INIT_EXTRACT() \
{\
}
#define CGEN_INIT_PRINT() \
{\
}

/* -- opc.h */

#undef CGEN_DIS_HASH_SIZE
#define CGEN_DIS_HASH_SIZE 256
#undef CGEN_DIS_HASH
#define X(b) (((unsigned char *) (b))[0] & 0xf0)
#define CGEN_DIS_HASH(buffer, insn) \
(X (buffer) | \
 (X (buffer) == 0x40 || X (buffer) == 0xe0 || X (buffer) == 0x60 || X (buffer) == 0x50 ? 0 \
  : X (buffer) == 0x70 || X (buffer) == 0xf0 ? (((unsigned char *) (buffer))[0] & 0xf) \
  : ((((unsigned char *) (buffer))[1] & 0xf0) >> 4)))

/* -- */


#endif /* m32r_OPC_H */
