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

/* Given symbol S, return fr30_cgen_<S>.  */
#define CGEN_SYM(s) CONCAT3 (fr30,_cgen_,s)

/* Selected cpu families.  */
#define HAVE_CPU_FR30BF

#define CGEN_INSN_LSB0_P 0
#define CGEN_WORD_BITSIZE 32
#define CGEN_DEFAULT_INSN_BITSIZE 16
#define CGEN_BASE_INSN_BITSIZE 16
#define CGEN_MIN_INSN_BITSIZE 16
#define CGEN_MAX_INSN_BITSIZE 48
#define CGEN_DEFAULT_INSN_SIZE (CGEN_DEFAULT_INSN_BITSIZE / 8)
#define CGEN_BASE_INSN_SIZE (CGEN_BASE_INSN_BITSIZE / 8)
#define CGEN_MIN_INSN_SIZE (CGEN_MIN_INSN_BITSIZE / 8)
#define CGEN_MAX_INSN_SIZE (CGEN_MAX_INSN_BITSIZE / 8)
#define CGEN_INT_INSN_P 0

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

/* Enum declaration for insn op4 enums.  */
typedef enum insn_op4 {
  OP4_0
} INSN_OP4;

/* Enum declaration for insn op5 enums.  */
typedef enum insn_op5 {
  OP5_0, OP5_1
} INSN_OP5;

/* Enum declaration for insn cc enums.  */
typedef enum insn_cc {
  CC_RA, CC_NO, CC_EQ, CC_NE
 , CC_C, CC_NC, CC_N, CC_P
 , CC_V, CC_NV, CC_LT, CC_GE
 , CC_LE, CC_GT, CC_LS, CC_HI
} INSN_CC;

/* Enum declaration for general registers.  */
typedef enum h_gr {
  H_GR_AC = 13, H_GR_FP = 14, H_GR_SP = 15, H_GR_R0 = 0
 , H_GR_R1 = 1, H_GR_R2 = 2, H_GR_R3 = 3, H_GR_R4 = 4
 , H_GR_R5 = 5, H_GR_R6 = 6, H_GR_R7 = 7, H_GR_R8 = 8
 , H_GR_R9 = 9, H_GR_R10 = 10, H_GR_R11 = 11, H_GR_R12 = 12
 , H_GR_R13 = 13, H_GR_R14 = 14, H_GR_R15 = 15
} H_GR;

/* Enum declaration for coprocessor registers.  */
typedef enum h_cr {
  H_CR_CR0, H_CR_CR1, H_CR_CR2, H_CR_CR3
 , H_CR_CR4, H_CR_CR5, H_CR_CR6, H_CR_CR7
 , H_CR_CR8, H_CR_CR9, H_CR_CR10, H_CR_CR11
 , H_CR_CR12, H_CR_CR13, H_CR_CR14, H_CR_CR15
} H_CR;

/* Enum declaration for dedicated registers.  */
typedef enum h_dr {
  H_DR_TBR, H_DR_RP, H_DR_SSP, H_DR_USP
 , H_DR_MDH, H_DR_MDL
} H_DR;

/* Enum declaration for program status.  */
typedef enum h_ps {
  H_PS_PS
} H_PS;

/* Enum declaration for General Register 13 explicitely required.  */
typedef enum h_r13 {
  H_R13_R13
} H_R13;

/* Enum declaration for General Register 14 explicitely required.  */
typedef enum h_r14 {
  H_R14_R14
} H_R14;

/* Enum declaration for General Register 15 explicitely required.  */
typedef enum h_r15 {
  H_R15_R15
} H_R15;

/* Enum declaration for fr30 operand types.  */
typedef enum cgen_operand_type {
  FR30_OPERAND_PC, FR30_OPERAND_RI, FR30_OPERAND_RJ, FR30_OPERAND_RIC
 , FR30_OPERAND_RJC, FR30_OPERAND_CRI, FR30_OPERAND_CRJ, FR30_OPERAND_RS1
 , FR30_OPERAND_RS2, FR30_OPERAND_R13, FR30_OPERAND_R14, FR30_OPERAND_R15
 , FR30_OPERAND_PS, FR30_OPERAND_U4, FR30_OPERAND_U4C, FR30_OPERAND_U8
 , FR30_OPERAND_I8, FR30_OPERAND_UDISP6, FR30_OPERAND_DISP8, FR30_OPERAND_DISP9
 , FR30_OPERAND_DISP10, FR30_OPERAND_S10, FR30_OPERAND_U10, FR30_OPERAND_I32
 , FR30_OPERAND_M4, FR30_OPERAND_I20, FR30_OPERAND_LABEL9, FR30_OPERAND_DIR8
 , FR30_OPERAND_DIR9, FR30_OPERAND_DIR10, FR30_OPERAND_LABEL12, FR30_OPERAND_REGLIST_LOW
 , FR30_OPERAND_REGLIST_HI, FR30_OPERAND_CC, FR30_OPERAND_CCC, FR30_OPERAND_NBIT
 , FR30_OPERAND_VBIT, FR30_OPERAND_ZBIT, FR30_OPERAND_CBIT, FR30_OPERAND_IBIT
 , FR30_OPERAND_SBIT, FR30_OPERAND_CCR, FR30_OPERAND_MAX
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
#define MAX_OPERAND_INSTANCES 9

/* Hardware, operand and instruction attribute indices.  */

/* Enum declaration for cgen_hw attrs.  */
typedef enum cgen_hw_attr {
  CGEN_HW_CACHE_ADDR, CGEN_HW_FUN_ACCESS, CGEN_HW_PC, CGEN_HW_PROFILE
} CGEN_HW_ATTR;

/* Number of non-boolean elements in cgen_hw.  */
#define CGEN_HW_NBOOL_ATTRS ((int) CGEN_HW_CACHE_ADDR)

/* Hardware, operand and instruction attribute indices.  */

/* Enum declaration for cgen_ifld attrs.  */
typedef enum cgen_ifld_attr {
  CGEN_IFLD_ABS_ADDR, CGEN_IFLD_PCREL_ADDR, CGEN_IFLD_RESERVED, CGEN_IFLD_SIGN_OPT
 , CGEN_IFLD_SIGNED, CGEN_IFLD_UNSIGNED, CGEN_IFLD_VIRTUAL
} CGEN_IFLD_ATTR;

/* Number of non-boolean elements in cgen_ifld.  */
#define CGEN_IFLD_NBOOL_ATTRS ((int) CGEN_IFLD_ABS_ADDR)

/* Enum declaration for fr30 ifield types.  */
typedef enum ifield_type {
  FR30_F_NIL, FR30_F_OP1, FR30_F_OP2, FR30_F_OP3
 , FR30_F_OP4, FR30_F_OP5, FR30_F_CC, FR30_F_CCC
 , FR30_F_RJ, FR30_F_RI, FR30_F_RS1, FR30_F_RS2
 , FR30_F_RJC, FR30_F_RIC, FR30_F_CRJ, FR30_F_CRI
 , FR30_F_U4, FR30_F_U4C, FR30_F_I4, FR30_F_M4
 , FR30_F_U8, FR30_F_I8, FR30_F_I20_4, FR30_F_I20_16
 , FR30_F_I20, FR30_F_I32, FR30_F_UDISP6, FR30_F_DISP8
 , FR30_F_DISP9, FR30_F_DISP10, FR30_F_S10, FR30_F_U10
 , FR30_F_REL9, FR30_F_DIR8, FR30_F_DIR9, FR30_F_DIR10
 , FR30_F_REL12, FR30_F_REGLIST_HI, FR30_F_REGLIST_LOW, FR30_F_MAX
} IFIELD_TYPE;

#define MAX_IFLD ((int) FR30_F_MAX)

/* Enum declaration for cgen_operand attrs.  */
typedef enum cgen_operand_attr {
  CGEN_OPERAND_ABS_ADDR, CGEN_OPERAND_HASH_PREFIX, CGEN_OPERAND_NEGATIVE, CGEN_OPERAND_PCREL_ADDR
 , CGEN_OPERAND_RELAX, CGEN_OPERAND_SEM_ONLY, CGEN_OPERAND_SIGN_OPT, CGEN_OPERAND_SIGNED
 , CGEN_OPERAND_UNSIGNED, CGEN_OPERAND_VIRTUAL
} CGEN_OPERAND_ATTR;

/* Number of non-boolean elements in cgen_operand.  */
#define CGEN_OPERAND_NBOOL_ATTRS ((int) CGEN_OPERAND_ABS_ADDR)

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_ALIAS, CGEN_INSN_COND_CTI, CGEN_INSN_DELAY_SLOT, CGEN_INSN_NO_DIS
 , CGEN_INSN_NOT_IN_DELAY_SLOT, CGEN_INSN_RELAX, CGEN_INSN_RELAXABLE, CGEN_INSN_SKIP_CTI
 , CGEN_INSN_UNCOND_CTI, CGEN_INSN_VIRTUAL
} CGEN_INSN_ATTR;

/* Number of non-boolean elements in cgen_insn.  */
#define CGEN_INSN_NBOOL_ATTRS ((int) CGEN_INSN_ALIAS)

/* Enum declaration for fr30 instruction types.  */
typedef enum cgen_insn_type {
  FR30_INSN_INVALID, FR30_INSN_ADD, FR30_INSN_ADDI, FR30_INSN_ADD2
 , FR30_INSN_ADDC, FR30_INSN_ADDN, FR30_INSN_ADDNI, FR30_INSN_ADDN2
 , FR30_INSN_SUB, FR30_INSN_SUBC, FR30_INSN_SUBN, FR30_INSN_CMP
 , FR30_INSN_CMPI, FR30_INSN_CMP2, FR30_INSN_AND, FR30_INSN_OR
 , FR30_INSN_EOR, FR30_INSN_ANDM, FR30_INSN_ANDH, FR30_INSN_ANDB
 , FR30_INSN_ORM, FR30_INSN_ORH, FR30_INSN_ORB, FR30_INSN_EORM
 , FR30_INSN_EORH, FR30_INSN_EORB, FR30_INSN_BANDL, FR30_INSN_BORL
 , FR30_INSN_BEORL, FR30_INSN_BANDH, FR30_INSN_BORH, FR30_INSN_BEORH
 , FR30_INSN_BTSTL, FR30_INSN_BTSTH, FR30_INSN_MUL, FR30_INSN_MULU
 , FR30_INSN_MULH, FR30_INSN_MULUH, FR30_INSN_DIV0S, FR30_INSN_DIV0U
 , FR30_INSN_DIV1, FR30_INSN_DIV2, FR30_INSN_DIV3, FR30_INSN_DIV4S
 , FR30_INSN_LSL, FR30_INSN_LSLI, FR30_INSN_LSL2, FR30_INSN_LSR
 , FR30_INSN_LSRI, FR30_INSN_LSR2, FR30_INSN_ASR, FR30_INSN_ASRI
 , FR30_INSN_ASR2, FR30_INSN_LDI8, FR30_INSN_LDI20, FR30_INSN_LDI32
 , FR30_INSN_LD, FR30_INSN_LDUH, FR30_INSN_LDUB, FR30_INSN_LDR13
 , FR30_INSN_LDR13UH, FR30_INSN_LDR13UB, FR30_INSN_LDR14, FR30_INSN_LDR14UH
 , FR30_INSN_LDR14UB, FR30_INSN_LDR15, FR30_INSN_LDR15GR, FR30_INSN_LDR15DR
 , FR30_INSN_LDR15PS, FR30_INSN_ST, FR30_INSN_STH, FR30_INSN_STB
 , FR30_INSN_STR13, FR30_INSN_STR13H, FR30_INSN_STR13B, FR30_INSN_STR14
 , FR30_INSN_STR14H, FR30_INSN_STR14B, FR30_INSN_STR15, FR30_INSN_STR15GR
 , FR30_INSN_STR15DR, FR30_INSN_STR15PS, FR30_INSN_MOV, FR30_INSN_MOVDR
 , FR30_INSN_MOVPS, FR30_INSN_MOV2DR, FR30_INSN_MOV2PS, FR30_INSN_JMP
 , FR30_INSN_JMPD, FR30_INSN_CALLR, FR30_INSN_CALLRD, FR30_INSN_CALL
 , FR30_INSN_CALLD, FR30_INSN_RET, FR30_INSN_RETD, FR30_INSN_INT
 , FR30_INSN_INTE, FR30_INSN_RETI, FR30_INSN_BRA, FR30_INSN_BRAD
 , FR30_INSN_BNO, FR30_INSN_BNOD, FR30_INSN_BEQ, FR30_INSN_BEQD
 , FR30_INSN_BNE, FR30_INSN_BNED, FR30_INSN_BC, FR30_INSN_BCD
 , FR30_INSN_BNC, FR30_INSN_BNCD, FR30_INSN_BN, FR30_INSN_BND
 , FR30_INSN_BP, FR30_INSN_BPD, FR30_INSN_BV, FR30_INSN_BVD
 , FR30_INSN_BNV, FR30_INSN_BNVD, FR30_INSN_BLT, FR30_INSN_BLTD
 , FR30_INSN_BGE, FR30_INSN_BGED, FR30_INSN_BLE, FR30_INSN_BLED
 , FR30_INSN_BGT, FR30_INSN_BGTD, FR30_INSN_BLS, FR30_INSN_BLSD
 , FR30_INSN_BHI, FR30_INSN_BHID, FR30_INSN_DMOVR13, FR30_INSN_DMOVR13H
 , FR30_INSN_DMOVR13B, FR30_INSN_DMOVR13PI, FR30_INSN_DMOVR13PIH, FR30_INSN_DMOVR13PIB
 , FR30_INSN_DMOVR15PI, FR30_INSN_DMOV2R13, FR30_INSN_DMOV2R13H, FR30_INSN_DMOV2R13B
 , FR30_INSN_DMOV2R13PI, FR30_INSN_DMOV2R13PIH, FR30_INSN_DMOV2R13PIB, FR30_INSN_DMOV2R15PD
 , FR30_INSN_LDRES, FR30_INSN_STRES, FR30_INSN_COPOP, FR30_INSN_COPLD
 , FR30_INSN_COPST, FR30_INSN_COPSV, FR30_INSN_NOP, FR30_INSN_ANDCCR
 , FR30_INSN_ORCCR, FR30_INSN_STILM, FR30_INSN_ADDSP, FR30_INSN_EXTSB
 , FR30_INSN_EXTUB, FR30_INSN_EXTSH, FR30_INSN_EXTUH, FR30_INSN_LDM0
 , FR30_INSN_LDM1, FR30_INSN_STM0, FR30_INSN_STM1, FR30_INSN_ENTER
 , FR30_INSN_LEAVE, FR30_INSN_XCHB, FR30_INSN_MAX
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
  long f_op4;
  long f_op5;
  long f_cc;
  long f_ccc;
  long f_Rj;
  long f_Ri;
  long f_Rs1;
  long f_Rs2;
  long f_Rjc;
  long f_Ric;
  long f_CRj;
  long f_CRi;
  long f_u4;
  long f_u4c;
  long f_i4;
  long f_m4;
  long f_u8;
  long f_i8;
  long f_i20_4;
  long f_i20_16;
  long f_i20;
  long f_i32;
  long f_udisp6;
  long f_disp8;
  long f_disp9;
  long f_disp10;
  long f_s10;
  long f_u10;
  long f_rel9;
  long f_dir8;
  long f_dir9;
  long f_dir10;
  long f_rel12;
  long f_reglist_hi;
  long f_reglist_low;
  int length;
};

/* Attributes.  */
extern const CGEN_ATTR_TABLE fr30_cgen_hw_attr_table[];
extern const CGEN_ATTR_TABLE fr30_cgen_operand_attr_table[];
extern const CGEN_ATTR_TABLE fr30_cgen_insn_attr_table[];

/* Enum declaration for fr30 hardware types.  */
typedef enum hw_type {
  HW_H_PC, HW_H_MEMORY, HW_H_SINT, HW_H_UINT
 , HW_H_ADDR, HW_H_IADDR, HW_H_GR, HW_H_CR
 , HW_H_DR, HW_H_PS, HW_H_R13, HW_H_R14
 , HW_H_R15, HW_H_NBIT, HW_H_ZBIT, HW_H_VBIT
 , HW_H_CBIT, HW_H_IBIT, HW_H_SBIT, HW_H_CCR
 , HW_MAX
} HW_TYPE;

#define MAX_HW ((int) HW_MAX)

/* Hardware decls.  */

extern CGEN_KEYWORD fr30_cgen_opval_h_gr;
extern CGEN_KEYWORD fr30_cgen_opval_h_cr;
extern CGEN_KEYWORD fr30_cgen_opval_h_dr;
extern CGEN_KEYWORD fr30_cgen_opval_h_ps;
extern CGEN_KEYWORD fr30_cgen_opval_h_r13;
extern CGEN_KEYWORD fr30_cgen_opval_h_r14;
extern CGEN_KEYWORD fr30_cgen_opval_h_r15;

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

/* ??? This can be improved upon.  */
#undef CGEN_DIS_HASH_SIZE
#define CGEN_DIS_HASH_SIZE 16
#undef CGEN_DIS_HASH
#define CGEN_DIS_HASH(buffer, value) (((unsigned char *) (buffer))[0] >> 4)

/* -- */


#endif /* FR30_OPC_H */
