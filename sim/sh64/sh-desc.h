/* CPU data header for sh.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

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

#ifndef SH_CPU_H
#define SH_CPU_H

#define CGEN_ARCH sh

/* Given symbol S, return sh_cgen_<S>.  */
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define CGEN_SYM(s) sh##_cgen_##s
#else
#define CGEN_SYM(s) sh/**/_cgen_/**/s
#endif


/* Selected cpu families.  */
#define HAVE_CPU_SH64

#define CGEN_INSN_LSB0_P 1

/* Minimum size of any insn (in bytes).  */
#define CGEN_MIN_INSN_SIZE 2

/* Maximum size of any insn (in bytes).  */
#define CGEN_MAX_INSN_SIZE 4

#define CGEN_INT_INSN_P 1

/* Maximum nymber of syntax bytes in an instruction.  */
#define CGEN_ACTUAL_MAX_SYNTAX_BYTES 22

/* CGEN_MNEMONIC_OPERANDS is defined if mnemonics have operands.
   e.g. In "b,a foo" the ",a" is an operand.  If mnemonics have operands
   we can't hash on everything up to the space.  */
#define CGEN_MNEMONIC_OPERANDS

/* Maximum number of fields in an instruction.  */
#define CGEN_ACTUAL_MAX_IFMT_OPERANDS 8

/* Enums.  */

/* Enum declaration for .  */
typedef enum frc_names {
  H_FRC_FR0, H_FRC_FR1, H_FRC_FR2, H_FRC_FR3
 , H_FRC_FR4, H_FRC_FR5, H_FRC_FR6, H_FRC_FR7
 , H_FRC_FR8, H_FRC_FR9, H_FRC_FR10, H_FRC_FR11
 , H_FRC_FR12, H_FRC_FR13, H_FRC_FR14, H_FRC_FR15
} FRC_NAMES;

/* Enum declaration for .  */
typedef enum drc_names {
  H_DRC_DR0 = 0, H_DRC_DR2 = 2, H_DRC_DR4 = 4, H_DRC_DR6 = 6
 , H_DRC_DR8 = 8, H_DRC_DR10 = 10, H_DRC_DR12 = 12, H_DRC_DR14 = 14
} DRC_NAMES;

/* Enum declaration for .  */
typedef enum xf_names {
  H_XF_XF0, H_XF_XF1, H_XF_XF2, H_XF_XF3
 , H_XF_XF4, H_XF_XF5, H_XF_XF6, H_XF_XF7
 , H_XF_XF8, H_XF_XF9, H_XF_XF10, H_XF_XF11
 , H_XF_XF12, H_XF_XF13, H_XF_XF14, H_XF_XF15
} XF_NAMES;

/* Attributes.  */

/* Enum declaration for machine type selection.  */
typedef enum mach_attr {
  MACH_BASE, MACH_SH2, MACH_SH3, MACH_SH3E
 , MACH_SH4, MACH_SH5, MACH_MAX
} MACH_ATTR;

/* Enum declaration for instruction set selection.  */
typedef enum isa_attr {
  ISA_COMPACT, ISA_MEDIA, ISA_MAX
} ISA_ATTR;

/* Number of architecture variants.  */
#define MAX_ISAS  ((int) ISA_MAX)
#define MAX_MACHS ((int) MACH_MAX)

/* Ifield support.  */

extern const struct cgen_ifld sh_cgen_ifld_table[];

/* Ifield attribute indices.  */

/* Enum declaration for cgen_ifld attrs.  */
typedef enum cgen_ifld_attr {
  CGEN_IFLD_VIRTUAL, CGEN_IFLD_PCREL_ADDR, CGEN_IFLD_ABS_ADDR, CGEN_IFLD_RESERVED
 , CGEN_IFLD_SIGN_OPT, CGEN_IFLD_SIGNED, CGEN_IFLD_END_BOOLS, CGEN_IFLD_START_NBOOLS = 31
 , CGEN_IFLD_MACH, CGEN_IFLD_ISA, CGEN_IFLD_END_NBOOLS
} CGEN_IFLD_ATTR;

/* Number of non-boolean elements in cgen_ifld_attr.  */
#define CGEN_IFLD_NBOOL_ATTRS (CGEN_IFLD_END_NBOOLS - CGEN_IFLD_START_NBOOLS - 1)

/* Enum declaration for sh ifield types.  */
typedef enum ifield_type {
  SH_F_NIL, SH_F_ANYOF, SH_F_OP4, SH_F_OP8
 , SH_F_OP16, SH_F_SUB4, SH_F_SUB8, SH_F_SUB10
 , SH_F_RN, SH_F_RM, SH_F_8_1, SH_F_DISP8
 , SH_F_DISP12, SH_F_IMM8, SH_F_IMM4, SH_F_IMM4X2
 , SH_F_IMM4X4, SH_F_IMM8X2, SH_F_IMM8X4, SH_F_DN
 , SH_F_DM, SH_F_VN, SH_F_VM, SH_F_XN
 , SH_F_XM, SH_F_OP, SH_F_EXT, SH_F_RSVD
 , SH_F_LEFT, SH_F_RIGHT, SH_F_DEST, SH_F_LEFT_RIGHT
 , SH_F_TRA, SH_F_TRB, SH_F_LIKELY, SH_F_25
 , SH_F_8_2, SH_F_IMM6, SH_F_IMM10, SH_F_IMM16
 , SH_F_UIMM6, SH_F_UIMM16, SH_F_DISP6, SH_F_DISP6X32
 , SH_F_DISP10, SH_F_DISP10X8, SH_F_DISP10X4, SH_F_DISP10X2
 , SH_F_DISP16, SH_F_MAX
} IFIELD_TYPE;

#define MAX_IFLD ((int) SH_F_MAX)

/* Hardware attribute indices.  */

/* Enum declaration for cgen_hw attrs.  */
typedef enum cgen_hw_attr {
  CGEN_HW_VIRTUAL, CGEN_HW_CACHE_ADDR, CGEN_HW_PC, CGEN_HW_PROFILE
 , CGEN_HW_END_BOOLS, CGEN_HW_START_NBOOLS = 31, CGEN_HW_MACH, CGEN_HW_END_NBOOLS
} CGEN_HW_ATTR;

/* Number of non-boolean elements in cgen_hw_attr.  */
#define CGEN_HW_NBOOL_ATTRS (CGEN_HW_END_NBOOLS - CGEN_HW_START_NBOOLS - 1)

/* Enum declaration for sh hardware types.  */
typedef enum cgen_hw_type {
  HW_H_MEMORY, HW_H_SINT, HW_H_UINT, HW_H_ADDR
 , HW_H_IADDR, HW_H_PC, HW_H_GR, HW_H_GRC
 , HW_H_CR, HW_H_SR, HW_H_FPSCR, HW_H_FRBIT
 , HW_H_SZBIT, HW_H_PRBIT, HW_H_SBIT, HW_H_MBIT
 , HW_H_QBIT, HW_H_FR, HW_H_FP, HW_H_FV
 , HW_H_FMTX, HW_H_DR, HW_H_TR, HW_H_ENDIAN
 , HW_H_ISM, HW_H_FRC, HW_H_DRC, HW_H_XF
 , HW_H_XD, HW_H_FVC, HW_H_FPCCR, HW_H_GBR
 , HW_H_PR, HW_H_MACL, HW_H_MACH, HW_H_TBIT
 , HW_MAX
} CGEN_HW_TYPE;

#define MAX_HW ((int) HW_MAX)

/* Operand attribute indices.  */

/* Enum declaration for cgen_operand attrs.  */
typedef enum cgen_operand_attr {
  CGEN_OPERAND_VIRTUAL, CGEN_OPERAND_PCREL_ADDR, CGEN_OPERAND_ABS_ADDR, CGEN_OPERAND_SIGN_OPT
 , CGEN_OPERAND_SIGNED, CGEN_OPERAND_NEGATIVE, CGEN_OPERAND_RELAX, CGEN_OPERAND_SEM_ONLY
 , CGEN_OPERAND_END_BOOLS, CGEN_OPERAND_START_NBOOLS = 31, CGEN_OPERAND_MACH, CGEN_OPERAND_ISA
 , CGEN_OPERAND_END_NBOOLS
} CGEN_OPERAND_ATTR;

/* Number of non-boolean elements in cgen_operand_attr.  */
#define CGEN_OPERAND_NBOOL_ATTRS (CGEN_OPERAND_END_NBOOLS - CGEN_OPERAND_START_NBOOLS - 1)

/* Enum declaration for sh operand types.  */
typedef enum cgen_operand_type {
  SH_OPERAND_PC, SH_OPERAND_ENDIAN, SH_OPERAND_ISM, SH_OPERAND_RM
 , SH_OPERAND_RN, SH_OPERAND_R0, SH_OPERAND_FRN, SH_OPERAND_FRM
 , SH_OPERAND_FVN, SH_OPERAND_FVM, SH_OPERAND_DRN, SH_OPERAND_DRM
 , SH_OPERAND_IMM4, SH_OPERAND_IMM8, SH_OPERAND_UIMM8, SH_OPERAND_IMM4X2
 , SH_OPERAND_IMM4X4, SH_OPERAND_IMM8X2, SH_OPERAND_IMM8X4, SH_OPERAND_DISP8
 , SH_OPERAND_DISP12, SH_OPERAND_RM64, SH_OPERAND_RN64, SH_OPERAND_GBR
 , SH_OPERAND_PR, SH_OPERAND_FPSCR, SH_OPERAND_TBIT, SH_OPERAND_SBIT
 , SH_OPERAND_MBIT, SH_OPERAND_QBIT, SH_OPERAND_FPUL, SH_OPERAND_FRBIT
 , SH_OPERAND_SZBIT, SH_OPERAND_PRBIT, SH_OPERAND_MACL, SH_OPERAND_MACH
 , SH_OPERAND_FSDM, SH_OPERAND_FSDN, SH_OPERAND_RD, SH_OPERAND_FRG
 , SH_OPERAND_FRH, SH_OPERAND_FRF, SH_OPERAND_FRGH, SH_OPERAND_FPF
 , SH_OPERAND_FVG, SH_OPERAND_FVH, SH_OPERAND_FVF, SH_OPERAND_MTRXG
 , SH_OPERAND_DRG, SH_OPERAND_DRH, SH_OPERAND_DRF, SH_OPERAND_DRGH
 , SH_OPERAND_CRJ, SH_OPERAND_CRK, SH_OPERAND_TRA, SH_OPERAND_TRB
 , SH_OPERAND_DISP6, SH_OPERAND_DISP6X32, SH_OPERAND_DISP10, SH_OPERAND_DISP10X2
 , SH_OPERAND_DISP10X4, SH_OPERAND_DISP10X8, SH_OPERAND_DISP16, SH_OPERAND_IMM6
 , SH_OPERAND_IMM10, SH_OPERAND_IMM16, SH_OPERAND_UIMM6, SH_OPERAND_UIMM16
 , SH_OPERAND_LIKELY, SH_OPERAND_MAX
} CGEN_OPERAND_TYPE;

/* Number of operands types.  */
#define MAX_OPERANDS 72

/* Maximum number of operands referenced by any insn.  */
#define MAX_OPERAND_INSTANCES 8

/* Insn attribute indices.  */

/* Enum declaration for cgen_insn attrs.  */
typedef enum cgen_insn_attr {
  CGEN_INSN_ALIAS, CGEN_INSN_VIRTUAL, CGEN_INSN_UNCOND_CTI, CGEN_INSN_COND_CTI
 , CGEN_INSN_SKIP_CTI, CGEN_INSN_DELAY_SLOT, CGEN_INSN_RELAXABLE, CGEN_INSN_RELAX
 , CGEN_INSN_NO_DIS, CGEN_INSN_PBB, CGEN_INSN_ILLSLOT, CGEN_INSN_FP_INSN
 , CGEN_INSN_END_BOOLS, CGEN_INSN_START_NBOOLS = 31, CGEN_INSN_MACH, CGEN_INSN_ISA
 , CGEN_INSN_END_NBOOLS
} CGEN_INSN_ATTR;

/* Number of non-boolean elements in cgen_insn_attr.  */
#define CGEN_INSN_NBOOL_ATTRS (CGEN_INSN_END_NBOOLS - CGEN_INSN_START_NBOOLS - 1)

/* cgen.h uses things we just defined.  */
#include "opcode/cgen.h"

/* Attributes.  */
extern const CGEN_ATTR_TABLE sh_cgen_hardware_attr_table[];
extern const CGEN_ATTR_TABLE sh_cgen_ifield_attr_table[];
extern const CGEN_ATTR_TABLE sh_cgen_operand_attr_table[];
extern const CGEN_ATTR_TABLE sh_cgen_insn_attr_table[];

/* Hardware decls.  */

extern CGEN_KEYWORD sh_cgen_opval_h_gr;
extern CGEN_KEYWORD sh_cgen_opval_h_grc;
extern CGEN_KEYWORD sh_cgen_opval_h_cr;
extern CGEN_KEYWORD sh_cgen_opval_h_fr;
extern CGEN_KEYWORD sh_cgen_opval_h_fp;
extern CGEN_KEYWORD sh_cgen_opval_h_fv;
extern CGEN_KEYWORD sh_cgen_opval_h_fmtx;
extern CGEN_KEYWORD sh_cgen_opval_h_dr;
extern CGEN_KEYWORD sh_cgen_opval_h_tr;
extern CGEN_KEYWORD sh_cgen_opval_frc_names;
extern CGEN_KEYWORD sh_cgen_opval_drc_names;
extern CGEN_KEYWORD sh_cgen_opval_xf_names;
extern CGEN_KEYWORD sh_cgen_opval_frc_names;
extern CGEN_KEYWORD sh_cgen_opval_h_fvc;




#endif /* SH_CPU_H */
