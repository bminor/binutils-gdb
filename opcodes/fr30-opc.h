/* Instruction description for fr30.

THIS FILE IS MACHINE GENERATED WITH CGEN.

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

#ifndef FR30_OPC_H
#define FR30_OPC_H

#define CGEN_ARCH fr30

/* Given symbol S, return fr30_cgen_<s>.  */
#define CGEN_SYM(s) CONCAT3 (fr30,_cgen_,s)

/* Selected cpu families.  */
#define HAVE_CPU_FR30BF

#define CGEN_INSN_LSB0_P 0
#define CGEN_WORD_BITSIZE 32
#define CGEN_DEFAULT_INSN_BITSIZE 16
#define CGEN_BASE_INSN_BITSIZE 16
#define CGEN_MIN_INSN_BITSIZE 16
#define CGEN_MAX_INSN_BITSIZE 16
#define CGEN_DEFAULT_INSN_SIZE (CGEN_DEFAULT_INSN_BITSIZE / 8)
#define CGEN_BASE_INSN_SIZE (CGEN_BASE_INSN_BITSIZE / 8)
#define CGEN_MIN_INSN_SIZE (CGEN_MIN_INSN_BITSIZE / 8)
#define CGEN_MAX_INSN_SIZE (CGEN_MAX_INSN_BITSIZE / 8)
#define CGEN_INT_INSN_P 1

/* FIXME: Need to compute CGEN_MAX_SYNTAX_BYTES.  */

/* CGEN_MNEMONIC_OPERANDS is defined if mnemonics have operands.
   e.g. In "b,a foo" the ",a" is an operand.  If mnemonics have operands
   we can't hash on everything up to the space.  */
#define CGEN_MNEMONIC_OPERANDS
/* Maximum number of operands any insn or macro-insn has.  */
#define CGEN_MAX_INSN_OPERANDS 16

/* Enums.  */

/* Enum declaration for insn op1 enums.  */
typedef enum insn_op1 {
  OP1_0, OP1_1, OP1_2, OP1_3
 , OP1_4, OP1_5, OP1_6, OP1_7
 , OP1_8, OP1_9, OP1_A, OP1_B
 , OP1_C, OP1_D, OP1_E, OP1_F
} INSN_OP1;

/* Enum declaration for insn op2 enums.  */
typedef enum insn_op2 {
  OP2_0, OP2_1, OP2_2, OP2_3
 , OP2_4, OP2_5, OP2_6, OP2_7
 , OP2_8, OP2_9, OP2_A, OP2_B
 , OP2_C, OP2_D, OP2_E, OP2_F
} INSN_OP2;

/* Enum declaration for insn op3 enums.  */
typedef enum insn_op3 {
  OP3_0, OP3_1, OP3_2, OP3_3
 , OP3_4, OP3_5, OP3_6, OP3_7
 , OP3_8, OP3_9, OP3_A, OP3_B
 , OP3_C, OP3_D, OP3_E, OP3_F
} INSN_OP3;

/* Enum declaration for insn op5 enums.  */
typedef enum insn_op5 {
  OP5_0, OP5_1
} INSN_OP5;

/* Enum declaration for general registers.  */
typedef enum h_gr {
  H_GR_AC = 13, H_GR_FP = 14, H_GR_SP = 15, H_GR_R0 = 0
 , H_GR_R1 = 1, H_GR_R2 = 2, H_GR_R3 = 3, H_GR_R4 = 4
 , H_GR_R5 = 5, H_GR_R6 = 6, H_GR_R7 = 7, H_GR_R8 = 8
 , H_GR_R9 = 9, H_GR_R10 = 10, H_GR_R11 = 11, H_GR_R12 = 12
 , H_GR_R13 = 13, H_GR_R14 = 14, H_GR_R15 = 15
} H_GR;

/* Enum declaration for fr30 operand types.  */
typedef enum cgen_operand_type {
  FR30_OPERAND_PC, FR30_OPERAND_RI, FR30_OPERAND_RJ, FR30_OPERAND_MAX
} CGEN_OPERAND_TYPE;

/* Non-boolean attributes.  */

/* Enum declaration for machine type selection.  */
typedef enum mach_attr {
  MACH_BASE, MACH_FR30, MACH_MAX
} MACH_ATTR;

/* Number of architecture variants.  */
#define MAX_MACHS ((int) MACH_MAX)

/* Number of operands types.  */
#define MAX_OPERANDS ((int) FR30_OPERAND_MAX)

/* Maximum number of operands referenced by any insn.  */
#define MAX_OPERAND_INSTANCES 3

/* Hardware, operand and instruction attribute indices.  */

/* Enum declaration for cgen_hw attrs.  */
typedef enum cgen_hw_attr {
  CGEN_HW_CACHE_ADDR, CGEN_HW_PC, CGEN_HW_PROFILE
} CGEN_HW_ATTR;

/* Number of non-boolean elements in cgen_hw.  */
#define CGEN_HW_NBOOL_ATTRS ((int) CGEN_HW_CACHE_ADDR)

/* Enum declaration for cgen_operand attrs.  */
typedef enum cgen_operand_attr {
  CGEN_OPERAND_ABS_ADDR, CGEN_OPERAND_FAKE, CGEN_OPERAND_NEGATIVE, CGEN_OPERAND_PCREL_ADDR
 , CGEN_OPERAND_RELAX, CGEN_OPERAND_SIGN_OPT, CGEN_OPERAND_UNSIGNED
} CGEN_OPERAND_ATTR;

/* Number of non-boolean elements in cgen_operand.  */
#define CGEN_OPERAND_NBOOL_ATTRS ((int) CGEN_OPERAND_ABS_ADDR)

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_ALIAS, CGEN_INSN_COND_CTI, CGEN_INSN_NO_DIS, CGEN_INSN_RELAX
 , CGEN_INSN_RELAXABLE, CGEN_INSN_SKIP_CTI, CGEN_INSN_UNCOND_CTI, CGEN_INSN_VIRTUAL
} CGEN_INSN_ATTR;

/* Number of non-boolean elements in cgen_insn.  */
#define CGEN_INSN_NBOOL_ATTRS ((int) CGEN_INSN_ALIAS)

/* Enum declaration for fr30 instruction types.  */
typedef enum cgen_insn_type {
  FR30_INSN_INVALID, FR30_INSN_ADD, FR30_INSN_MAX
} CGEN_INSN_TYPE;

/* Index of `invalid' insn place holder.  */
#define CGEN_INSN_INVALID FR30_INSN_INVALID
/* Total number of insns in table.  */
#define MAX_INSNS ((int) FR30_INSN_MAX)

/* cgen.h uses things we just defined.  */
#include "opcode/cgen.h"

/* This struct records data prior to insertion or after extraction.  */
struct cgen_fields 
{
  long f_nil;
  long f_op1;
  long f_op2;
  long f_op3;
  long f_op5;
  long f_Rj;
  long f_Ri;
  long f_Rs;
  long f_u4;
  long f_i4;
  long f_m4;
  long f_u8;
  long f_i8;
  long f_o8;
  long f_rel8;
  long f_dir;
  long f_rlist;
  long f_rel11;
  int length;
};

/* Attributes.  */
extern const CGEN_ATTR_TABLE fr30_cgen_hw_attr_table[];
extern const CGEN_ATTR_TABLE fr30_cgen_operand_attr_table[];
extern const CGEN_ATTR_TABLE fr30_cgen_insn_attr_table[];

/* Enum declaration for fr30 hardware types.  */
typedef enum hw_type {
  HW_H_PC, HW_H_MEMORY, HW_H_SINT, HW_H_UINT
 , HW_H_ADDR, HW_H_IADDR, HW_H_GR, HW_MAX
} HW_TYPE;

#define MAX_HW ((int) HW_MAX)

/* Hardware decls.  */

extern CGEN_KEYWORD fr30_cgen_opval_h_gr;

#define CGEN_INIT_PARSE(od) \
{\
}
#define CGEN_INIT_INSERT(od) \
{\
}
#define CGEN_INIT_EXTRACT(od) \
{\
}
#define CGEN_INIT_PRINT(od) \
{\
}



#endif /* FR30_OPC_H */
