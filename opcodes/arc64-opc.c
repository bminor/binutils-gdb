/* Opcode table for ARC64.
   Copyright (C) 2023 Free Software Foundation, Inc.

   Contributed by Claudiu Zissulescu (claziss@synopsys.com)

   This file is part of libopcodes.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "arcxx-opc.inc"

#define F32_BR0 0x00
#define F32_BR1 0x01
#define F32_LD_OFFSET 0x02
#define F32_ST_OFFSET 0x03
#define F32_GEN4 0x04
#define F32_EXT5 0x05
#define F32_EXT6 0x06
#define F32_APEX 0x07

#define F16_COMPACT0 0x08
#define F16_COMPACT1 0x08
#define F16_MOVL 0x08

#define F16_LD_ADD_SUB 0x09

#define LD_ST_R01 0x0A
#define LDI_S 0x0A
#define JLI_S_U10 0x0A

#define F16_JLI_EI 0x0B
#define F32_GEN_OP64 0x0B

/* Macros required for ARCv3 floating point instructions.  */
/* Flags.  */
#define FL_NONE { 0 }
#define FL_CC   { C_FPCC }

/* Arguments.  */
#define ARG_NONE       { 0 }
#define ARG_64FP_3OP   { FA, FB, FC, FD }
#define ARG_128FP_3OP  { FDA, FDB, FDC, FDD }
#define ARG_64FP_2OP   { FA, FB, FC }
#define ARG_64FP_CMP   { FB, FC }
#define ARG_128FP_2OP  { FDA, FDB, FDC }
#define ARG_64FP_1OP   { FA, FC }
#define ARG_64FP_SOP   { FA, FB }
#define ARG_128FP_SOP  { FDA, FDB }

#define ARG_64FP_CVI2F { FA, FRB }
#define ARG_64FP_CVF2I { FRD, FC }

/* Macros to help generating floating point pattern instructions.  */
/* Define FP_TOP.  */
#define FIELDS1(word) (word & 0x1F)
#define FIELDS2(word) (((word & 0x07) << 24) | (((word >> 3) & 0x03) << 12))
#define FIELDS3(word) ((word & 0x1F) << 19)
#define FIELDD(word)  ((word & 0x1F) << 6)
#define FIELDTOP(word) (((word & 0x01) << 5) | ((word >> 1) & 0x07) << 16)
#define FIELDP(word)  ((word & 0x03) << 14)
#define MASK_32BIT(VAL) (0xffffffff & (VAL))

#define INSNFP3OP(TOPF, P)					\
  ((0x1C << 27) | FIELDTOP (TOPF) | FIELDP (P) | (1 << 11))
#define MINSNFP3OP							\
  (MASK_32BIT (~(FIELDS1 (31) | FIELDS2 (31) | FIELDS3 (31) | FIELDD (31))))

/* Define FP_DOP.  */
#define FIELDDOP(ops) ((ops & 0x1f) << 16)

#define INSNFP2OP(DOPF, P)					\
  ((0x1C << 27) | FIELDDOP (DOPF) | FIELDP (P) | (1 << 5))
#define MINSNFP2OP						\
  (MASK_32BIT (~(FIELDS2 (31) | FIELDS1 (31) | FIELDD (31))))

/* Define FP_CVF2F.  */
#define FIELDCVTF(WORD) ((WORD & 0x03) << 16)
#define FIELDU0(BIT) (BIT & 0x01)
#define FIELDU1(BIT) (BIT & 0x02)
#define FIELDU3(BIT) (BIT & 0x08)
#define FIELDU4(BIT) (BIT & 0x10)

#define FP_CVF2F_MACHINE(CVTF, BIT)					\
  ((0x1C << 27) | (0x03 << 21) | FIELDCVTF (CVTF)			\
   | (1 << 5) | (1 << 2) |  FIELDU0 (BIT) | FIELDU3 (BIT) | FIELDU4 (BIT))
#define MFP_CVF2F (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FP_RND.  */
#define FP_RND_MACHINE(CVTF, BIT)					\
((0x1C << 27) | (0x03 << 21) | FIELDCVTF (CVTF) | (1 << 5) | (0x03 << 1) \
 | FIELDU3 (BIT))
#define MFP_RND (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FP_CVF2I.  */
#define FP_CVF2I_MACHINE(CVTF, BIT) ((0x1C << 27) | (0x03 << 21)       \
				     | FIELDCVTF (CVTF) | (1 << 5) | 1 \
				     | FIELDU3 (BIT) | FIELDU1 (BIT))
#define MFP_CVF2I (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FMVVF2I.  */
#define FM_VVF2I(CVTF, BIT) ((0x1C << 27) | (0x03 << 21) | FIELDCVTF (CVTF) \
			     | (1 << 5) | 1 << 4 | 1)
#define MFM_VVF2I (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FP_SOP.  */
#define FP_SOP_MACHINE(SOPF, P)						\
  ((0x1C << 27) | (0x02 << 21) | FIELDCVTF (SOPF) | FIELDP (P) | (1 << 5))
#define MFP_SOP_MACHINE (MASK_32BIT (~(FIELDS1 (31) | FIELDD (31))))

/* Define FP_COP.  */
#define FP_COP_MACHINE(COPF, P)						\
  ((0x1C << 27) | (0x09 << 19) | FIELDCVTF (COPF) | FIELDP (P) | (1 << 5))
#define MFP_COP_MACHINE						\
  (MASK_32BIT (~(FIELDS1 (31) | FIELDD (31) | FIELDS2(31))))

/* Define FP_ZOP.  */
#define INSNFPZOP(COPF)						\
  ((0x1C << 27) | (0x07 << 20) | FIELDCVTF (COPF) | (1 << 5))

/* Define FP_VMVI.  */
#define INSNFPVMVI(WMVF, P)						\
  ((0x1C << 27) | (0x05 << 20) | FIELDCVTF (WMVF) | FIELDP (P) | (1 << 5))
#define MINSNFPCOP (MASK_32BIT (~(FIELDS1 (31) | FIELDD (31) | FIELDS2 (31))))
#define MINSNFPVMVIZ (MASK_32BIT (~(FIELDS1 (31) | FIELDD (31))))

/* Define FP_VMVR.  */
#define INSNFPVMVR(WMVF, P)						\
  ((0x1C << 27) | (0x01 << 23) | FIELDCVTF (WMVF) | FIELDP (P) | (1 << 5))
#define MINSNFPVMVR (MASK_32BIT (~(FIELDS1 (31) | FIELDD (31) | FIELDS2 (31))))

/* Define FP_CVI2F.  */
#define INSNFPCVI2F(CVTF, BIT) ((0x1C << 27) | (0x07 << 21) | FIELDCVTF (CVTF) \
				| (1 << 5) | FIELDU3 (BIT) | FIELDU1 (BIT))
#define MINSNFPCVI2F (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FMVI2F.  */
#define INSNFMVI2F(CVTF, BIT) ((0x1C << 27) | (0x07 << 21) | FIELDCVTF (CVTF) \
			       | (1 << 5) | (1 << 4))
#define MINSNFMVI2F (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FMVF2I.  */
#define INSNFMVF2I(CVTF, BIT) ((0x1C << 27) | (0x03 << 21) | FIELDCVTF (CVTF) \
			       | (1 << 5) | (1 << 4) | (1))
#define MINSNFMVF2I (MASK_32BIT (~(FIELDS2 (31) | FIELDD (31))))

/* Define FP_LOAD.  */
#define FP_LOAD_ENCODING(SIZE, D) (0x0D << 27 | ((SIZE & 0x03) << 1)	\
				   | ((D & 0x01) << 5))
#define MSK_FP_LOAD (MASK_32BIT (~(FIELDB (63) | FIELDD (31) | (0x03 << 3) \
				   | (0x1FF << 15))))

#define FP_LSYM_ENCODING(SIZE, D) (0x0D << 27 | ((SIZE & 0x03) << 1)	\
				   | FIELDB(62) | ((D & 0x01) << 5))
#define MSK_FP_SYM (MASK_32BIT (~(FIELDD (31))))

/* Define FP_STORE.  */
#define FP_STORE_ENCODING(SIZE, D) ((0x0D << 27) | ((SIZE & 0x03) << 1)	\
				    | ((D & 0x01) << 5) | (1))
#define MSK_FP_STORE (MASK_32BIT (~(FIELDB (63) | FIELDD (31) | (0x03 << 3) \
				   | (0x1FF << 15))))
#define FP_SSYM_ENCODING(SIZE, D) (0x0D << 27 | ((SIZE & 0x03) << 1)	\
				   | FIELDB(62) | ((D & 0x01) << 5) | (1))

/* FP Load/Store.  */
#define FP_LOAD(NAME, SIZE, D)						\
  { #NAME, FP_LOAD_ENCODING (SIZE, D), MSK_FP_LOAD, ARC_OPCODE_ARC64, LOAD, \
    NONE, { FA, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 } },		\
  { #NAME, FP_LSYM_ENCODING (SIZE, D), MSK_FP_SYM, ARC_OPCODE_ARC64, LOAD, \
    NONE, { FA, BRAKET, LIMM, BRAKETdup }, FL_NONE },

#define FP_STORE(NAME, SIZE, D)						\
  { #NAME, FP_STORE_ENCODING (SIZE, D), MSK_FP_STORE, ARC_OPCODE_ARC64, STORE, \
    NONE, { FA, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 } },		\
  { #NAME, FP_SSYM_ENCODING (SIZE, D), MSK_FP_SYM, ARC_OPCODE_ARC64, LOAD, \
    NONE, { FA, BRAKET, LIMM, BRAKETdup }, FL_NONE },

/* Macros used to generate conversion instructions.  */
#define FMVF2I_INSN(NAME, CPU, CLASS, SCLASS, OPS, BIT, ARG)		\
  { NAME, INSNFMVF2I (OPS, BIT), MINSNFMVF2I, CPU, CLASS,		\
    SCLASS, ARG, FL_NONE },

#define FMVF2I(NAME, OPS, BIT)				    \
  FMVF2I_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, OPS,   \
	       BIT, ARG_64FP_CVF2I)

#define FMVI2F_INSN(NAME, CPU, CLASS, SCLASS, OPS, BIT, ARG)		\
  { NAME, INSNFMVI2F (OPS, BIT), MINSNFMVI2F, CPU, CLASS,		\
    SCLASS, ARG, FL_NONE },

#define FMVI2F(NAME, OPS, BIT)				   \
  FMVI2F_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, OPS,  \
	       BIT, ARG_64FP_CVI2F)

#define FP_RND_INSN(NAME, CPU, CLASS, SCLASS, OPS, BIT, ARG)		\
  { NAME, FP_RND_MACHINE (OPS, BIT), MFP_RND, CPU, CLASS,		\
    SCLASS, ARG, FL_NONE },

#define FP_RND(NAME, OPS, BIT)				   \
  FP_RND_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, OPS,  \
	       BIT, ARG_64FP_1OP)

#define FP_CVF2F_INSN(NAME, CPU, CLASS, SCLASS, OPS, BIT, ARG)		\
  { NAME, FP_CVF2F_MACHINE (OPS, BIT), MFP_CVF2F, CPU, CLASS,		\
    SCLASS, ARG, FL_NONE },

#define FP_CVF2F(NAME, OPS, BIT)				   \
  FP_CVF2F_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, OPS,	   \
		 BIT, ARG_64FP_1OP)

#define FP_CVF2I_INSN(NAME, CPU, CLASS, SCLASS, OPS, BIT, ARG)		\
  { NAME, FP_CVF2I_MACHINE (OPS, BIT), MFP_CVF2I, CPU, CLASS,		\
    SCLASS, ARG, FL_NONE },

#define FP_CVF2I(NAME, OPS, BIT)				   \
  FP_CVF2I_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, OPS,	   \
		 BIT, ARG_64FP_CVF2I)

#define FP_CVI2F_INSN(NAME, CPU, CLASS, SCLASS, OPS, BIT, ARG)		\
  { NAME, INSNFPCVI2F (OPS, BIT), MINSNFPCVI2F, CPU, CLASS,		\
    SCLASS, ARG, FL_NONE },

#define FP_CVI2F(NAME, OPS, BIT)				   \
  FP_CVI2F_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, OPS,	   \
		 BIT, ARG_64FP_CVI2F)

/* Macro to generate 1 operand extension instruction.  */
#define FP_SOP_INSN(NAME, CPU, CLASS, SCLASS, OPS, PRC, ARG)		\
  { NAME, FP_SOP_MACHINE (OPS, PRC), MFP_SOP_MACHINE, CPU, CLASS, SCLASS, \
    ARG, FL_NONE },

#define FP_SOP(NAME, OPS, PRECISION)				   \
  FP_SOP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, SOPF_ ## OPS, \
	       P_ ## PRECISION, ARG_64FP_SOP)

#define FP_SOP_D(NAME, OPS, PRECISION)				    \
  FP_SOP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, SOPF_ ## OPS, \
	       P_ ## PRECISION, ARG_128FP_SOP)

/* Macro to generate 2 operand extension instruction.  */
#define FP_DOP_INSN(NAME, CPU, CLASS, SCLASS, OPS, PRC, ARG)	\
  { NAME, INSNFP2OP (OPS, PRC), MINSNFP2OP, CPU, CLASS, SCLASS,	\
    ARG, FL_NONE },

#define FP_DOP(NAME, OPS, PRECISION)				   \
  FP_DOP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, DOPF_ ## OPS, \
	       P_ ## PRECISION, ARG_64FP_2OP)

#define FP_DOPC_INSN(NAME, CPU, CLASS, SCLASS, OPS, PRC, ARG)		\
  { NAME, INSNFP2OP (OPS, PRC) | FIELDD (0), MINSNFP2OP, CPU,		\
    CLASS, SCLASS, ARG, FL_NONE },

#define FP_DOP_C(NAME, OPS, PRECISION)				    \
  FP_DOPC_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, DOPF_ ## OPS, \
		P_ ## PRECISION, ARG_64FP_CMP)

#define FP_DOP_D(NAME, OPS, PRECISION)				   \
  FP_DOP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, DOPF_ ## OPS, \
	       P_ ## PRECISION, ARG_128FP_2OP)

/* Macro to generate 3 operand generic instruction.  */
#define FP_TOP_INSN(NAME, CPU, CLASS, SCLASS, TOPF, P, ARG)		\
  { NAME, INSNFP3OP (TOPF, P),  MINSNFP3OP,  CPU, CLASS, SCLASS,	\
    ARG, FL_NONE },

#define FP_TOP(NAME, OPS, PRECISION)					\
  FP_TOP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, TOPF_ ## OPS,	\
	       P_ ## PRECISION, ARG_64FP_3OP)

#define FP_TOP_D(NAME, OPS, PRECISION)				    \
  FP_TOP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, TOPF_ ## OPS, \
	       P_ ## PRECISION, ARG_128FP_3OP)

/* Conditional mov instructions.  */
#define FP_COP_INSN(NAME, CPU, CLASS, SCLASS, OPS, PRC, ARG)		\
  { NAME, FP_COP_MACHINE (OPS, PRC), MFP_COP_MACHINE, CPU, CLASS, SCLASS, \
    ARG, FL_CC },

#define FP_COP(NAME, OPS, PRECISION)					\
  FP_COP_INSN (#NAME, ARC_OPCODE_ARC64, FLOAT, NONE, COPF_ ## OPS,	\
	       P_ ## PRECISION, ARG_64FP_SOP)

#define FP_EXT(NAME, PRECISION)						\
  {#NAME, INSNFPVMVI (0x00, P_ ## PRECISION), MINSNFPCOP,		\
   ARC_OPCODE_ARC64, FLOAT, NONE, { FA, FB, BRAKET, UIMM5_FP,		\
     BRAKETdup }, FL_NONE },						\
  {#NAME, INSNFPVMVR (0x00, P_ ## PRECISION), MINSNFPVMVR,		\
      ARC_OPCODE_ARC64,  FLOAT, NONE, { FA, FB, BRAKET, FRB, BRAKETdup}, \
      FL_NONE },

#define FP_INS(NAME, PRECISION)						\
  {#NAME, INSNFPVMVI (0x01, P_ ## PRECISION), MINSNFPCOP,		\
   ARC_OPCODE_ARC64, FLOAT, NONE, { FA, BRAKET, UIMM5_FP, BRAKETdup,	\
     FB }, FL_NONE },							\
  {#NAME, INSNFPVMVR (0x01, P_ ## PRECISION), MINSNFPVMVR,		\
      ARC_OPCODE_ARC64, FLOAT, NONE, { FA, BRAKET, FRB, BRAKETdup,	\
	FB }, FL_NONE },

#define FP_REP(NAME, PRECISION)						\
  {#NAME, INSNFPVMVI (0x02, P_ ## PRECISION) | FIELDS2 (0x00),		\
   MINSNFPVMVIZ, ARC_OPCODE_ARC64, FLOAT, NONE, { FA, FB }, FL_NONE },


/* Common combinations of FLAGS.  */
#define FLAGS_NONE { 0 }
#define FLAGS_F    { C_F }
#define FLAGS_CC   { C_CC }
#define FLAGS_CCF  { C_CC, C_F }

/* Common combination of arguments.  */
#define ARG_NONE		{ 0 }
#define ARG_32BIT_RARBRC	{ RA, RB, RC }
#define ARG_32BIT_ZARBRC	{ ZA, RB, RC }
#define ARG_32BIT_RBRBRC	{ RB, RBdup, RC }
#define ARG_32BIT_RARBU6	{ RA, RB, UIMM6_20 }
#define ARG_32BIT_ZARBU6	{ ZA, RB, UIMM6_20 }
#define ARG_32BIT_RBRBU6	{ RB, RBdup, UIMM6_20 }
#define ARG_32BIT_RBRBS12	{ RB, RBdup, SIMM12_20 }
#define ARG_32BIT_RALIMMRC	{ RA, LIMM, RC }
#define ARG_32BIT_RARBLIMM	{ RA, RB, LIMM }
#define ARG_32BIT_ZALIMMRC	{ ZA, LIMM, RC }
#define ARG_32BIT_ZARBLIMM	{ ZA, RB, LIMM }

#define ARG_32BIT_RBRBLIMM	{ RB, RBdup, LIMM }
#define ARG_32BIT_RALIMMU6	{ RA, LIMM, UIMM6_20 }
#define ARG_32BIT_ZALIMMU6	{ ZA, LIMM, UIMM6_20 }

#define ARG_32BIT_ZALIMMS12	{ ZA, LIMM, SIMM12_20 }
#define ARG_32BIT_RALIMMLIMM	{ RA, LIMM, LIMMdup }
#define ARG_32BIT_ZALIMMLIMM	{ ZA, LIMM, LIMMdup }

#define ARG_32BIT_RBRC   { RB, RC }
#define ARG_32BIT_ZARC   { ZA, RC }
#define ARG_32BIT_RBU6   { RB, UIMM6_20 }
#define ARG_32BIT_ZAU6   { ZA, UIMM6_20 }
#define ARG_32BIT_RBLIMM { RB, LIMM }
#define ARG_32BIT_ZALIMM { ZA, LIMM }

#define ARG_32BIT_RAXIMMRC	{ RA, XIMM, RC }
#define ARG_32BIT_RARBXIMM	{ RA, RB, XIMM }
#define ARG_32BIT_RBRBXIMM	{ RB, RBdup, XIMM }
#define ARG_32BIT_RAXIMMU6	{ RA, XIMM, UIMM6_20 }

/* Macro to generate 2 operand extension instruction.  */
#define EXTINSN2OPF(NAME, CPU, CLASS, SCLASS, MOP, SOP, FL)	 \
  { NAME, INSN2OP_BC (MOP,SOP), MINSN2OP_BC, CPU, CLASS, SCLASS, \
      ARG_32BIT_RBRC,   FL },					 \
  { NAME, INSN2OP_0C (MOP,SOP), MINSN2OP_0C, CPU, CLASS, SCLASS, \
      ARG_32BIT_ZARC,   FL },					 \
  { NAME, INSN2OP_BU (MOP,SOP), MINSN2OP_BU, CPU, CLASS, SCLASS, \
      ARG_32BIT_RBU6,   FL },					 \
  { NAME, INSN2OP_0U (MOP,SOP), MINSN2OP_0U, CPU, CLASS, SCLASS, \
      ARG_32BIT_ZAU6,   FL },					 \
  { NAME, INSN2OP_BL (MOP,SOP), MINSN2OP_BL, CPU, CLASS, SCLASS, \
      ARG_32BIT_RBLIMM, FL },					 \
  { NAME, INSN2OP_0L (MOP,SOP), MINSN2OP_0L, CPU, CLASS, SCLASS, \
      ARG_32BIT_ZALIMM, FL },

#define EXTINSN2OP(NAME, CPU, CLASS, SCLASS, MOP, SOP)		 \
  EXTINSN2OPF(NAME, CPU, CLASS, SCLASS, MOP, SOP, FLAGS_F)

/* Macro to generate 3 operand extesion instruction.  */
#define EXTINSN3OP(NAME, CPU, CLASS, SCLASS, MOP, SOP)			\
  { NAME, INSN3OP_ABC (MOP,SOP),  MINSN3OP_ABC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBRC,     FLAGS_F },					\
  { NAME, INSN3OP_0BC (MOP,SOP),  MINSN3OP_0BC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZARBRC,     FLAGS_F   },				\
  { NAME, INSN3OP_CBBC (MOP,SOP), MINSN3OP_CBBC, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBRC,     FLAGS_CCF },				\
  { NAME, INSN3OP_ABU (MOP,SOP),  MINSN3OP_ABU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBU6,     FLAGS_F   },				\
  { NAME, INSN3OP_0BU (MOP,SOP),  MINSN3OP_0BU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZARBU6,     FLAGS_F   },				\
  { NAME, INSN3OP_CBBU (MOP,SOP), MINSN3OP_CBBU, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBU6,     FLAGS_CCF },				\
  { NAME, INSN3OP_BBS (MOP,SOP),  MINSN3OP_BBS,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBS12,    FLAGS_F   },				\
  { NAME, INSN3OP_ALC (MOP,SOP),  MINSN3OP_ALC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RALIMMRC,   FLAGS_F   },				\
  { NAME, INSN3OP_ABL (MOP,SOP),  MINSN3OP_ABL,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBLIMM,   FLAGS_F   },				\
  { NAME, INSN3OP_0LC (MOP,SOP),  MINSN3OP_0LC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMRC,   FLAGS_F   },				\
  { NAME, INSN3OP_0BL (MOP,SOP),  MINSN3OP_0BL,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZARBLIMM,   FLAGS_F   },				\
  { NAME, INSN3OP_C0LC (MOP,SOP), MINSN3OP_C0LC, CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMRC,   FLAGS_CCF },				\
  { NAME, INSN3OP_CBBL (MOP,SOP), MINSN3OP_CBBL, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBLIMM,   FLAGS_CCF },				\
  { NAME, INSN3OP_ALU (MOP,SOP),  MINSN3OP_ALU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RALIMMU6,   FLAGS_F   },				\
  { NAME, INSN3OP_0LU (MOP,SOP),  MINSN3OP_0LU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMU6,   FLAGS_F   },				\
  { NAME, INSN3OP_C0LU (MOP,SOP), MINSN3OP_C0LU, CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMU6,   FLAGS_CCF },				\
  { NAME, INSN3OP_0LS (MOP,SOP),  MINSN3OP_0LS,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMS12,  FLAGS_F   },				\
  { NAME, INSN3OP_ALL (MOP,SOP),  MINSN3OP_ALL,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RALIMMLIMM, FLAGS_F   },				\
  { NAME, INSN3OP_0LL (MOP,SOP),  MINSN3OP_0LL,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMLIMM, FLAGS_F   },				\
  { NAME, INSN3OP_C0LL (MOP,SOP), MINSN3OP_C0LL, CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZALIMMLIMM, FLAGS_CCF },


/* Generate sign extend 32-bit immediate instructions.  */
#define INSN3OP_AXC(MOP,SOP)  (INSN3OP (MOP,SOP) | FIELDB (60))
#define INSN3OP_ABX(MOP,SOP)  (INSN3OP (MOP,SOP) | FIELDC (60))
#define INSN3OP_CBBX(MOP,SOP) (INSN3OP (MOP,SOP) | (0x03 << 22) | FIELDC (60))
#define INSN3OP_AXU(MOP,SOP)  (INSN3OP (MOP,SOP) | (0x01 << 22) | FIELDB (60))

/* Macro to generate 3 operand 64bit instruction.  */
#define OP64INSN3OP(NAME, CPU, CLASS, SCLASS, MOP, SOP)			\
  { NAME, INSN3OP_ABC (MOP,SOP),  MINSN3OP_ABC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBRC,     FLAGS_F },					\
  { NAME, INSN3OP_0BC (MOP,SOP),  MINSN3OP_0BC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZARBRC,     FLAGS_F   },				\
  { NAME, INSN3OP_CBBC (MOP,SOP), MINSN3OP_CBBC, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBRC,     FLAGS_CCF },				\
  { NAME, INSN3OP_ABU (MOP,SOP),  MINSN3OP_ABU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBU6,     FLAGS_F   },				\
  { NAME, INSN3OP_0BU (MOP,SOP),  MINSN3OP_0BU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_ZARBU6,     FLAGS_F   },				\
  { NAME, INSN3OP_CBBU (MOP,SOP), MINSN3OP_CBBU, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBU6,     FLAGS_CCF },				\
  { NAME, INSN3OP_BBS (MOP,SOP),  MINSN3OP_BBS,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBS12,    FLAGS_F   },				\
  { NAME, INSN3OP_AXC (MOP,SOP),  MINSN3OP_ALC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RAXIMMRC,   FLAGS_F   },				\
  { NAME, INSN3OP_ABX (MOP,SOP),  MINSN3OP_ABL,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBXIMM,   FLAGS_F   },				\
  { NAME, INSN3OP_CBBX (MOP,SOP), MINSN3OP_CBBL, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBXIMM,   FLAGS_CCF },				\
  { NAME, INSN3OP_AXU (MOP,SOP),  MINSN3OP_ALU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RAXIMMU6,   FLAGS_F   },				\
  { NAME, INSN3OP_ALC (MOP,SOP),  MINSN3OP_ALC,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RALIMMRC,   FLAGS_F   },				\
  { NAME, INSN3OP_ABL (MOP,SOP),  MINSN3OP_ABL,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RARBLIMM,   FLAGS_F   },				\
  { NAME, INSN3OP_CBBL (MOP,SOP), MINSN3OP_CBBL, CPU, CLASS, SCLASS,	\
      ARG_32BIT_RBRBLIMM,   FLAGS_CCF },				\
  { NAME, INSN3OP_ALU (MOP,SOP),  MINSN3OP_ALU,  CPU, CLASS, SCLASS,	\
      ARG_32BIT_RALIMMU6,   FLAGS_F   },


#define STDL_ENCODING(K, ZZ) ((F32_ST_OFFSET << 27) | (1 << 5) | (ZZ << 1) \
			      | (K))
#define MSK_STDL (MASK_32BIT (~(FIELDB (63) | (0x1ff << 15) | FIELDC (63) \
				| (0x3 << 3))))
#define STDL_ASYM_ENCODING(K, ZZ, X) ((F32_ST_OFFSET << 27) | FIELDB (X) \
				   | (1 << 5) | (ZZ << 1) | (K))
#define STDL_DIMM_ENCODING(K, ZZ, X) ((F32_ST_OFFSET << 27) | FIELDC (X) \
				   | (1 << 5) | (ZZ << 1) | (K))

/* stdl<.aa> c,[b,s9]
   stdl<.aa> w6,[b,s9]
   stdl<.as> c,[ximm]
   stdl<.as> w6,[ximm]
   stdl<.aa> ximm,[b,s9]
   stdl<.aa> limm,[b,s9]
*/
#define STDL								\
  { "stdl", STDL_ENCODING (0, 0x03), MSK_STDL, ARC_OPCODE_ARC64, STORE, \
    NONE, { RCD, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},		\
  { "stdl", STDL_ENCODING (1, 0x02), MSK_STDL, ARC_OPCODE_ARC64, STORE, \
      NONE, { W6, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},	\
  { "stdl", STDL_ASYM_ENCODING (0, 0x03, 60), 0xFF007027, ARC_OPCODE_ARC64, \
      STORE, NONE, { RCD, BRAKET, XIMM, BRAKETdup }, {C_AS27 }},	\
  { "stdl", STDL_ASYM_ENCODING (1, 0x02, 60), 0xFF007027, ARC_OPCODE_ARC64, \
      STORE, NONE, { W6, BRAKET, XIMM, BRAKETdup }, { C_AS27 }},	\
  { "stdl", STDL_ASYM_ENCODING (0, 0x03, 62), 0xFF007027, ARC_OPCODE_ARC64, \
      STORE, NONE, { RCD, BRAKET, LIMM, BRAKETdup }, {C_AS27 }},	\
  { "stdl", STDL_ASYM_ENCODING (1, 0x02, 62), 0xFF007027, ARC_OPCODE_ARC64, \
      STORE, NONE, { W6, BRAKET, LIMM, BRAKETdup }, { C_AS27 }},	\
  { "stdl", STDL_DIMM_ENCODING (0, 0x03, 60), 0XF8000FE7, ARC_OPCODE_ARC64, \
      STORE, NONE, { XIMM, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }}, \
  { "stdl", STDL_DIMM_ENCODING (0, 0x03, 62), 0xF8000FE7, ARC_OPCODE_ARC64, \
      STORE, NONE, { LIMM, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},

#define STL_ENCODING(DI) ((F32_ST_OFFSET << 27) | (DI << 5) | (0X03 << 1) \
			  | (1))
#define MSK_STL (MASK_32BIT (~(FIELDB (63) | (0x1ff << 15) | FIELDC (63) \
			       | (0x03 << 3))))
#define STL_ASYM_ENCODING(DI, X) ((F32_ST_OFFSET << 27) | (DI << 5) \
				  | (0X03 << 1) | FIELDB (X) | (1))
#define STL_DSYM_ENCODING(X) ((F32_ST_OFFSET << 27) | (0X03 << 1)	\
			       | FIELDC (X) | (1))

/* stl<.aa> c,[b,s9]
   stl<.aa> w6,[b,s9]
   stl<.as> c,[ximm]
   stl<.as> w6,[ximm]
   stl<.aa> ximm,[b,s9]
   stl<.aa> limm,[b,s9]
*/
#define STL								\
  { "stl", STL_ENCODING (0), MSK_STL, ARC_OPCODE_ARC64, STORE, NONE,	\
    { RC, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},		\
  { "stl", STL_ENCODING (1), MSK_STL, ARC_OPCODE_ARC64, STORE, NONE,	\
    { W6, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},		\
  { "stl", STL_ASYM_ENCODING (0, 60), 0xFFFFF03F, ARC_OPCODE_ARC64, STORE, \
      NONE, { RC, BRAKET, XIMM, BRAKETdup }, { C_AS27 }},		\
  { "stl", STL_ASYM_ENCODING (1, 60), 0xFFFFF03F, ARC_OPCODE_ARC64, STORE, \
      NONE, { W6, BRAKET, XIMM, BRAKETdup }, { C_AS27 }},		\
  { "stl", STL_ASYM_ENCODING (0, 62), 0xFFFFF03F, ARC_OPCODE_ARC64, STORE, \
      NONE, { RC, BRAKET, LIMM, BRAKETdup }, { C_AS27 }},		\
  { "stl", STL_ASYM_ENCODING (1, 62), 0xFFFFF03F, ARC_OPCODE_ARC64, STORE, \
      NONE, { W6, BRAKET, LIMM, BRAKETdup }, { C_AS27 }},		\
  { "stl", STL_DSYM_ENCODING (60), 0xF8000FC7, ARC_OPCODE_ARC64, STORE,	\
      NONE, { XIMM, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},	\
  { "stl", STL_DSYM_ENCODING (62), 0xF8000FC7, ARC_OPCODE_ARC64, STORE,	\
      NONE, { LIMM, BRAKET, RB, SIMM9_8, BRAKETdup }, { C_AA27 }},

/* The opcode table.

   The format of the opcode table is:

   NAME OPCODE MASK CPU CLASS SUBCLASS { OPERANDS } { FLAGS }.

   The table is organised such that, where possible, all instructions with
   the same mnemonic are together in a block.  When the assembler searches
   for a suitable instruction the entries are checked in table order, so
   more specific, or specialised cases should appear earlier in the table.

   As an example, consider two instructions 'add a,b,u6' and 'add
   a,b,limm'.  The first takes a 6-bit immediate that is encoded within the
   32-bit instruction, while the second takes a 32-bit immediate that is
   encoded in a follow-on 32-bit, making the total instruction length
   64-bits.  In this case the u6 variant must appear first in the table, as
   all u6 immediates could also be encoded using the 'limm' extension,
   however, we want to use the shorter instruction wherever possible.

   It is possible though to split instructions with the same mnemonic into
   multiple groups.  However, the instructions are still checked in table
   order, even across groups.  The only time that instructions with the
   same mnemonic should be split into different groups is when different
   variants of the instruction appear in different architectures, in which
   case, grouping all instructions from a particular architecture together
   might be preferable to merging the instruction into the main instruction
   table.

   An example of this split instruction groups can be found with the 'sync'
   instruction.  The core arc architecture provides a 'sync' instruction,
   while the nps instruction set extension provides 'sync.rd' and
   'sync.wr'.  The rd/wr flags are instruction flags, not part of the
   mnemonic, so we end up with two groups for the sync instruction, the
   first within the core arc instruction table, and the second within the
   nps extension instructions.  */

const struct arc_opcode arc_opcodes[] =
{

  /* STL and STDL instructions.  */
  STL
  STDL

#include "arc64-tbl.h"

  FP_TOP (fhmadd , FMADD , HALF)
  FP_TOP (fhmsub , FMSUB , HALF)
  FP_TOP (fhnmadd, FNMADD, HALF)
  FP_TOP (fhnmsub, FNMSUB, HALF)

  FP_TOP (fsmadd , FMADD , SINGLE)
  FP_TOP (fsmsub , FMSUB , SINGLE)
  FP_TOP (fsnmadd, FNMADD, SINGLE)
  FP_TOP (fsnmsub, FNMSUB, SINGLE)

  FP_TOP (fdmadd , FMADD , DOUBLE)
  FP_TOP (fdmsub , FMSUB , DOUBLE)
  FP_TOP (fdnmadd, FNMADD, DOUBLE)
  FP_TOP (fdnmsub, FNMSUB, DOUBLE)

  /* Vectors.  */
  FP_TOP (vfhmadd , VFMADD , HALF)
  FP_TOP (vfhmsub , VFMSUB , HALF)
  FP_TOP (vfhnmadd, VFNMADD, HALF)
  FP_TOP (vfhnmsub, VFNMSUB, HALF)
  FP_TOP (vfhmadds , VFMADDS , HALF)
  FP_TOP (vfhmsubs , VFMSUBS , HALF)
  FP_TOP (vfhnmadds, VFNMADDS, HALF)
  FP_TOP (vfhnmsubs, VFNMSUBS, HALF)

  FP_TOP (vfsmadd , VFMADD , SINGLE)
  FP_TOP (vfsmsub , VFMSUB , SINGLE)
  FP_TOP (vfsnmadd, VFNMADD, SINGLE)
  FP_TOP (vfsnmsub, VFNMSUB, SINGLE)
  FP_TOP (vfsmadds , VFMADDS , SINGLE)
  FP_TOP (vfsmsubs , VFMSUBS , SINGLE)
  FP_TOP (vfsnmadds, VFNMADDS, SINGLE)
  FP_TOP (vfsnmsubs, VFNMSUBS, SINGLE)

  FP_TOP_D (vfdmadd , VFMADD , DOUBLE)
  FP_TOP_D (vfdmsub , VFMSUB , DOUBLE)
  FP_TOP_D (vfdnmadd, VFNMADD, DOUBLE)
  FP_TOP_D (vfdnmsub, VFNMSUB, DOUBLE)
  FP_TOP_D (vfdmadds , VFMADDS , DOUBLE)
  FP_TOP_D (vfdmsubs , VFMSUBS , DOUBLE)
  FP_TOP_D (vfdnmadds, VFNMADDS, DOUBLE)
  FP_TOP_D (vfdnmsubs, VFNMSUBS, DOUBLE)

  /* 2OPS.  */
  FP_DOP (fhadd  , FADD  , HALF)
  FP_DOP (fhsub  , FSUB  , HALF)
  FP_DOP (fhmul  , FMUL  , HALF)
  FP_DOP (fhdiv  , FDIV  , HALF)
  FP_DOP (fhmin  , FMIN  , HALF)
  FP_DOP (fhmax  , FMAX  , HALF)
  FP_DOP (fhsgnj , FSGNJ , HALF)
  FP_DOP (fhsgnjn, FSGNJN, HALF)
  FP_DOP (fhsgnjx, FSGNJX, HALF)

  FP_DOP (fsadd  , FADD  , SINGLE)
  FP_DOP (fssub  , FSUB  , SINGLE)
  FP_DOP (fsmul  , FMUL  , SINGLE)
  FP_DOP (fsdiv  , FDIV  , SINGLE)
  FP_DOP (fsmin  , FMIN  , SINGLE)
  FP_DOP (fsmax  , FMAX  , SINGLE)
  FP_DOP (fssgnj , FSGNJ , SINGLE)
  FP_DOP (fssgnjn, FSGNJN, SINGLE)
  FP_DOP (fssgnjx, FSGNJX, SINGLE)

  FP_DOP (fdadd  , FADD  , DOUBLE)
  FP_DOP (fdsub  , FSUB  , DOUBLE)
  FP_DOP (fdmul  , FMUL  , DOUBLE)
  FP_DOP (fddiv  , FDIV  , DOUBLE)
  FP_DOP (fdmin  , FMIN  , DOUBLE)
  FP_DOP (fdmax  , FMAX  , DOUBLE)
  FP_DOP (fdsgnj , FSGNJ , DOUBLE)
  FP_DOP (fdsgnjn, FSGNJN, DOUBLE)
  FP_DOP (fdsgnjx, FSGNJX, DOUBLE)

  FP_DOP_C (fhcmp , FCMP , HALF)
  FP_DOP_C (fhcmpf, FCMPF, HALF)
  FP_DOP_C (fscmp , FCMP , SINGLE)
  FP_DOP_C (fscmpf, FCMPF, SINGLE)
  FP_DOP_C (fdcmp , FCMP , DOUBLE)
  FP_DOP_C (fdcmpf, FCMPF, DOUBLE)

  /* Vectors.  */
  FP_DOP (vfhadd , VFADD , HALF)
  FP_DOP (vfhsub , VFSUB , HALF)
  FP_DOP (vfhmul , VFMUL , HALF)
  FP_DOP (vfhdiv , VFDIV , HALF)
  FP_DOP (vfhadds, VFADDS, HALF)
  FP_DOP (vfhsubs, VFSUBS, HALF)
  FP_DOP (vfhmuls, VFMULS, HALF)
  FP_DOP (vfhdivs, VFDIVS, HALF)

  FP_DOP (vfhunpkl , VFUNPKL , HALF)
  FP_DOP (vfhunpkm , VFUNPKM , HALF)
  FP_DOP (vfhpackl , VFPACKL , HALF)
  FP_DOP (vfhpackm , VFPACKM , HALF)
  FP_DOP (vfhbflyl , VFBFLYL , HALF)
  FP_DOP (vfhbflym , VFBFLYM , HALF)
  FP_DOP (vfhaddsub, VFADDSUB, HALF)
  FP_DOP (vfhsubadd, VFSUBADD, HALF)

  FP_DOP (vfsadd , VFADD , SINGLE)
  FP_DOP (vfssub , VFSUB , SINGLE)
  FP_DOP (vfsmul , VFMUL , SINGLE)
  FP_DOP (vfsdiv , VFDIV , SINGLE)
  FP_DOP (vfsadds, VFADDS, SINGLE)
  FP_DOP (vfssubs, VFSUBS, SINGLE)
  FP_DOP (vfsmuls, VFMULS, SINGLE)
  FP_DOP (vfsdivs, VFDIVS, SINGLE)

  FP_DOP (vfsunpkl , VFUNPKL , SINGLE)
  FP_DOP (vfsunpkm , VFUNPKM , SINGLE)
  FP_DOP (vfspackl , VFPACKL , SINGLE)
  FP_DOP (vfspackm , VFPACKM , SINGLE)
  FP_DOP (vfsbflyl , VFBFLYL , SINGLE)
  FP_DOP (vfsbflym , VFBFLYM , SINGLE)
  FP_DOP (vfsaddsub, VFADDSUB, SINGLE)
  FP_DOP (vfssubadd, VFSUBADD, SINGLE)

  FP_DOP_D (vfdadd , VFADD , DOUBLE)
  FP_DOP_D (vfdsub , VFSUB , DOUBLE)
  FP_DOP_D (vfdmul , VFMUL , DOUBLE)
  FP_DOP_D (vfddiv , VFDIV , DOUBLE)
  FP_DOP_D (vfdadds, VFADDS, DOUBLE)
  FP_DOP_D (vfdsubs, VFSUBS, DOUBLE)
  FP_DOP_D (vfdmuls, VFMULS, DOUBLE)
  FP_DOP_D (vfddivs, VFDIVS, DOUBLE)

  FP_DOP_D (vfdunpkl , VFUNPKL , DOUBLE)
  FP_DOP_D (vfdunpkm , VFUNPKM , DOUBLE)
  FP_DOP_D (vfdpackl , VFPACKL , DOUBLE)
  FP_DOP_D (vfdpackm , VFPACKM , DOUBLE)
  FP_DOP_D (vfdbflyl , VFBFLYL , DOUBLE)
  FP_DOP_D (vfdbflym , VFBFLYM , DOUBLE)
  FP_DOP_D (vfdaddsub, VFADDSUB, DOUBLE)
  FP_DOP_D (vfdsubadd, VFSUBADD, DOUBLE)

  FP_SOP (fhsqrt, FSQRT, HALF)
  FP_SOP (fssqrt, FSQRT, SINGLE)
  FP_SOP (fdsqrt, FSQRT, DOUBLE)
  FP_SOP (vfhsqrt, VFSQRT, HALF)
  FP_SOP (vfssqrt, VFSQRT, SINGLE)
  FP_SOP_D (vfdsqrt, VFSQRT,DOUBLE)

  FP_SOP (vfhexch, VFEXCH, HALF)
  FP_SOP (vfsexch, VFEXCH, SINGLE)
  FP_SOP_D (vfdexch, VFEXCH, DOUBLE)

  FP_COP (fhmov, FMOV, HALF)
  FP_COP (fsmov, FMOV, SINGLE)
  FP_COP (fdmov, FMOV, DOUBLE)
  FP_COP (vfhmov, VFMOV, HALF)
  FP_COP (vfsmov, VFMOV, SINGLE)
  FP_COP (vfdmov, VFMOV, DOUBLE)

  FP_CVI2F (fuint2s, FUINT2S, 0x00)
  FP_CVI2F (fuint2d, FUINT2D, 0x00)
  FP_CVI2F (ful2s,   FUL2S,   0x00)
  FP_CVI2F (ful2d,   FUL2D,   0x00)

  FP_CVF2I (fs2uint, FS2UINT, 0x01)
  FP_CVF2I (fs2ul,   FS2UL,   0x01)
  FP_CVF2I (fd2uint, FD2UINT, 0x01)
  FP_CVF2I (fd2ul,   FD2UL,   0x01)

  FP_CVI2F (fint2s, FINT2S, 0x02)
  FP_CVI2F (fint2d, FINT2D, 0x02)
  FP_CVI2F (fl2s,   FL2S,   0x02)
  FP_CVI2F (fl2d,   FL2D,   0x02)

  FP_CVF2I (fs2int, FS2INT, 0x03)
  FP_CVF2I (fs2l,   FS2L,   0x03)
  FP_CVF2I (fd2int, FD2INT, 0x03)
  FP_CVF2I (fd2l,   FD2L,   0x03)

  FP_CVF2F (fs2d, FS2D, 0x04)
  FP_CVF2F (fd2s, FD2S, 0x04)

  FP_RND (fsrnd, FSRND, 0x06)
  FP_RND (fdrnd, FDRND, 0x06)

  FP_CVF2I (fs2uint_rz,  F2UINT_RZ,  0x09)
  FP_CVF2I (fs2ul_rz,   FS2UL_RZ,   0x09)
  FP_CVF2I (fd2uint_rz, FD2UINT_RZ, 0x09)
  FP_CVF2I (fd2ul_rz,   FD2UL_RZ,   0x09)

  FP_CVF2I (fs2int_rz,  FSINT_RZ,  0x0B)
  FP_CVF2I (fs2l_rz,   FS2L_RZ,   0x0B)
  FP_CVF2I (fd2int_rz, FD2INT_RZ, 0x0B)
  FP_CVF2I (fd2l_rz,   FD2L_RZ,   0x0B)

  FP_RND (fsrnd_rz, FSRND_RZ, 0x0E)
  FP_RND (fdrnd_rz, FDRND_RZ, 0x0E)

  FMVI2F (fmvi2s, FMVI2S, 0x10)
  FMVI2F (fmvl2d, FMVL2D, 0x10)

  FMVF2I (fmvs2i, FMVS2I, 0x11)
  FMVF2I (fmvd2l, FMVD2L, 0x11)

  FP_CVF2F (fs2h, FS2H, 0x14)
  FP_CVF2F (fh2s, FH2S, 0x15)
  FP_CVF2F (fs2h_rz, FS2H_RZ, 0x1C)

  FP_LOAD (fld16, 0x02, 0)
  FP_LOAD (fld32, 0x00, 0)
  FP_LOAD (fld64, 0x01, 0)
  FP_LOAD (fldd32, 0x00, 1)
  FP_LOAD (fldd64, 0x01, 1)

  FP_STORE (fst16, 0x02, 0)
  FP_STORE (fst32, 0x00, 0)
  FP_STORE (fst64, 0x01, 0)
  FP_STORE (fstd32, 0x00, 1)
  FP_STORE (fstd64, 0x01, 1)

  FP_EXT (vfhext, HALF)
  FP_EXT (vfsext, SINGLE)
  FP_EXT (vfdext, DOUBLE)

  FP_INS (vfhins, HALF)
  FP_INS (vfsins, SINGLE)
  FP_INS (vfdins, DOUBLE)

  FP_REP (vfhrep, HALF)
  FP_REP (vfsrep, SINGLE)
  FP_REP (vfdrep, DOUBLE)

#undef FLAGS_F
#define FLAGS_F { 0 }

#undef FLAGS_CCF
#define FLAGS_CCF { C_CC }

#undef FIELDF
#define FIELDF 0x0

  EXTINSN3OP ("vmin2", ARC_OPCODE_ARC32, MOVE, NONE, F32_GEN4, 0x11)
  EXTINSN3OP ("vmax2", ARC_OPCODE_ARC32, MOVE, NONE, F32_GEN4, 0x0b)

#undef HARD_FIELDF
#define HARD_FIELDF (0x01 << 15)

  EXTINSN3OP ("vmin2", ARC_OPCODE_ARC64, MOVE, NONE, F32_EXT5, 0x38)
  EXTINSN3OP ("vmax2", ARC_OPCODE_ARC64, MOVE, NONE, F32_EXT5, 0x39)

  EXTINSN3OP ("vpack4hl", ARC_OPCODE_ARC64, MOVE, NONE, F32_GEN_OP64, 0x30)
  EXTINSN3OP ("vpack4hm", ARC_OPCODE_ARC64, MOVE, NONE, F32_GEN_OP64, 0x31)
  EXTINSN3OP ("vpack2wl", ARC_OPCODE_ARC64, MOVE, NONE, F32_GEN_OP64, 0x32)
  EXTINSN3OP ("vpack2wm", ARC_OPCODE_ARC64, MOVE, NONE, F32_GEN_OP64, 0x33)
  EXTINSN3OP ("vpack2hm", ARC_OPCODE_ARC32 | ARC_OPCODE_ARC64, MOVE, NONE,
	      F32_EXT5, 0x29)

#undef HARD_FIELDF
#define HARD_FIELDF (0x0)

  OP64INSN3OP ("mpyl", ARC_OPCODE_ARC64, ARITH, NONE, F32_GEN_OP64, 0x30)
  OP64INSN3OP ("mpyml", ARC_OPCODE_ARC64, ARITH, NONE, F32_GEN_OP64, 0x31)
  OP64INSN3OP ("mpymul", ARC_OPCODE_ARC64, ARITH, NONE, F32_GEN_OP64, 0x32)
  OP64INSN3OP ("mpymsul", ARC_OPCODE_ARC64, ARITH, NONE, F32_GEN_OP64, 0x33)

  EXTINSN3OP ("vpack2hl", ARC_OPCODE_ARC32 | ARC_OPCODE_ARC64, MOVE, NONE,
	      F32_EXT5, 0x29)

  { NULL, 0, 0, 0, 0, 0, { 0 }, { 0 } }
};
