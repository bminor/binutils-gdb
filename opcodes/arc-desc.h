/* CPU data header for arc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2005 Free Software Foundation, Inc.

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
51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#ifndef ARC_DESC_H
#define ARC_DESC_H

#include "opcode/cgen-bitset.h"

#define CGEN_ARCH arc

/* Given symbol S, return arc_cgen_<S>.  */
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define CGEN_SYM(s) arc##_cgen_##s
#else
#define CGEN_SYM(s) arc/**/_cgen_/**/s
#endif


/* Selected cpu families.  */
#define HAVE_CPU_A5F
#define HAVE_CPU_ARC600F
#define HAVE_CPU_ARC700F

#define CGEN_INSN_LSB0_P 0

/* Minimum size of any insn (in bytes).  */
#define CGEN_MIN_INSN_SIZE 4

/* Maximum size of any insn (in bytes).  */
#define CGEN_MAX_INSN_SIZE 4

#define CGEN_INT_INSN_P 1

/* Maximum number of syntax elements in an instruction.  */
#define CGEN_ACTUAL_MAX_SYNTAX_ELEMENTS 19

/* CGEN_MNEMONIC_OPERANDS is defined if mnemonics have operands.
   e.g. In "b,a foo" the ",a" is an operand.  If mnemonics have operands
   we can't hash on everything up to the space.  */
#define CGEN_MNEMONIC_OPERANDS

/* Maximum number of fields in an instruction.  */
#define CGEN_ACTUAL_MAX_IFMT_OPERANDS 8

/* Enums.  */

/* Enum declaration for enum values for Qcond to be used in case form.  */
typedef enum e_qvalues {
  COND_AL = 0, COND_EQ = 1, COND_Z = 1, COND_NE = 2
 , COND_NZ = 2, COND_PL = 3, COND_P = 3, COND_MI = 4
 , COND_N = 4, COND_CS = 5, COND_C = 5, COND_LO = 5
 , COND_CC = 6, COND_NC = 6, COND_HS = 6, COND_VS = 7
 , COND_V = 7, COND_VC = 8, COND_NV = 8, COND_GT = 9
 , COND_GE = 10, COND_LT = 11, COND_LE = 12, COND_HI = 13
 , COND_LS = 14, COND_PNZ = 15
} E_QVALUES;

/* Enum declaration for enum values for i3cond to be used in case form.  */
typedef enum e_i3cond {
  COND3_GT = 0, COND3_GE = 1, COND3_LT = 2, COND3_LE = 3
 , COND3_HI = 4, COND3_CC = 5, COND3_NC = 5, COND3_HS = 5
 , COND3_CS = 6, COND3_C = 6, COND3_LO = 6, COND3_LS = 7
} E_I3COND;

/* Enum declaration for enum values for brcond to be used in case form.  */
typedef enum e_brcond {
  CONDBR_REQ = 0, CONDBR_RNE = 1, CONDBR_RLT = 2, CONDBR_RGE = 3
 , CONDBR_RLO = 4, CONDBR_RHS = 5, CONDBR_BIT0 = 14, CONDBR_BIT1 = 15
} E_BRCOND;

/* Enum declaration for .  */
typedef enum cr_names {
  H_CR_GP = 26, H_CR_FP = 27, H_CR_SP = 28, H_CR_BLINK = 31
 , H_CR_MLO = 57, H_CR_MMID = 58, H_CR_MHI = 59, H_CR_LP_COUNT = 60
 , H_CR_PCL = 63, H_CR_ILINK1 = 29, H_CR_ILINK2 = 30, H_CR_R29 = 29
 , H_CR_R30 = 30, H_CR_R0 = 0, H_CR_R1 = 1, H_CR_R2 = 2
 , H_CR_R3 = 3, H_CR_R4 = 4, H_CR_R5 = 5, H_CR_R6 = 6
 , H_CR_R7 = 7, H_CR_R8 = 8, H_CR_R9 = 9, H_CR_R10 = 10
 , H_CR_R11 = 11, H_CR_R12 = 12, H_CR_R13 = 13, H_CR_R14 = 14
 , H_CR_R15 = 15, H_CR_R16 = 16, H_CR_R17 = 17, H_CR_R18 = 18
 , H_CR_R19 = 19, H_CR_R20 = 20, H_CR_R21 = 21, H_CR_R22 = 22
 , H_CR_R23 = 23, H_CR_R24 = 24, H_CR_R25 = 25, H_CR_R26 = 26
 , H_CR_R27 = 27, H_CR_R28 = 28, H_CR_R31 = 31, H_CR_R32 = 32
 , H_CR_R33 = 33, H_CR_R34 = 34, H_CR_R35 = 35, H_CR_R36 = 36
 , H_CR_R37 = 37, H_CR_R38 = 38, H_CR_R39 = 39, H_CR_R40 = 40
 , H_CR_R41 = 41, H_CR_R42 = 42, H_CR_R43 = 43, H_CR_R44 = 44
 , H_CR_R45 = 45, H_CR_R46 = 46, H_CR_R47 = 47, H_CR_R48 = 48
 , H_CR_R49 = 49, H_CR_R50 = 50, H_CR_R51 = 51, H_CR_R52 = 52
 , H_CR_R53 = 53, H_CR_R54 = 54, H_CR_R55 = 55, H_CR_R56 = 56
 , H_CR_R57 = 57, H_CR_R58 = 58, H_CR_R59 = 59, H_CR_R60 = 60
} CR_NAMES;

/* Enum declaration for Core Register A encodings.  */
typedef enum e_ra_rn {
  RA_R0, RA_R1, RA_R2, RA_R3
 , RA_R4, RA_R5, RA_R6, RA_R7
 , RA_R8, RA_R9, RA_R10, RA_R11
 , RA_R12, RA_R13, RA_R14, RA_R15
 , RA_R16, RA_R17, RA_R18, RA_R19
 , RA_R20, RA_R21, RA_R22, RA_R23
 , RA_R24, RA_R25, RA_R26, RA_R27
 , RA_R28, RA_R29, RA_R30, RA_R31
 , RA_R32, RA_R33, RA_R34, RA_R35
 , RA_R36, RA_R37, RA_R38, RA_R39
 , RA_R40, RA_R41, RA_R42, RA_R43
 , RA_R44, RA_R45, RA_R46, RA_R47
 , RA_R48, RA_R49, RA_R50, RA_R51
 , RA_R52, RA_R53, RA_R54, RA_R55
 , RA_R56, RA_R57, RA_R58, RA_R59
 , RA_R60, RA_R61, RA_R62, RA_R63
} E_RA_RN;

/* Enum declaration for major opcode.  */
typedef enum op_maj {
  OPM_B = 0, OPM_BLR = 1, OPM_LD_S9 = 2, OPM_ST_S9 = 3
 , OPM_GO = 4, OPM_X05 = 5, OPM_X06 = 6, OPM_X07 = 7
 , OPM_SLDADDR = 12, OPM_SADDSUBSHI = 13, OPM_SMOVCMPADDH = 14, OPM_SGO = 15
 , OPM_LDO_S = 16, OPM_LDOB_S = 17, OPM_LDOW_S = 18, OPM_LDOWX_S = 19
 , OPM_STO_S = 20, OPM_STOB_S = 21, OPM_STOW_S = 22, OPM_SSHSUBBIMM = 23
 , OPM_SP = 24, OPM_GP = 25, OPM_LDPCREL = 26, OPM_SMOVU8 = 27
 , OPM_SADDCMPU7 = 28, OPM_BR_S = 29, OPM_B_S = 30, OPM_BL_S = 31
 , OPM_PSEUDO = 32
} OP_MAJ;

/* Enum declaration for general operations type.  */
typedef enum go_type {
  GO_TYPE_R_R, GO_TYPE_U6, GO_TYPE_S12, GO_TYPE_CC
} GO_TYPE;

/* Enum declaration for general operations conditional subtype.  */
typedef enum go_cc_type {
  GO_CC_REG, GO_CC_U6
} GO_CC_TYPE;

/* Enum declaration for general operations type.  */
typedef enum go_op {
  GO_OP_ADD = 0, GO_OP_ADC = 1, GO_OP_SUB = 2, GO_OP_SBC = 3
 , GO_OP_AND = 4, GO_OP_OR = 5, GO_OP_BIC = 6, GO_OP_XOR = 7
 , GO_OP_MAX = 8, GO_OP_MIN = 9, GO_OP_MOV = 10, GO_OP_TST = 11
 , GO_OP_CMP = 12, GO_OP_RCMP = 13, GO_OP_RSUB = 14, GO_OP_BSET = 15
 , GO_OP_BCLR = 16, GO_OP_BTST = 17, GO_OP_BXOR = 18, GO_OP_BMSK = 19
 , GO_OP_ADD1 = 20, GO_OP_ADD2 = 21, GO_OP_ADD3 = 22, GO_OP_SUB1 = 23
 , GO_OP_SUB2 = 24, GO_OP_SUB3 = 25, GO_OP_MPY = 26, GO_OP_MPYH = 27
 , GO_OP_MPYHU = 28, GO_OP_MPYU = 29, GO_OP_RES30 = 30, GO_OP_RES31 = 31
 , GO_OP_J = 32, GO_OP_J_D = 33, GO_OP_JL = 34, GO_OP_JL_D = 35
 , GO_OP_LP = 40, GO_OP_FLAG = 41, GO_OP_LR = 42, GO_OP_SR = 43
 , GO_OP_SOP = 47
} GO_OP;

/* Enum declaration for general single-operand operations type.  */
typedef enum go_sop {
  GO_OP_SOP_ASL = 0, GO_OP_SOP_ASR = 1, GO_OP_SOP_LSR = 2, GO_OP_SOP_ROR = 3
 , GO_OP_SOP_RRC = 4, GO_OP_SOP_SEXB = 5, GO_OP_SOP_SEXW = 6, GO_OP_SOP_EXTB = 7
 , GO_OP_SOP_EXTW = 8, GO_OP_SOP_ABS = 9, GO_OP_SOP_NOT = 10, GO_OP_SOP_RLC = 11
 , GO_OP_SOP_EX = 12, GO_OP_SOP_ZOP = 63, GO_OP_SOP_PSEUDO = 62
} GO_SOP;

/* Enum declaration for short add / sub immediate type.  */
typedef enum i16ldaddr_type {
  I16_LDADDR_LD, I16_LDADDR_LDB, I16_LDADDR_LDW, I16_LDADDR_ADD
} I16LDADDR_TYPE;

/* Enum declaration for short add / sub immediate type.  */
typedef enum i16addsubshi_type {
  I16_ADDSUBSHI_ADD, I16_ADDSUBSHI_SUB, I16_ADDSUBSHI_ASL, I16_ADDSUBSHI_ASR
} I16ADDSUBSHI_TYPE;

/* Enum declaration for short mov / cmp / add  with high register type.  */
typedef enum i16movcmpaddh_type {
  I16_MOVCMPADDH_ADD, I16_MOVCMPADDH_MOVBH, I16_MOVCMPADDH_CMP, I16_MOVCMPADDH_MOVHB
} I16MOVCMPADDH_TYPE;

/* Enum declaration for short general operations.  */
typedef enum i16go_type {
  I16_GO_SOP = 0, I16_GO_SUB = 2, I16_GO_AND = 4, I16_GO_OR = 5
 , I16_GO_BIC = 6, I16_GO_XOR = 7, I16_GO_TST = 11, I16_GO_MUL64 = 12
 , I16_GO_SEXB = 13, I16_GO_SEXW = 14, I16_GO_EXTB = 15, I16_GO_EXTW = 16
 , I16_GO_ABS = 17, I16_GO_NOT = 18, I16_GO_NEG = 19, I16_GO_ADD1 = 20
 , I16_GO_ADD2 = 21, I16_GO_ADD3 = 22, I16_GO_ASLM = 24, I16_GO_LSRM = 25
 , I16_GO_ASRM = 26, I16_GO_ASL = 27, I16_GO_ASR = 28, I16_GO_LSR = 29
 , I16_GO_TRAP = 30, I16_GO_BRK = 31
} I16GO_TYPE;

/* Enum declaration for short general operations single operand.  */
typedef enum i16go_sop_type {
  I16_GO_SOP_J = 0, I16_GO_SOP_J_D = 1, I16_GO_SOP_JL = 2, I16_GO_SOP_JL_D = 3
 , I16_GO_SOP_SUB_NE = 6, I16_GO_SOP_ZOP = 7
} I16GO_SOP_TYPE;

/* Enum declaration for short general operations single operand.  */
typedef enum i16go_zop_type {
  I16_GO_ZOP_NOP = 0, I16_GO_ZOP_UNIMP = 1, I16_GO_ZOP_JEQ = 4, I16_GO_ZOP_JNE = 5
 , I16_GO_ZOP_J = 6, I16_GO_ZOP_J_D = 7
} I16GO_ZOP_TYPE;

/* Enum declaration for sp based insn type.  */
typedef enum i16sp_type {
  I16_SP_LD, I16_SP_LDB, I16_SP_ST, I16_SP_STB
 , I16_SP_ADD, I16_SP_ADDSUB, I16_SP_POP, I16_SP_PUSH
} I16SP_TYPE;

/* Enum declaration for sp based 1op insn type.  */
typedef enum i16addsub_spsp_type {
  I16_SP_ADDSUB_ADD, I16_SP_ADDSUB_SUB
} I16ADDSUB_SPSP_TYPE;

/* Enum declaration for gp-relative insn type.  */
typedef enum i16gp_type {
  I16_GP_LD, I16_GP_LDB, I16_GP_LDW, I16_GP_ADD
} I16GP_TYPE;

/* Enum declaration for short add / cmp immediate type.  */
typedef enum i16addcmpu7_type {
  I16_ADDCMPU7_ADD, I16_ADDCMPU7_CMP
} I16ADDCMPU7_TYPE;

/* Enum declaration for shift / sub / bit immediate short insn w/ u5 type.  */
typedef enum i16shsubbimm {
  I16_SHSUBBIMM_ASL, I16_SHSUBBIMM_LSR, I16_SHSUBBIMM_ASR, I16_SHSUBBIMM_SUB
 , I16_SHSUBBIMM_BSET, I16_SHSUBBIMM_BCLR, I16_SHSUBBIMM_BMSK, I16_SHSUBBIMM_BTST
} I16SHSUBBIMM;

/* Enum declaration for .  */
typedef enum i_buf {
  B_CC, B_UNCOND_FAR
} I_BUF;

/* Enum declaration for .  */
typedef enum i_blr {
  BLR_BL, BLR_BR
} I_BLR;

/* Enum declaration for .  */
typedef enum i_br {
  BR_RC, BR_U6
} I_BR;

/* Enum declaration for .  */
typedef enum op_bl {
  BL_CC, BL_UNCOND_FAR
} OP_BL;

/* Enum declaration for .  */
typedef enum i_bcc_s {
  B_S_CC = 3
} I_BCC_S;

/* Enum declaration for .  */
typedef enum i_ldozz {
  LDO_LD = 0, LDO_LDB = 2, LDO_LDBX = 3, LDO_LDW = 4
 , LDO_LDWX = 5
} I_LDOZZ;

/* Enum declaration for .  */
typedef enum i_ldr6zzx {
  LDR_LD = 48, LDR_LDB = 50, LDR_LDBX = 51, LDR_LDW = 52
 , LDR_LDWX = 53
} I_LDR6ZZX;

/* Enum declaration for .  */
typedef enum i_stozzr {
  STO_ST = 0, STO_STB = 2, STO_STW = 4
} I_STOZZR;

/* Enum declaration for .  */
typedef enum i_ldoaa {
  LDOAA_NO, LDOAA_AW, LDOAA_AB, LDOAA_AS
} I_LDOAA;

/* Enum declaration for .  */
typedef enum i_ldraa {
  LDRAA_NO, LDRAA_AW, LDRAA_AB, LDRAA_AS
} I_LDRAA;

/* Enum declaration for .  */
typedef enum i_stoaa {
  STOAA_NO, STOAA_AW, STOAA_AB, STOAA_AS
} I_STOAA;

/* Enum declaration for general zero-operand operations type.  */
typedef enum go_zop {
  GO_OP_ZOP_SLEEP = 1, GO_OP_ZOP_SWI = 2, GO_OP_ZOP_SYNC = 3, GO_OP_ZOP_RTIE = 4
 , GO_OP_ZOP_BRK = 5
} GO_ZOP;

/* Enum declaration for general operations type.  */
typedef enum x05_go_op {
  X05_ASL = 0, X05_LSR = 1, X05_ASR = 2, X05_ROR = 3
 , X05_MUL64 = 4, X05_MULU64 = 5, X05_ADDS = 6, X05_SUBS = 7
 , X05_DIVAW = 8, X05_ASLS = 10, X05_ASRS = 11, X05_ADDSDW = 40
 , X05_SUBSDW = 41, X05_SOP = 47, X05_CMACRDW = 38, X05_MACDW = 16
 , X05_MACFLW = 52, X05_MACHFLW = 55, X05_MACHLW = 54, X05_MACHULW = 53
 , X05_MACLW = 51, X05_MACRDW = 18, X05_MACUDW = 17, X05_MSUBDW = 20
 , X05_MULDW = 12, X05_MULFLW = 50, X05_MULHFLW = 57, X05_MULHLW = 56
 , X05_MULLW = 49, X05_MULRDW = 14, X05_MULUDW = 13, X05_MULULW = 48
} X05_GO_OP;

/* Enum declaration for x06 extension single-operand operantion.  */
typedef enum x05_sop_kind {
  X05_SOP_SWAP = 0, X05_SOP_NORM = 1, X05_SOP_SAT16 = 2, X05_SOP_RND16 = 3
 , X05_SOP_ABSSW = 4, X05_SOP_ABSS = 5, X05_SOP_NEGSW = 6, X05_SOP_NEGS = 7
 , X05_SOP_NORMW = 8, X05_SOP_ZOP = 63
} X05_SOP_KIND;

/* Enum declaration for .  */
typedef enum pushpop_kind {
  PUSHPOP_B = 1, PUSHPOP_BLINK = 17
} PUSHPOP_KIND;

/* Enum declaration for .  */
typedef enum pushpop_r_b {
  OP_B_0
} PUSHPOP_R_B;

/* Attributes.  */

/* Enum declaration for machine type selection.  */
typedef enum mach_attr {
  MACH_BASE, MACH_A5, MACH_ARC600, MACH_ARC700
 , MACH_MAX
} MACH_ATTR;

/* Enum declaration for instruction set selection.  */
typedef enum isa_attr {
  ISA_ARCOMPACT, ISA_MAX
} ISA_ATTR;

/* Enum declaration for can take long immediate for operand.  */
typedef enum limm_attr {
  LIMM_NONE, LIMM_H, LIMM_B, LIMM_BC
 , LIMM_C
} LIMM_ATTR;

/* Number of architecture variants.  */
#define MAX_ISAS  1
#define MAX_MACHS ((int) MACH_MAX)

/* Ifield support.  */

/* Ifield attribute indices.  */

/* Enum declaration for cgen_ifld attrs.  */
typedef enum cgen_ifld_attr {
  CGEN_IFLD_VIRTUAL, CGEN_IFLD_PCREL_ADDR, CGEN_IFLD_ABS_ADDR, CGEN_IFLD_RESERVED
 , CGEN_IFLD_SIGN_OPT, CGEN_IFLD_SIGNED, CGEN_IFLD_END_BOOLS, CGEN_IFLD_START_NBOOLS = 31
 , CGEN_IFLD_MACH, CGEN_IFLD_END_NBOOLS
} CGEN_IFLD_ATTR;

/* Number of non-boolean elements in cgen_ifld_attr.  */
#define CGEN_IFLD_NBOOL_ATTRS (CGEN_IFLD_END_NBOOLS - CGEN_IFLD_START_NBOOLS - 1)

/* cgen_ifld attribute accessor macros.  */
#define CGEN_ATTR_CGEN_IFLD_MACH_VALUE(attrs) ((attrs)->nonbool[CGEN_IFLD_MACH-CGEN_IFLD_START_NBOOLS-1].nonbitset)
#define CGEN_ATTR_CGEN_IFLD_VIRTUAL_VALUE(attrs) (((attrs)->bool & (1 << CGEN_IFLD_VIRTUAL)) != 0)
#define CGEN_ATTR_CGEN_IFLD_PCREL_ADDR_VALUE(attrs) (((attrs)->bool & (1 << CGEN_IFLD_PCREL_ADDR)) != 0)
#define CGEN_ATTR_CGEN_IFLD_ABS_ADDR_VALUE(attrs) (((attrs)->bool & (1 << CGEN_IFLD_ABS_ADDR)) != 0)
#define CGEN_ATTR_CGEN_IFLD_RESERVED_VALUE(attrs) (((attrs)->bool & (1 << CGEN_IFLD_RESERVED)) != 0)
#define CGEN_ATTR_CGEN_IFLD_SIGN_OPT_VALUE(attrs) (((attrs)->bool & (1 << CGEN_IFLD_SIGN_OPT)) != 0)
#define CGEN_ATTR_CGEN_IFLD_SIGNED_VALUE(attrs) (((attrs)->bool & (1 << CGEN_IFLD_SIGNED)) != 0)

/* Enum declaration for arc ifield types.  */
typedef enum ifield_type {
  ARC_F_NIL, ARC_F_ANYOF, ARC_F_COND_Q, ARC_F_COND_I2
 , ARC_F_COND_I3, ARC_F_BRCOND, ARC_F_OP__A, ARC_F_OP__B
 , ARC_F_OP__C, ARC_F_B_5_3, ARC_F_OP_B, ARC_F_OP_C
 , ARC_F_OP_CJ, ARC_F_H_2_0, ARC_F_H_5_3, ARC_F_OP_H
 , ARC_F_U6, ARC_F_U6X2, ARC_F_DELAY_N, ARC_F_RES27
 , ARC_F_F, ARC_F_CBRANCH_IMM, ARC_F_OP_A, ARC_F_S12H
 , ARC_F_S12, ARC_F_S12X2, ARC_F_REL10, ARC_F_REL7
 , ARC_F_REL8, ARC_F_REL13BL, ARC_F_D21L, ARC_F_D21BL
 , ARC_F_D21H, ARC_F_D25M, ARC_F_D25H, ARC_F_REL21
 , ARC_F_REL21BL, ARC_F_REL25, ARC_F_REL25BL, ARC_F_D9L
 , ARC_F_D9H, ARC_F_REL9, ARC_F_U3, ARC_F_U5
 , ARC_F_U7, ARC_F_U8, ARC_F_S9, ARC_F_U5X2
 , ARC_F_U5X4, ARC_F_U8X4, ARC_F_S9X1, ARC_F_S9X2
 , ARC_F_S9X4, ARC_F_DUMMY, ARC_F_OPM, ARC_F_GO_TYPE
 , ARC_F_GO_CC_TYPE, ARC_F_GO_OP, ARC_F_I16_43, ARC_F_I16_GO
 , ARC_F_I16_GP_TYPE, ARC_F_I16ADDCMPU7_TYPE, ARC_F_BUF, ARC_F_BR
 , ARC_F_BLUF, ARC_F_BRSCOND, ARC_F_LDOZZX, ARC_F_LDR6ZZX
 , ARC_F_STOZZR, ARC_F_LDOAA, ARC_F_LDRAA, ARC_F_STOAA
 , ARC_F_LDODI, ARC_F_LDRDI, ARC_F_STODI, ARC_F_TRAPNUM
 , ARC_F_MAX
} IFIELD_TYPE;

#define MAX_IFLD ((int) ARC_F_MAX)

/* Hardware attribute indices.  */

/* Enum declaration for cgen_hw attrs.  */
typedef enum cgen_hw_attr {
  CGEN_HW_VIRTUAL, CGEN_HW_CACHE_ADDR, CGEN_HW_PC, CGEN_HW_PROFILE
 , CGEN_HW_END_BOOLS, CGEN_HW_START_NBOOLS = 31, CGEN_HW_MACH, CGEN_HW_END_NBOOLS
} CGEN_HW_ATTR;

/* Number of non-boolean elements in cgen_hw_attr.  */
#define CGEN_HW_NBOOL_ATTRS (CGEN_HW_END_NBOOLS - CGEN_HW_START_NBOOLS - 1)

/* cgen_hw attribute accessor macros.  */
#define CGEN_ATTR_CGEN_HW_MACH_VALUE(attrs) ((attrs)->nonbool[CGEN_HW_MACH-CGEN_HW_START_NBOOLS-1].nonbitset)
#define CGEN_ATTR_CGEN_HW_VIRTUAL_VALUE(attrs) (((attrs)->bool & (1 << CGEN_HW_VIRTUAL)) != 0)
#define CGEN_ATTR_CGEN_HW_CACHE_ADDR_VALUE(attrs) (((attrs)->bool & (1 << CGEN_HW_CACHE_ADDR)) != 0)
#define CGEN_ATTR_CGEN_HW_PC_VALUE(attrs) (((attrs)->bool & (1 << CGEN_HW_PC)) != 0)
#define CGEN_ATTR_CGEN_HW_PROFILE_VALUE(attrs) (((attrs)->bool & (1 << CGEN_HW_PROFILE)) != 0)

/* Enum declaration for arc hardware types.  */
typedef enum cgen_hw_type {
  HW_H_MEMORY, HW_H_SINT, HW_H_UINT, HW_H_ADDR
 , HW_H_IADDR, HW_H_LBIT, HW_H_ZBIT, HW_H_NBIT
 , HW_H_CBIT, HW_H_VBIT, HW_H_UBIT, HW_H_E1
 , HW_H_E2, HW_H_S1BIT, HW_H_S2BIT, HW_H_QCONDB
 , HW_H_QCONDJ, HW_H_QCONDI, HW_H_UNCONDB, HW_H_UNCONDJ
 , HW_H_UNCONDI, HW_H_I2COND, HW_H_I3COND, HW_H_DELAY
 , HW_H_UFLAGS, HW_H_NIL, HW_H_AUFLAGS, HW_H_AUFFLAGS
 , HW_H_DI, HW_H_INSN16, HW_H_INSN32, HW_H__AW
 , HW_H_CR, HW_H_CR16, HW_H_R0, HW_H_GP
 , HW_H_SP, HW_H_PCL, HW_H_NOILINK, HW_H_ILINKX
 , HW_H_R31, HW_H_AUXR, HW_H_STATUS32, HW_H_TIMER_EXPIRE
 , HW_H_PROF_OFFSET, HW_H_NE, HW_H_PC, HW_H_RCCS
 , HW_H_RCC, HW_MAX
} CGEN_HW_TYPE;

#define MAX_HW ((int) HW_MAX)

/* Operand attribute indices.  */

/* Enum declaration for cgen_operand attrs.  */
typedef enum cgen_operand_attr {
  CGEN_OPERAND_VIRTUAL, CGEN_OPERAND_PCREL_ADDR, CGEN_OPERAND_ABS_ADDR, CGEN_OPERAND_SIGN_OPT
 , CGEN_OPERAND_SIGNED, CGEN_OPERAND_NEGATIVE, CGEN_OPERAND_RELAX, CGEN_OPERAND_SEM_ONLY
 , CGEN_OPERAND_END_BOOLS, CGEN_OPERAND_START_NBOOLS = 31, CGEN_OPERAND_MACH, CGEN_OPERAND_END_NBOOLS
} CGEN_OPERAND_ATTR;

/* Number of non-boolean elements in cgen_operand_attr.  */
#define CGEN_OPERAND_NBOOL_ATTRS (CGEN_OPERAND_END_NBOOLS - CGEN_OPERAND_START_NBOOLS - 1)

/* cgen_operand attribute accessor macros.  */
#define CGEN_ATTR_CGEN_OPERAND_MACH_VALUE(attrs) ((attrs)->nonbool[CGEN_OPERAND_MACH-CGEN_OPERAND_START_NBOOLS-1].nonbitset)
#define CGEN_ATTR_CGEN_OPERAND_VIRTUAL_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_VIRTUAL)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_PCREL_ADDR_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_PCREL_ADDR)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_ABS_ADDR_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_ABS_ADDR)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_SIGN_OPT_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_SIGN_OPT)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_SIGNED_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_SIGNED)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_NEGATIVE_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_NEGATIVE)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_RELAX_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_RELAX)) != 0)
#define CGEN_ATTR_CGEN_OPERAND_SEM_ONLY_VALUE(attrs) (((attrs)->bool & (1 << CGEN_OPERAND_SEM_ONLY)) != 0)

/* Enum declaration for arc operand types.  */
typedef enum cgen_operand_type {
  ARC_OPERAND_PC, ARC_OPERAND_LBIT, ARC_OPERAND_ZBIT, ARC_OPERAND_NBIT
 , ARC_OPERAND_CBIT, ARC_OPERAND_VBIT, ARC_OPERAND_S1BIT, ARC_OPERAND_S2BIT
 , ARC_OPERAND_QCONDB, ARC_OPERAND_QCONDJ, ARC_OPERAND_QCONDI, ARC_OPERAND_UNCONDB
 , ARC_OPERAND_UNCONDJ, ARC_OPERAND_UNCONDI, ARC_OPERAND_I2COND, ARC_OPERAND_I3COND
 , ARC_OPERAND_DELAY_N, ARC_OPERAND__S, ARC_OPERAND__L, ARC_OPERAND_F
 , ARC_OPERAND_F1, ARC_OPERAND_F1F, ARC_OPERAND_F0, ARC_OPERAND_R_A
 , ARC_OPERAND_RA, ARC_OPERAND_R_B, ARC_OPERAND_RB, ARC_OPERAND_R_C
 , ARC_OPERAND_RC, ARC_OPERAND_RH, ARC_OPERAND_R0, ARC_OPERAND_R31
 , ARC_OPERAND_GP, ARC_OPERAND_SP, ARC_OPERAND_PCL, ARC_OPERAND_RA_0
 , ARC_OPERAND_RB_0, ARC_OPERAND_RC_ILINK, ARC_OPERAND_RC_NOILINK, ARC_OPERAND_NE
 , ARC_OPERAND_U6, ARC_OPERAND_U6X2, ARC_OPERAND_U3, ARC_OPERAND_U5
 , ARC_OPERAND_U7, ARC_OPERAND_U8, ARC_OPERAND_S9, ARC_OPERAND_S12
 , ARC_OPERAND_S12X2, ARC_OPERAND_U5X4, ARC_OPERAND_SC_U5_, ARC_OPERAND_SC_U5W
 , ARC_OPERAND_SC_U5B, ARC_OPERAND_U8X4, ARC_OPERAND_S9X4, ARC_OPERAND_SC_S9_
 , ARC_OPERAND_SC_S9W, ARC_OPERAND_SC_S9B, ARC_OPERAND_LABEL7, ARC_OPERAND_LABEL8
 , ARC_OPERAND_LABEL9, ARC_OPERAND_LABEL10, ARC_OPERAND_LABEL13A, ARC_OPERAND_LABEL21
 , ARC_OPERAND_LABEL21A, ARC_OPERAND_LABEL25, ARC_OPERAND_LABEL25A, ARC_OPERAND_DUMMY_OP
 , ARC_OPERAND_RCCS, ARC_OPERAND_RCC, ARC_OPERAND_LDODI, ARC_OPERAND_LDRDI
 , ARC_OPERAND_STODI, ARC_OPERAND_EXDI, ARC_OPERAND__AW, ARC_OPERAND_TRAPNUM
 , ARC_OPERAND_MAX
} CGEN_OPERAND_TYPE;

/* Number of operands types.  */
#define MAX_OPERANDS 76

/* Maximum number of operands referenced by any insn.  */
#define MAX_OPERAND_INSTANCES 26

/* Insn attribute indices.  */

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_ALIAS, CGEN_INSN_VIRTUAL, CGEN_INSN_UNCOND_CTI, CGEN_INSN_COND_CTI
 , CGEN_INSN_SKIP_CTI, CGEN_INSN_DELAY_SLOT, CGEN_INSN_RELAXABLE, CGEN_INSN_RELAXED
 , CGEN_INSN_NO_DIS, CGEN_INSN_PBB, CGEN_INSN_SHORT_P, CGEN_INSN_END_BOOLS
 , CGEN_INSN_START_NBOOLS = 31, CGEN_INSN_MACH, CGEN_INSN_LIMM, CGEN_INSN_END_NBOOLS
} CGEN_INSN_ATTR;

/* Number of non-boolean elements in cgen_insn_attr.  */
#define CGEN_INSN_NBOOL_ATTRS (CGEN_INSN_END_NBOOLS - CGEN_INSN_START_NBOOLS - 1)

/* cgen_insn attribute accessor macros.  */
#define CGEN_ATTR_CGEN_INSN_MACH_VALUE(attrs) ((attrs)->nonbool[CGEN_INSN_MACH-CGEN_INSN_START_NBOOLS-1].nonbitset)
#define CGEN_ATTR_CGEN_INSN_LIMM_VALUE(attrs) ((attrs)->nonbool[CGEN_INSN_LIMM-CGEN_INSN_START_NBOOLS-1].nonbitset)
#define CGEN_ATTR_CGEN_INSN_ALIAS_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_ALIAS)) != 0)
#define CGEN_ATTR_CGEN_INSN_VIRTUAL_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_VIRTUAL)) != 0)
#define CGEN_ATTR_CGEN_INSN_UNCOND_CTI_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_UNCOND_CTI)) != 0)
#define CGEN_ATTR_CGEN_INSN_COND_CTI_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_COND_CTI)) != 0)
#define CGEN_ATTR_CGEN_INSN_SKIP_CTI_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_SKIP_CTI)) != 0)
#define CGEN_ATTR_CGEN_INSN_DELAY_SLOT_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_DELAY_SLOT)) != 0)
#define CGEN_ATTR_CGEN_INSN_RELAXABLE_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_RELAXABLE)) != 0)
#define CGEN_ATTR_CGEN_INSN_RELAXED_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_RELAXED)) != 0)
#define CGEN_ATTR_CGEN_INSN_NO_DIS_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_NO_DIS)) != 0)
#define CGEN_ATTR_CGEN_INSN_PBB_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_PBB)) != 0)
#define CGEN_ATTR_CGEN_INSN_SHORT_P_VALUE(attrs) (((attrs)->bool & (1 << CGEN_INSN_SHORT_P)) != 0)

/* cgen.h uses things we just defined.  */
#include "opcode/cgen.h"

extern const struct cgen_ifld arc_cgen_ifld_table[];

/* Attributes.  */
#ifdef UNUSED
extern const CGEN_ATTR_TABLE arc_cgen_hardware_attr_table[];
extern const CGEN_ATTR_TABLE arc_cgen_ifield_attr_table[];
extern const CGEN_ATTR_TABLE arc_cgen_operand_attr_table[];
extern const CGEN_ATTR_TABLE arc_cgen_insn_attr_table[];
#endif

/* Hardware decls.  */

extern CGEN_KEYWORD arc_cgen_opval_h_Qcondb;
extern CGEN_KEYWORD arc_cgen_opval_h_Qcondj;
extern CGEN_KEYWORD arc_cgen_opval_h_Qcondi;
extern CGEN_KEYWORD arc_cgen_opval_h_uncondb;
extern CGEN_KEYWORD arc_cgen_opval_h_uncondj;
extern CGEN_KEYWORD arc_cgen_opval_h_uncondi;
extern CGEN_KEYWORD arc_cgen_opval_h_i2cond;
extern CGEN_KEYWORD arc_cgen_opval_h_i3cond;
extern CGEN_KEYWORD arc_cgen_opval_h_delay;
extern CGEN_KEYWORD arc_cgen_opval_h_uflags;
extern CGEN_KEYWORD arc_cgen_opval_h_nil;
extern CGEN_KEYWORD arc_cgen_opval_h_auflags;
extern CGEN_KEYWORD arc_cgen_opval_h_aufflags;
extern CGEN_KEYWORD arc_cgen_opval_h_Di;
extern CGEN_KEYWORD arc_cgen_opval_h_insn16;
extern CGEN_KEYWORD arc_cgen_opval_h_insn32;
extern CGEN_KEYWORD arc_cgen_opval_h__aw;
extern CGEN_KEYWORD arc_cgen_opval_cr_names;
extern CGEN_KEYWORD arc_cgen_opval_h_cr16;
extern CGEN_KEYWORD arc_cgen_opval_h_r0;
extern CGEN_KEYWORD arc_cgen_opval_h_gp;
extern CGEN_KEYWORD arc_cgen_opval_h_sp;
extern CGEN_KEYWORD arc_cgen_opval_h_pcl;
extern CGEN_KEYWORD arc_cgen_opval_h_noilink;
extern CGEN_KEYWORD arc_cgen_opval_h_ilinkx;
extern CGEN_KEYWORD arc_cgen_opval_h_r31;
extern CGEN_KEYWORD arc_cgen_opval_cr_names;
extern CGEN_KEYWORD arc_cgen_opval_h_status32;
extern CGEN_KEYWORD arc_cgen_opval_h_ne;
extern CGEN_KEYWORD arc_cgen_opval_h_RccS;
extern CGEN_KEYWORD arc_cgen_opval_h_Rcc;

#endif /* ARC_DESC_H */
