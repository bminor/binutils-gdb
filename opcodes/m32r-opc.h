/* Instruction description for m32r.

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

#ifndef M32R_OPC_H
#define M32R_OPC_H

#define CGEN_ARCH m32r

/* Given symbol S, return m32r_cgen_<S>.  */
#define CGEN_SYM(s) CONCAT3 (m32r,_cgen_,s)

/* Selected cpu families.  */
#define HAVE_CPU_M32RBF
/* start-sanitize-m32rx */
#define HAVE_CPU_M32RXF
/* end-sanitize-m32rx */

#define CGEN_INSN_LSB0_P 0
#define CGEN_WORD_BITSIZE 32
#define CGEN_DEFAULT_INSN_BITSIZE 32
#define CGEN_BASE_INSN_BITSIZE 32
#define CGEN_MIN_INSN_BITSIZE 16
#define CGEN_MAX_INSN_BITSIZE 32
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

/* Maximum number of fields in an instruction.  */
#define CGEN_MAX_IFMT_OPERANDS 7

/* Enums.  */

/* Enum declaration for insn format enums.  */
typedef enum insn_op1 {
  OP1_0, OP1_1, OP1_2, OP1_3
 , OP1_4, OP1_5, OP1_6, OP1_7
 , OP1_8, OP1_9, OP1_10, OP1_11
 , OP1_12, OP1_13, OP1_14, OP1_15
} INSN_OP1;

/* Enum declaration for op2 enums.  */
typedef enum insn_op2 {
  OP2_0, OP2_1, OP2_2, OP2_3
 , OP2_4, OP2_5, OP2_6, OP2_7
 , OP2_8, OP2_9, OP2_10, OP2_11
 , OP2_12, OP2_13, OP2_14, OP2_15
} INSN_OP2;

/* Enum declaration for general registers.  */
typedef enum h_gr {
  H_GR_FP = 13, H_GR_LR = 14, H_GR_SP = 15, H_GR_R0 = 0
 , H_GR_R1 = 1, H_GR_R2 = 2, H_GR_R3 = 3, H_GR_R4 = 4
 , H_GR_R5 = 5, H_GR_R6 = 6, H_GR_R7 = 7, H_GR_R8 = 8
 , H_GR_R9 = 9, H_GR_R10 = 10, H_GR_R11 = 11, H_GR_R12 = 12
 , H_GR_R13 = 13, H_GR_R14 = 14, H_GR_R15 = 15
} H_GR;

/* Enum declaration for control registers.  */
typedef enum h_cr {
  H_CR_PSW = 0, H_CR_CBR = 1, H_CR_SPI = 2, H_CR_SPU = 3
 , H_CR_BPC = 6, H_CR_BBPSW = 8, H_CR_BBPC = 14, H_CR_CR0 = 0
 , H_CR_CR1 = 1, H_CR_CR2 = 2, H_CR_CR3 = 3, H_CR_CR4 = 4
 , H_CR_CR5 = 5, H_CR_CR6 = 6, H_CR_CR7 = 7, H_CR_CR8 = 8
 , H_CR_CR9 = 9, H_CR_CR10 = 10, H_CR_CR11 = 11, H_CR_CR12 = 12
 , H_CR_CR13 = 13, H_CR_CR14 = 14, H_CR_CR15 = 15
} H_CR;

/* start-sanitize-m32rx */
/* Enum declaration for accumulators.  */
typedef enum h_accums {
  H_ACCUMS_A0, H_ACCUMS_A1
} H_ACCUMS;

/* end-sanitize-m32rx */
/* Attributes.  */

/* Enum declaration for machine type selection.  */
typedef enum mach_attr {
  MACH_BASE, MACH_M32R
/* start-sanitize-m32rx */
 , MACH_M32RX
/* end-sanitize-m32rx */
 , MACH_MAX
} MACH_ATTR;

/* start-sanitize-m32rx */
/* Enum declaration for parallel execution pipeline selection.  */
typedef enum pipe_attr {
  PIPE_NONE, PIPE_O, PIPE_S, PIPE_OS
} PIPE_ATTR;

/* end-sanitize-m32rx */
/* Number of architecture variants.  */
#define MAX_MACHS ((int) MACH_MAX)

/* Ifield attribute indices.  */

/* Enum declaration for cgen_ifld attrs.  */
typedef enum cgen_ifld_attr {
  CGEN_IFLD_MACH, CGEN_IFLD_VIRTUAL, CGEN_IFLD_UNSIGNED, CGEN_IFLD_PCREL_ADDR
 , CGEN_IFLD_ABS_ADDR, CGEN_IFLD_RESERVED, CGEN_IFLD_SIGN_OPT, CGEN_IFLD_RELOC
} CGEN_IFLD_ATTR;

/* Number of non-boolean elements in cgen_ifld.  */
#define CGEN_IFLD_NBOOL_ATTRS ((int) CGEN_IFLD_VIRTUAL)

/* Enum declaration for m32r ifield types.  */
typedef enum ifield_type {
  M32R_F_NIL, M32R_F_OP1, M32R_F_OP2, M32R_F_COND
 , M32R_F_R1, M32R_F_R2, M32R_F_SIMM8, M32R_F_SIMM16
 , M32R_F_SHIFT_OP2, M32R_F_UIMM4, M32R_F_UIMM5, M32R_F_UIMM16
 , M32R_F_UIMM24, M32R_F_HI16, M32R_F_DISP8, M32R_F_DISP16
 , M32R_F_DISP24
/* start-sanitize-m32rx */
 , M32R_F_OP23
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_OP3
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_ACC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_ACCS
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_ACCD
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_BITS67
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_BIT14
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_F_IMM1
/* end-sanitize-m32rx */
 , M32R_F_MAX
} IFIELD_TYPE;

#define MAX_IFLD ((int) M32R_F_MAX)

/* Hardware attribute indices.  */

/* Enum declaration for cgen_hw attrs.  */
typedef enum cgen_hw_attr {
  CGEN_HW_MACH, CGEN_HW_VIRTUAL, CGEN_HW_UNSIGNED, CGEN_HW_SIGNED
 , CGEN_HW_CACHE_ADDR, CGEN_HW_FUN_ACCESS, CGEN_HW_PC, CGEN_HW_PROFILE
} CGEN_HW_ATTR;

/* Number of non-boolean elements in cgen_hw.  */
#define CGEN_HW_NBOOL_ATTRS ((int) CGEN_HW_VIRTUAL)

/* Enum declaration for m32r hardware types.  */
typedef enum hw_type {
  HW_H_PC, HW_H_MEMORY, HW_H_SINT, HW_H_UINT
 , HW_H_ADDR, HW_H_IADDR, HW_H_HI16, HW_H_SLO16
 , HW_H_ULO16, HW_H_GR, HW_H_CR, HW_H_ACCUM
/* start-sanitize-m32rx */
 , HW_H_ACCUMS
/* end-sanitize-m32rx */
 , HW_H_COND, HW_H_PSW, HW_H_BPSW, HW_H_BBPSW
 , HW_H_LOCK, HW_MAX
} HW_TYPE;

#define MAX_HW ((int) HW_MAX)

/* Operand attribute indices.  */

/* Enum declaration for cgen_operand attrs.  */
typedef enum cgen_operand_attr {
  CGEN_OPERAND_MACH, CGEN_OPERAND_VIRTUAL, CGEN_OPERAND_UNSIGNED, CGEN_OPERAND_PCREL_ADDR
 , CGEN_OPERAND_ABS_ADDR, CGEN_OPERAND_SIGN_OPT, CGEN_OPERAND_NEGATIVE, CGEN_OPERAND_RELAX
 , CGEN_OPERAND_SEM_ONLY, CGEN_OPERAND_RELOC, CGEN_OPERAND_HASH_PREFIX
} CGEN_OPERAND_ATTR;

/* Number of non-boolean elements in cgen_operand.  */
#define CGEN_OPERAND_NBOOL_ATTRS ((int) CGEN_OPERAND_VIRTUAL)

/* Enum declaration for m32r operand types.  */
typedef enum cgen_operand_type {
  M32R_OPERAND_PC, M32R_OPERAND_SR, M32R_OPERAND_DR, M32R_OPERAND_SRC1
 , M32R_OPERAND_SRC2, M32R_OPERAND_SCR, M32R_OPERAND_DCR, M32R_OPERAND_SIMM8
 , M32R_OPERAND_SIMM16, M32R_OPERAND_UIMM4, M32R_OPERAND_UIMM5, M32R_OPERAND_UIMM16
/* start-sanitize-m32rx */
 , M32R_OPERAND_IMM1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_OPERAND_ACCD
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_OPERAND_ACCS
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_OPERAND_ACC
/* end-sanitize-m32rx */
 , M32R_OPERAND_HASH, M32R_OPERAND_HI16, M32R_OPERAND_SLO16, M32R_OPERAND_ULO16
 , M32R_OPERAND_UIMM24, M32R_OPERAND_DISP8, M32R_OPERAND_DISP16, M32R_OPERAND_DISP24
 , M32R_OPERAND_CONDBIT, M32R_OPERAND_ACCUM, M32R_OPERAND_MAX
} CGEN_OPERAND_TYPE;

/* Number of operands types.  */
#define MAX_OPERANDS ((int) M32R_OPERAND_MAX)

/* Maximum number of operands referenced by any insn.  */
#define MAX_OPERAND_INSTANCES 11

/* Insn attribute indices.  */

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_MACH
/* start-sanitize-m32rx */
 , CGEN_INSN_PIPE
/* end-sanitize-m32rx */
 , CGEN_INSN_VIRTUAL, CGEN_INSN_UNCOND_CTI, CGEN_INSN_COND_CTI, CGEN_INSN_SKIP_CTI
 , CGEN_INSN_DELAY_SLOT, CGEN_INSN_RELAXABLE, CGEN_INSN_RELAX, CGEN_INSN_ALIAS
 , CGEN_INSN_NO_DIS, CGEN_INSN_PBB, CGEN_INSN_FILL_SLOT
/* start-sanitize-m32rx */
 , CGEN_INSN_SPECIAL
/* end-sanitize-m32rx */
} CGEN_INSN_ATTR;

/* Number of non-boolean elements in cgen_insn.  */
#define CGEN_INSN_NBOOL_ATTRS ((int) CGEN_INSN_VIRTUAL)

/* Enum declaration for m32r instruction types.  */
typedef enum cgen_insn_type {
  M32R_INSN_INVALID, M32R_INSN_ADD, M32R_INSN_ADD3, M32R_INSN_AND
 , M32R_INSN_AND3, M32R_INSN_OR, M32R_INSN_OR3, M32R_INSN_XOR
 , M32R_INSN_XOR3, M32R_INSN_ADDI, M32R_INSN_ADDV, M32R_INSN_ADDV3
 , M32R_INSN_ADDX, M32R_INSN_BC8, M32R_INSN_BC24, M32R_INSN_BEQ
 , M32R_INSN_BEQZ, M32R_INSN_BGEZ, M32R_INSN_BGTZ, M32R_INSN_BLEZ
 , M32R_INSN_BLTZ, M32R_INSN_BNEZ, M32R_INSN_BL8, M32R_INSN_BL24
/* start-sanitize-m32rx */
 , M32R_INSN_BCL8
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_BCL24
/* end-sanitize-m32rx */
 , M32R_INSN_BNC8, M32R_INSN_BNC24, M32R_INSN_BNE, M32R_INSN_BRA8
 , M32R_INSN_BRA24
/* start-sanitize-m32rx */
 , M32R_INSN_BNCL8
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_BNCL24
/* end-sanitize-m32rx */
 , M32R_INSN_CMP, M32R_INSN_CMPI, M32R_INSN_CMPU, M32R_INSN_CMPUI
/* start-sanitize-m32rx */
 , M32R_INSN_CMPEQ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_CMPZ
/* end-sanitize-m32rx */
 , M32R_INSN_DIV, M32R_INSN_DIVU, M32R_INSN_REM, M32R_INSN_REMU
/* start-sanitize-m32rx */
 , M32R_INSN_DIVH
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_JC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_JNC
/* end-sanitize-m32rx */
 , M32R_INSN_JL, M32R_INSN_JMP, M32R_INSN_LD, M32R_INSN_LD_D
 , M32R_INSN_LDB, M32R_INSN_LDB_D, M32R_INSN_LDH, M32R_INSN_LDH_D
 , M32R_INSN_LDUB, M32R_INSN_LDUB_D, M32R_INSN_LDUH, M32R_INSN_LDUH_D
 , M32R_INSN_LD_PLUS, M32R_INSN_LD24, M32R_INSN_LDI8, M32R_INSN_LDI16
 , M32R_INSN_LOCK, M32R_INSN_MACHI
/* start-sanitize-m32rx */
 , M32R_INSN_MACHI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MACLO
/* start-sanitize-m32rx */
 , M32R_INSN_MACLO_A
/* end-sanitize-m32rx */
 , M32R_INSN_MACWHI
/* start-sanitize-m32rx */
 , M32R_INSN_MACWHI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MACWLO
/* start-sanitize-m32rx */
 , M32R_INSN_MACWLO_A
/* end-sanitize-m32rx */
 , M32R_INSN_MUL, M32R_INSN_MULHI
/* start-sanitize-m32rx */
 , M32R_INSN_MULHI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MULLO
/* start-sanitize-m32rx */
 , M32R_INSN_MULLO_A
/* end-sanitize-m32rx */
 , M32R_INSN_MULWHI
/* start-sanitize-m32rx */
 , M32R_INSN_MULWHI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MULWLO
/* start-sanitize-m32rx */
 , M32R_INSN_MULWLO_A
/* end-sanitize-m32rx */
 , M32R_INSN_MV, M32R_INSN_MVFACHI
/* start-sanitize-m32rx */
 , M32R_INSN_MVFACHI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MVFACLO
/* start-sanitize-m32rx */
 , M32R_INSN_MVFACLO_A
/* end-sanitize-m32rx */
 , M32R_INSN_MVFACMI
/* start-sanitize-m32rx */
 , M32R_INSN_MVFACMI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MVFC, M32R_INSN_MVTACHI
/* start-sanitize-m32rx */
 , M32R_INSN_MVTACHI_A
/* end-sanitize-m32rx */
 , M32R_INSN_MVTACLO
/* start-sanitize-m32rx */
 , M32R_INSN_MVTACLO_A
/* end-sanitize-m32rx */
 , M32R_INSN_MVTC, M32R_INSN_NEG, M32R_INSN_NOP, M32R_INSN_NOT
 , M32R_INSN_RAC
/* start-sanitize-m32rx */
 , M32R_INSN_RAC_DSI
/* end-sanitize-m32rx */
 , M32R_INSN_RACH
/* start-sanitize-m32rx */
 , M32R_INSN_RACH_DSI
/* end-sanitize-m32rx */
 , M32R_INSN_RTE, M32R_INSN_SETH, M32R_INSN_SLL, M32R_INSN_SLL3
 , M32R_INSN_SLLI, M32R_INSN_SRA, M32R_INSN_SRA3, M32R_INSN_SRAI
 , M32R_INSN_SRL, M32R_INSN_SRL3, M32R_INSN_SRLI, M32R_INSN_ST
 , M32R_INSN_ST_D, M32R_INSN_STB, M32R_INSN_STB_D, M32R_INSN_STH
 , M32R_INSN_STH_D, M32R_INSN_ST_PLUS, M32R_INSN_ST_MINUS, M32R_INSN_SUB
 , M32R_INSN_SUBV, M32R_INSN_SUBX, M32R_INSN_TRAP, M32R_INSN_UNLOCK
/* start-sanitize-m32rx */
 , M32R_INSN_SATB
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_SATH
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_SAT
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_PCMPBZ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_SADD
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_MACWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_MSBLO
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_MULWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_MACLH1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_SC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32R_INSN_SNC
/* end-sanitize-m32rx */
 , M32R_INSN_MAX
} CGEN_INSN_TYPE;

/* Index of `invalid' insn place holder.  */
#define CGEN_INSN_INVALID M32R_INSN_INVALID
/* Total number of insns in table.  */
#define MAX_INSNS ((int) M32R_INSN_MAX)

/* cgen.h uses things we just defined.  */
#include "opcode/cgen.h"

/* This struct records data prior to insertion or after extraction.  */
struct cgen_fields 
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
/* start-sanitize-m32rx */
  long f_op23;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_op3;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_acc;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_accs;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_accd;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_bits67;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_bit14;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
  long f_imm1;
/* end-sanitize-m32rx */
  int length;
};

/* Attributes.  */
extern const CGEN_ATTR_TABLE m32r_cgen_hw_attr_table[];
extern const CGEN_ATTR_TABLE m32r_cgen_operand_attr_table[];
extern const CGEN_ATTR_TABLE m32r_cgen_insn_attr_table[];

/* Hardware decls.  */

extern CGEN_KEYWORD m32r_cgen_opval_h_gr;
extern CGEN_KEYWORD m32r_cgen_opval_h_cr;
/* start-sanitize-m32rx */
extern CGEN_KEYWORD m32r_cgen_opval_h_accums;
/* end-sanitize-m32rx */

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

/* -- opc.h */

#undef CGEN_DIS_HASH_SIZE
#define CGEN_DIS_HASH_SIZE 256
#undef CGEN_DIS_HASH
#define X(b) (((unsigned char *) (b))[0] & 0xf0)
#define CGEN_DIS_HASH(buffer, value) \
(X (buffer) | \
 (X (buffer) == 0x40 || X (buffer) == 0xe0 || X (buffer) == 0x60 || X (buffer) == 0x50 ? 0 \
  : X (buffer) == 0x70 || X (buffer) == 0xf0 ? (((unsigned char *) (buffer))[0] & 0xf) \
  : X (buffer) == 0x30 ? ((((unsigned char *) (buffer))[1] & 0x70) >> 4) \
  : ((((unsigned char *) (buffer))[1] & 0xf0) >> 4)))

/* -- */


#endif /* M32R_OPC_H */
