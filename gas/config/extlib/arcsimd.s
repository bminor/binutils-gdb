;; arcsimd.s:	SIMD Extension library for ARC.
;; Copyright 2002, 2003 Free Software Foundation
;;
;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;; 
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

.ifdef __Xsimd
	.ifdef __ARC700__
	;; The Vector Registers Vr00-Vr63
	.extCoreRegister vr0 , 0, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr00 , 0, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr1 , 1, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr01 , 1, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr2 , 2, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr02 , 2, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr3 , 3, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr03 , 3, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr4 , 4, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr04 , 4, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr5 , 5, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr05 , 5, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr6 , 6, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr06 , 6, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr7 , 7, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr07 , 7, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr8 , 8, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr08 , 8, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr9 , 9, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr09 , 9, r|w, can_shortcut  , VECTOR
	.extCoreRegister vr10 , 10, r|w, can_shortcut, VECTOR
	.extCoreRegister vr11 , 11, r|w, can_shortcut, VECTOR
	.extCoreRegister vr12 , 12, r|w, can_shortcut, VECTOR
	.extCoreRegister vr13 , 13, r|w, can_shortcut, VECTOR
	.extCoreRegister vr14 , 14, r|w, can_shortcut, VECTOR
	.extCoreRegister vr15 , 15, r|w, can_shortcut, VECTOR
	.extCoreRegister vr16 , 16, r|w, can_shortcut, VECTOR
	.extCoreRegister vr17 , 17, r|w, can_shortcut, VECTOR
	.extCoreRegister vr18 , 18, r|w, can_shortcut, VECTOR
	.extCoreRegister vr19 , 19, r|w, can_shortcut, VECTOR
	.extCoreRegister vr20 , 20, r|w, can_shortcut, VECTOR
	.extCoreRegister vr21 , 21, r|w, can_shortcut, VECTOR
	.extCoreRegister vr22 , 22, r|w, can_shortcut, VECTOR
	.extCoreRegister vr23 , 23, r|w, can_shortcut, VECTOR
	.extCoreRegister vr24 , 24, r|w, can_shortcut, VECTOR
	.extCoreRegister vr25 , 25, r|w, can_shortcut, VECTOR
	.extCoreRegister vr26 , 26, r|w, can_shortcut, VECTOR
	.extCoreRegister vr27 , 27, r|w, can_shortcut, VECTOR
	.extCoreRegister vr28 , 28, r|w, can_shortcut, VECTOR
	.extCoreRegister vr29 , 29, r|w, can_shortcut, VECTOR
	.extCoreRegister vr30 , 30, r|w, can_shortcut, VECTOR
	.extCoreRegister vr31 , 31, r|w, can_shortcut, VECTOR
	.extCoreRegister vr32 , 32, r|w, can_shortcut, VECTOR
	.extCoreRegister vr33 , 33, r|w, can_shortcut, VECTOR
	.extCoreRegister vr34 , 34, r|w, can_shortcut, VECTOR
	.extCoreRegister vr35 , 35, r|w, can_shortcut, VECTOR
	.extCoreRegister vr36 , 36, r|w, can_shortcut, VECTOR
	.extCoreRegister vr37 , 37, r|w, can_shortcut, VECTOR
	.extCoreRegister vr38 , 38, r|w, can_shortcut, VECTOR
	.extCoreRegister vr39 , 39, r|w, can_shortcut, VECTOR
	.extCoreRegister vr40 , 40, r|w, can_shortcut, VECTOR
	.extCoreRegister vr41 , 41, r|w, can_shortcut, VECTOR
	.extCoreRegister vr42 , 42, r|w, can_shortcut, VECTOR
	.extCoreRegister vr43 , 43, r|w, can_shortcut, VECTOR
	.extCoreRegister vr44 , 44, r|w, can_shortcut, VECTOR
	.extCoreRegister vr45 , 45, r|w, can_shortcut, VECTOR
	.extCoreRegister vr46 , 46, r|w, can_shortcut, VECTOR
	.extCoreRegister vr47 , 47, r|w, can_shortcut, VECTOR
	.extCoreRegister vr48 , 48, r|w, can_shortcut, VECTOR
	.extCoreRegister vr49 , 49, r|w, can_shortcut, VECTOR
	.extCoreRegister vr50 , 50, r|w, can_shortcut, VECTOR
	.extCoreRegister vr51 , 51, r|w, can_shortcut, VECTOR
	.extCoreRegister vr52 , 52, r|w, can_shortcut, VECTOR
	.extCoreRegister vr53 , 53, r|w, can_shortcut, VECTOR
	.extCoreRegister vr54 , 54, r|w, can_shortcut, VECTOR
	.extCoreRegister vr55 , 55, r|w, can_shortcut, VECTOR
	.extCoreRegister vr56 , 56, r|w, can_shortcut, VECTOR
	.extCoreRegister vr57 , 57, r|w, can_shortcut, VECTOR
	.extCoreRegister vr58 , 58, r|w, can_shortcut, VECTOR
	.extCoreRegister vr59 , 59, r|w, can_shortcut, VECTOR
	.extCoreRegister vr60 , 60, r|w, can_shortcut, VECTOR
	.extCoreRegister vr61 , 61, r|w, can_shortcut, VECTOR
	.extCoreRegister vr62 , 62, r|w, can_shortcut, VECTOR
	.extCoreRegister vr63 , 63, r|w, can_shortcut, VECTOR
		

	;; The Scalar Registers I0-I7
	.extCoreRegister i0 , 0, r|w, can_shortcut, SCALAR
	.extCoreRegister k0 , 0, r|w, can_shortcut, KSCALAR
	.extCoreRegister i1 , 1, r|w, can_shortcut, SCALAR
	.extCoreRegister k1 , 1, r|w, can_shortcut, KSCALAR
	.extCoreRegister k01 , 1, r|w, can_shortcut, KSCALAR
	.extCoreRegister i01 , 1, r|w, can_shortcut, SCALAR
	.extCoreRegister i2 , 2, r|w, can_shortcut, SCALAR
	.extCoreRegister k2 , 2, r|w, can_shortcut, KSCALAR
	.extCoreRegister k02 , 2, r|w, can_shortcut, KSCALAR
	.extCoreRegister i02 , 2, r|w, can_shortcut, SCALAR
	.extCoreRegister i3 , 3, r|w, can_shortcut, SCALAR
	.extCoreRegister k3 , 3, r|w, can_shortcut, KSCALAR
	.extCoreRegister k03 , 3, r|w, can_shortcut, KSCALAR
	.extCoreRegister i03 , 3, r|w, can_shortcut, SCALAR
	.extCoreRegister i4 , 4, r|w, can_shortcut, SCALAR
	.extCoreRegister k4 , 4, r|w, can_shortcut, KSCALAR
	.extCoreRegister k04 , 4, r|w, can_shortcut, KSCALAR
	.extCoreRegister i04 , 4, r|w, can_shortcut, SCALAR
	.extCoreRegister i5 , 5, r|w, can_shortcut, SCALAR
	.extCoreRegister k5 , 5, r|w, can_shortcut, KSCALAR
	.extCoreRegister k05 , 5, r|w, can_shortcut, KSCALAR
	.extCoreRegister i05 , 5, r|w, can_shortcut, SCALAR
	.extCoreRegister i6 , 6, r|w, can_shortcut, SCALAR
	.extCoreRegister k6 , 6, r|w, can_shortcut, KSCALAR
	.extCoreRegister k06 , 6, r|w, can_shortcut, KSCALAR
	.extCoreRegister i06 , 6, r|w, can_shortcut, SCALAR
	.extCoreRegister i7 , 7, r|w, can_shortcut, SCALAR
	.extCoreRegister k7 , 7, r|w, can_shortcut, KSCALAR
	.extCoreRegister k07 , 7, r|w, can_shortcut, KSCALAR
	.extCoreRegister i07 , 7, r|w, can_shortcut, SCALAR
	.extCoreRegister i8 , 8, r|w, can_shortcut, SCALAR
	.extCoreRegister k8 , 8, r|w, can_shortcut, KSCALAR
	.extCoreRegister k08 , 8, r|w, can_shortcut, KSCALAR
	.extCoreRegister i08 , 8, r|w, can_shortcut, SCALAR
	.extCoreRegister i9 , 9, r|w, can_shortcut, SCALAR
	.extCoreRegister k9 , 9, r|w, can_shortcut, KSCALAR
	.extCoreRegister k09 , 9, r|w, can_shortcut, KSCALAR
	.extCoreRegister i09 , 9, r|w, can_shortcut, SCALAR
	.extCoreRegister i10 , 10, r|w, can_shortcut, SCALAR
	.extCoreRegister k10 , 10, r|w, can_shortcut, KSCALAR
	.extCoreRegister i11 , 11, r|w, can_shortcut, SCALAR
	.extCoreRegister k11 , 11, r|w, can_shortcut, KSCALAR
	.extCoreRegister i12 , 12, r|w, can_shortcut, SCALAR
	.extCoreRegister k12 , 12, r|w, can_shortcut, KSCALAR
	.extCoreRegister i13 , 13, r|w, can_shortcut, SCALAR
	.extCoreRegister k13 , 13, r|w, can_shortcut, KSCALAR
	.extCoreRegister i14 , 14, r|w, can_shortcut, SCALAR
	.extCoreRegister k14 , 14, r|w, can_shortcut, KSCALAR
	.extCoreRegister i15 , 15, r|w, can_shortcut, SCALAR
	.extCoreRegister k15 , 15, r|w, can_shortcut, KSCALAR

	;;  The DMA registers Dr0-Dr63
	.extCoreRegister dr0 , 0, r|w, can_shortcut  , DMA
	.extCoreRegister dr00 , 0, r|w, can_shortcut  , DMA
	.extCoreRegister dr1 , 1, r|w, can_shortcut  , DMA
	.extCoreRegister dr01 , 1, r|w, can_shortcut  , DMA
	.extCoreRegister dr2 , 2, r|w, can_shortcut  , DMA
	.extCoreRegister dr02 , 2, r|w, can_shortcut  , DMA
	.extCoreRegister dr3 , 3, r|w, can_shortcut  , DMA
	.extCoreRegister dr03 , 3, r|w, can_shortcut  , DMA
	.extCoreRegister dr4 , 4, r|w, can_shortcut  , DMA
	.extCoreRegister dr04 , 4, r|w, can_shortcut  , DMA
	.extCoreRegister dr5 , 5, r|w, can_shortcut  , DMA
	.extCoreRegister dr05 , 5, r|w, can_shortcut  , DMA
	.extCoreRegister dr6 , 6, r|w, can_shortcut  , DMA
	.extCoreRegister dr06 , 6, r|w, can_shortcut  , DMA
	.extCoreRegister dr7 , 7, r|w, can_shortcut  , DMA
	.extCoreRegister dr07 , 7, r|w, can_shortcut  , DMA
	.extCoreRegister dr8 , 8, r|w, can_shortcut  , DMA
	.extCoreRegister dr08 , 8, r|w, can_shortcut  , DMA
	.extCoreRegister dr9 , 9, r|w, can_shortcut  , DMA
	.extCoreRegister dr09 , 9, r|w, can_shortcut  , DMA
	.extCoreRegister dr10 , 10, r|w, can_shortcut, DMA
	.extCoreRegister dr11 , 11, r|w, can_shortcut, DMA
	.extCoreRegister dr12 , 12, r|w, can_shortcut, DMA
	.extCoreRegister dr13 , 13, r|w, can_shortcut, DMA
	.extCoreRegister dr14 , 14, r|w, can_shortcut, DMA
	.extCoreRegister dr15 , 15, r|w, can_shortcut, DMA
	.extCoreRegister dr16 , 16, r|w, can_shortcut, DMA
	.extCoreRegister dr17 , 17, r|w, can_shortcut, DMA
	.extCoreRegister dr18 , 18, r|w, can_shortcut, DMA
	.extCoreRegister dr19 , 19, r|w, can_shortcut, DMA
	.extCoreRegister dr20 , 20, r|w, can_shortcut, DMA
	.extCoreRegister dr21 , 21, r|w, can_shortcut, DMA
	.extCoreRegister dr22 , 22, r|w, can_shortcut, DMA
	.extCoreRegister dr23 , 23, r|w, can_shortcut, DMA
	.extCoreRegister dr24 , 24, r|w, can_shortcut, DMA
	.extCoreRegister dr25 , 25, r|w, can_shortcut, DMA
	.extCoreRegister dr26 , 26, r|w, can_shortcut, DMA
	.extCoreRegister dr27 , 27, r|w, can_shortcut, DMA
	.extCoreRegister dr28 , 28, r|w, can_shortcut, DMA
	.extCoreRegister dr29 , 29, r|w, can_shortcut, DMA
	.extCoreRegister dr30 , 30, r|w, can_shortcut, DMA
	.extCoreRegister dr31 , 31, r|w, can_shortcut, DMA
	.extCoreRegister dr32 , 32, r|w, can_shortcut, DMA
	.extCoreRegister dr33 , 33, r|w, can_shortcut, DMA
	.extCoreRegister dr34 , 34, r|w, can_shortcut, DMA
	.extCoreRegister dr35 , 35, r|w, can_shortcut, DMA
	.extCoreRegister dr36 , 36, r|w, can_shortcut, DMA
	.extCoreRegister dr37 , 37, r|w, can_shortcut, DMA
	.extCoreRegister dr38 , 38, r|w, can_shortcut, DMA
	.extCoreRegister dr39 , 39, r|w, can_shortcut, DMA
	.extCoreRegister dr40 , 40, r|w, can_shortcut, DMA
	.extCoreRegister dr41 , 41, r|w, can_shortcut, DMA
	.extCoreRegister dr42 , 42, r|w, can_shortcut, DMA
	.extCoreRegister dr43 , 43, r|w, can_shortcut, DMA
	.extCoreRegister dr44 , 44, r|w, can_shortcut, DMA
	.extCoreRegister dr45 , 45, r|w, can_shortcut, DMA
	.extCoreRegister dr46 , 46, r|w, can_shortcut, DMA
	.extCoreRegister dr47 , 47, r|w, can_shortcut, DMA
	.extCoreRegister dr48 , 48, r|w, can_shortcut, DMA
	.extCoreRegister dr49 , 49, r|w, can_shortcut, DMA
	.extCoreRegister dr50 , 50, r|w, can_shortcut, DMA
	.extCoreRegister dr51 , 51, r|w, can_shortcut, DMA
	.extCoreRegister dr52 , 52, r|w, can_shortcut, DMA
	.extCoreRegister dr53 , 53, r|w, can_shortcut, DMA
	.extCoreRegister dr54 , 54, r|w, can_shortcut, DMA
	.extCoreRegister dr55 , 55, r|w, can_shortcut, DMA
	.extCoreRegister dr56 , 56, r|w, can_shortcut, DMA
	.extCoreRegister dr57 , 57, r|w, can_shortcut, DMA
	.extCoreRegister dr58 , 58, r|w, can_shortcut, DMA
	.extCoreRegister dr59 , 59, r|w, can_shortcut, DMA
	.extCoreRegister dr60 , 60, r|w, can_shortcut, DMA
	.extCoreRegister dr61 , 61, r|w, can_shortcut, DMA
	.extCoreRegister dr62 , 62, r|w, can_shortcut, DMA
	.extCoreRegister dr63 , 63, r|w, can_shortcut, DMA

	.extAuxRegister	vlc_build,     0xfc,r|w ; VLC extensions (all
						; entropy decoder blocks)
	.extAuxRegister	simd_dma_build,0xfd,r|w ; Aurora SIMD/DMA

	.extAuxRegister	se_ctrl,  0xa1, r|w	; SIMD Engine Control Register
	.extAuxRegister	se_stat,  0xa2, r	; SIMD Engine Status Register
	.extAuxRegister	se_err,	  0xa3, r	; SIMD Engine Error Register
	.extAuxRegister	se_eadr,  0xa4, r	; SIMD Engine Error Addr Reg
	.extAuxRegister	se_spc,   0xa5, r	; SIMD Engine program counter
	.extAuxRegister	sdm_base, 0xa6, r|w	; Base addr of SDM memory
	.extAuxRegister	scm_base, 0xa7, r|w	; Base addr of SCM memory
	
	;;  The SIMD Instructions

	;; FLAG2_SET means bit 22 is set
	;; FLAG1_SET means bit 23 is set
	;; FLAG_SET means bit 15 is set		
        ;; EXTENDED  64 bit format
        ;; EXTEND2 64 bit format with A operand used for addition suboperation
        ;;         second sub-op is bits 8-15 of sub-opcode
        ;; EXTEND3 64 bit format with A and B operands used for suboperation
        ;;         third sub-op is bits 16-23 of sub-opcode
        ;; On all 64 bit instruction bits 24-25 are arithmetic mode

	; Note:	Normally, for 3-operand instructions, the sub-opcode value given in
	; the extension instruction directive refers to the 6-bit opcode field
	; in bits [21:16]. But note that for ENCODE_U8 instructions, the sub-op
	; value given in the directive refers to the 5-bit I5 opcode field in
	; bits [21:17].
	; For non-u8 SIMD instructions, the 6-bit sub-opcode is extended into
	; bit 15 for a total of a 7-bit opcode field.  We currently don't have
	; a special encoding flag to indicate this but it can be done by treat-
	; ing the sub-opcode field as the traditional 6-bit value and specifying
	; the FLAG_SET instruction flag if the flag bit (bit 15) needs to be
	; set to complete the encoding of the instruction.

        ;;  following five need proper numbers
 	.extInstruction	chop,     0x05, 0x3f2f, FLAG2_SET|EXTEND1, SYNTAX_0
 	.extInstruction	chex,     0x05, 0x13f2f,FLAG2_SET|EXTEND1, SYNTAX_0
	.extInstruction vexit,    0x0a, 0x23f2f, SUFFIX_MASK|EXTEND3|FLAG2_SET|ENCODE_SETLM,SYNTAX_0
	.extInstruction vexite,    0x0a, 0x23f2f, SUFFIX_MASK|EXTEND3|FLAG2_SET|ENCODE_SETLM,SYNTAX_0
        .extInstruction	scq,      0x09, 0x037182f, EXTENDED|EXTEND2|EXTEND3, SYNTAX_DISC|SYNTAX_C0|SYNTAX_CC
        .extInstruction	scq,      0x09, 0x0182f, EXTENDED|EXTEND2, SYNTAX_C00|SYNTAX_VVC
        .extInstruction vjd,      0x0a,0xa2f,EXTENDED|EXTEND2|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_CC
        .extInstruction vjde,      0x0a,0xa2f,EXTENDED|EXTEND2|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_CC
	.extInstruction vjp,      0x0a,0x003f2f,EXTEND3|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_C|SYNTAX_0
	.extInstruction vjpe,      0x0a,0x003f2f,EXTEND3|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_C|SYNTAX_0
	.extInstruction vjp,      0x0a,0x013f2f,EXTEND3|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_0|SYNTAX_VVI
	.extInstruction vjpe,      0x0a,0x013f2f,EXTEND3|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_0|SYNTAX_VVI
	.extInstruction vjpt,      0x0a,0x0002f,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTEND2|ENCODE_SETLM,        SYNTAX_V00|SYNTAX_IREGA
	.extInstruction vjpte,      0x0a,0x0002f,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTEND2|ENCODE_SETLM,        SYNTAX_V00|SYNTAX_IREGA
	.extInstruction vim,      0x0a,0x16,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA|SYNTAX_IREGB
	.extInstruction vime,      0x0a,0x16,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA|SYNTAX_IREGB
	.extInstruction vkm,      0x0a,0x1a,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|ENCODE_SETLM|ENCODE_KREG,SYNTAX_V00|SYNTAX_IREGA|SYNTAX_IREGB
	.extInstruction vkme,      0x0a,0x1a,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|ENCODE_SETLM|ENCODE_KREG,SYNTAX_V00|SYNTAX_IREGA|SYNTAX_IREGB
	.extInstruction vsnew,    0x0a,0x2d2f,SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0
	.extInstruction vsnewe,    0x0a,0x2d2f,SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0
	.extInstruction vsnew,    0x0a,0x2d2f,SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vsnew,    0x0a,0x2d2f,SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsnewe,    0x0a,0x2d2f,SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vsnewe,    0x0a,0x2d2f,SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsnew,    0x0a,0x2d2f,ENCODE_U6|SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0
	.extInstruction vsnewe,    0x0a,0x2d2f,ENCODE_U6|SUFFIX_MASK|EXTENDED|EXTEND2|FLAG_SET,SYNTAX_VV0
	.extInstruction vtnone,    0x0a,0x42f,EXTEND2|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U16,SYNTAX_C00|SYNTAX_IREGB
	.extInstruction vtnonei,    0x0a,0x42f,EXTEND2|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U16,SYNTAX_C00|SYNTAX_IREGB
	.extInstruction vtnone,    0x0a,0x292f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_VVI
	.extInstruction vtanyi,     0x0a,0x22f,FLAG1_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK|ENCODE_U16,SYNTAX_C00|SYNTAX_IREGB
	.extInstruction vtany,     0x0a,0x22f,FLAG1_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK|ENCODE_U16,SYNTAX_C00|SYNTAX_IREGB
	.extInstruction vtany,    0x0a,0x272f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_VVI
       	.extInstruction vtalli,    0x0a,0x02f,FLAG1_SET|ENCODE_U16|SUFFIX_COND|SUFFIX_MASK|EXTEND2,SYNTAX_C00|SYNTAX_IREGB
       	.extInstruction vtall,    0x0a,0x02f,FLAG1_SET|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_C00|SYNTAX_IREGB
       	.extInstruction vtall,    0x0a,0x252f,SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_C00|SYNTAX_IREGB|SYNTAX_VVI
        .extInstruction vld8w,    0x0a,0x18,FLAG1_SET|SUFFIX_MASK|EXTENDED|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_1,    0x0a,0x1c,FLAG1_SET|SUFFIX_MASK|EXTENDED|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_2,    0x0a,0x1e,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_3,    0x0a,0x20,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_4,    0x0a,0x22,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_5,    0x0a,0x24,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_6,    0x0a,0x26,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld8w_7,    0x0a,0x28,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vrget,      0x0a,0x3d,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vrput,      0x0a,0x3e,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld16,    0x0a,0x17,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld16e,    0x0a,0x17,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld16_0,  0x0a,0x17,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld16_1,  0x0a,0x1b,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld16_2,  0x0a,0x1d,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld16_3,  0x0a,0x1f,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld16_4,  0x0a,0x21,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld16_5,  0x0a,0x23,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld16_6,  0x0a,0x25,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld16_7,  0x0a,0x27,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32,	  0x0a,0x00,ENCODE_S12|EXTENDED|SUFFIX_MASK|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32e,	  0x0a,0x00,ENCODE_S12|EXTENDED|SUFFIX_MASK|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32,	  0x09,0x04,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vld32wl,  0x0a,0x03,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32wle,  0x0a,0x03,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32wl,  0x09,0x06,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vld32wh,  0x0a,0x04,ENCODE_S12|EXTENDED|SUFFIX_MASK|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32whe,  0x0a,0x04,ENCODE_S12|EXTENDED|SUFFIX_MASK|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_0,  0x09,0x04,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vld32_2,  0x0a,0x29,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_2e,  0x0a,0x29,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_2,  0x09,0x29,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vld32_4,  0x0a,0x2a,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_4e,  0x0a,0x2a,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_4,  0x09,0x2a,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vld32_6,  0x0a,0x2b,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_6e,  0x0a,0x2b,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32_6,  0x09,0x2b,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vld64,	  0x0a,0x01,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_3|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld64e,	  0x0a,0x01,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_3|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld64,	  0x09,0x08,ENCODE_U8|SCALE_3,SYNTAX_VbI0
	.extInstruction vld64w,	  0x0a,0x02,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_3|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld64we,	  0x0a,0x02,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_3|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld64w,	  0x09,0x0a,ENCODE_U8|SCALE_3,SYNTAX_VbI0
	.extInstruction vld32wle,  0x0a,0x03,ENCODE_S12|EXTENDED|FLAG1_SET|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld32whe,  0x0a,0x04,ENCODE_S12|FLAG1_SET|EXTENDED|ENCODE_SETLM|SCALE_2,SYNTAX_VbI0
	.extInstruction vld32wh,  0x09,0x02,ENCODE_U8|SCALE_2,SYNTAX_VbI0
        .extInstruction vld128,   0x0a,0x5,EXTENDED|FLAG1_SET|SUFFIX_MASK|ENCODE_S12|SCALE_4|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vld128,   0x09,0xc,ENCODE_U8|FLAG1_SET|SCALE_4,SYNTAX_VbI0
        .extInstruction vld128e,   0x0a,0x5,EXTENDED|FLAG1_SET|SUFFIX_MASK|ENCODE_S12|SCALE_4|ENCODE_SETLM,SYNTAX_VbI0
        .extInstruction vld128r,  0x0a,0x6,SCALE_0|EXTENDED|FLAG1_SET|SUFFIX_MASK|ENCODE_SETLM|ENCODE_S12,SYNTAX_VbC0|SYNTAX_Vb00
        .extInstruction vld128re,  0x0a,0x6,SCALE_0|EXTENDED|FLAG1_SET|SUFFIX_MASK|ENCODE_SETLM|ENCODE_S12,SYNTAX_VbC0|SYNTAX_Vb00
	.extInstruction vld128r,  0x09,0x1c,SCALE_0|FLAG1_SET,SYNTAX_VbC0|SYNTAX_Vb00
	.extInstruction vst8,	  0x0a,0x2c,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_0,	  0x0a,0x2c,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_1,	  0x00a,0x2d,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_2,	  0x00a,0x2e,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_3,	  0x00a,0x30,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_4,	  0x00a,0x31,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_5,	  0x00a,0x32,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_6,	  0x00a,0x33,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst8_7,	  0x00a,0x34,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_0|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16,	  0x0a,0x07,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_0,  0x0a,0x07,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16e,	  0x0a,0x07,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16,	  0x09,0x07,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	
	.extInstruction vst16_0,  0x09,0x07,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_1,  0x0a,0x000000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_1e,  0x0a,0x000000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_1,  0x09,0x11,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_2,  0x0a,0x000000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_2e,  0x0a,0x000000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_2,  0x09,0x03,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_3,  0x0a,0x000000e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_3e,  0x0a,0x000000e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_3,  0x09,0x0b,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_4,  0x0a,0x000000f,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_4e,  0x0a,0x000000f,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_4,  0x09,0x13,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_5,  0x0a,0x0000010,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_5e,  0x0a,0x0000010,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_5,  0x09,0x19,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_6,  0x0a,0x0000011,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_6e,  0x0a,0x0000011,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_6,  0x09,0x1b,ENCODE_U8|SCALE_1,SYNTAX_VbI0
	.extInstruction vst16_7,  0x0a,0x0000012,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_7e,  0x0a,0x0000012,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_1|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst16_7,  0x09,0x1d,ENCODE_U8|SCALE_1,SYNTAX_VbI0

	.extInstruction vst32,	  0x0a,0x08,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32e,	  0x0a,0x08,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32,	  0x09,0x05,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vst32_0,  0x09,0x05,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vst32_2,  0x0a,0x0000013,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32_2e,  0x0a,0x0000013,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32_2,  0x09,0x12,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vst32_4,  0x0a,0x0000014,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32_4e,  0x0a,0x0000014,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32_4,  0x09,0x16,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vst32_6,  0x0a,0x0000015,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32_6e,  0x0a,0x0000015,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|ENCODE_S12|SCALE_2|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst32_6,  0x09,0x18,ENCODE_U8|SCALE_2,SYNTAX_VbI0
	.extInstruction vst64,	  0x0a,0x09,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_3|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst64e,	  0x0a,0x09,EXTENDED|SUFFIX_MASK|FLAG1_SET|ENCODE_S12|SCALE_3|ENCODE_SETLM,SYNTAX_VbI0
	.extInstruction vst64,	  0x09,0x09,ENCODE_U8|SCALE_3,SYNTAX_VbI0
	.extInstruction vst128,  0x0a,0x0a,FLAG1_SET|EXTENDED|SUFFIX_MASK|SCALE_4|ENCODE_S12|ENCODE_SETLM, SYNTAX_Vb00
	.extInstruction vst128e,  0x0a,0x0a,FLAG1_SET|EXTENDED|SUFFIX_MASK|SCALE_4|ENCODE_S12|ENCODE_SETLM, SYNTAX_Vb00
	.extInstruction vst128,	  0x09,0x0d,FLAG1_SET|ENCODE_U8|SCALE_4,SYNTAX_VbI0
	.extInstruction vst128r,  0x0a,0x05,EXTENDED|ENCODE_S12|FLAG1_SET|SCALE_4|ENCODE_SETLM,SYNTAX_VbI0|SYNTAX_VbC0
	.extInstruction vst128re,  0x0a,0x05,EXTENDED|ENCODE_S12|FLAG1_SET|SCALE_4|ENCODE_SETLM,SYNTAX_VbI0|SYNTAX_VbC0
	.extInstruction vst128r,  0x09,0x1e,SCALE_4,SYNTAX_Vb00|SYNTAX_VbC0
	.extInstruction vmvzw,	  0x09,0x14,ENCODE_U8|FLAG1_SET,  SYNTAX_VV0

	.extInstruction vmvw,     0x0a,0x262f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction vmvw,     0x0a,0x262f,ENCODE_ZEROB|EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0
       	.extInstruction vmvw,	  0x09,0x10,ENCODE_U8|FLAG1_SET,  SYNTAX_VV0
	.extInstruction vmvwe,    0x0a,0x262f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction vmvwe,    0x0a,0x262f,ENCODE_ZEROB|EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction vmovwe,     0x0a,0x282f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmovwe,     0x0a,0x282f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV|SYNTAX_VVL
	.extInstruction vmovw,     0x0a,0x282f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmovw,     0x0a,0x282f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV|SYNTAX_VVL
	.extInstruction vmovw,	  0x09,0x34,FLAG1_SET,	SYNTAX_V00|SYNTAX_VC0
	.extInstruction vmovw,	  0x09,0x34,FLAG1_SET,	SYNTAX_V00|SYNTAX_VL0
	.extInstruction vmovw,	  0x09,0x1a,FLAG1_SET|ENCODE_U8|ENCODE_ZEROB,	SYNTAX_V00|SYNTAX_VL0
	.extInstruction vmovw,	  0x0a,0x3d,EXTENDED|FLAG1_SET,SYNTAX_V00|SYNTAX_VC0|SYNTAX_VVC
	.extInstruction vmovw,	  0x0a,0x3d,EXTENDED|FLAG1_SET,SYNTAX_V00|SYNTAX_VL0|SYNTAX_VVC
	.extInstruction vmovwe,	  0x0a,0x3d,EXTENDED|FLAG1_SET,SYNTAX_V00|SYNTAX_VC0|SYNTAX_VVC
	.extInstruction vmovw,	  0x0a,0x3d,EXTENDED|FLAG1_SET,SYNTAX_V00|SYNTAX_VVC|SYNTAX_VL0
	.extInstruction vmovwe,	  0x0a,0x3d,EXTENDED|FLAG1_SET,SYNTAX_V00|SYNTAX_VVC|SYNTAX_VL0
	.extInstruction vmovw,    0x0a,0x1f,EXTENDED|ENCODE_U8,SYNTAX_VC0|SYNTAX_V00
	.extInstruction vmovwe,    0x0a,0x1f,EXTENDED|ENCODE_U8,SYNTAX_VC0|SYNTAX_V00
	.extInstruction vmovwe,    0x0a,0x1f,EXTENDED|ENCODE_U8,SYNTAX_VL0|SYNTAX_V00
	.extInstruction vmovw,	  0x09,0x1a,ENCODE_U8|FLAG1_SET,SYNTAX_V00|SYNTAX_VC0
	.extInstruction vmovw,	  0x09,0x1a,ENCODE_U8|FLAG1_SET,SYNTAX_V00|SYNTAX_VL0
	
	.extInstruction vmovzw,	  0x09,0x1e,ENCODE_U8|FLAG1_SET,				    SYNTAX_V00|SYNTAX_VC0
	.extInstruction vmovzw,	  0x09,0x1e,ENCODE_U8|FLAG1_SET,				    SYNTAX_V00|SYNTAX_VL0
	.extInstruction vmvaw,	  0x09,0x15,ENCODE_U8|FLAG1_SET,  SYNTAX_VV0
	.extInstruction vmovaw,	  0x09,0x1f,ENCODE_U8|FLAG1_SET, SYNTAX_VC0|SYNTAX_V00
	.extInstruction vmovaw,	  0x09,0x1f,ENCODE_U8|FLAG1_SET, SYNTAX_VL0|SYNTAX_V00
       	.extInstruction vmov, 0x0a,0x2a2f,EXTEND2|SUFFIX_MASK|ENCODE_LIMM,SYNTAX_V00
       	.extInstruction vmove, 0x0a,0x2a2f,EXTEND2|SUFFIX_MASK|ENCODE_LIMM,SYNTAX_V00
	.extInstruction vmov, 0x0a,0x2b2f,EXTEND2|SUFFIX_MASK,SYNTAX_V00|SYNTAX_VC0
	.extInstruction vmov, 0x0a,0x2b2f,EXTEND2|SUFFIX_MASK,SYNTAX_V00|SYNTAX_VL0
	.extInstruction vmove, 0x0a,0x2b2f,EXTEND2|SUFFIX_MASK,SYNTAX_V00|SYNTAX_VC0
	.extInstruction vmove, 0x0a,0x2b2f,EXTEND2|SUFFIX_MASK,SYNTAX_V00|SYNTAX_VL0
	.extInstruction vd6tapf,  0x09,0x1c,ENCODE_U8|FLAG1_SET,  SYNTAX_VV0
	.extInstruction viaddw, 0x0a,0x1000003,EXTENDED|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddw, 0x0a,0x1000003,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddw, 0x0a,0x1000003,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddwe, 0x0a,0x1000003,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddwe, 0x0a,0x1000003,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vaddw,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vaddw,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddw,    0x0a,0x1000004,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vaddwe,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vaddwe,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddwe,    0x0a,0x1000004,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbaddwe,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbaddwe,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbaddwe,    0x0a,0x1000004,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbaddw,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbaddw,    0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbaddw,    0x0a,0x1000004,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vaddw,    0x0a,0x1000000,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddw,    0x0a,0x1000000,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddwe,    0x0a,0x1000000,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddwe,    0x0a,0x1000000,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddw,    0x0a,0x1000000,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddw,    0x0a,0x1000000,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddwe,    0x0a,0x1000000,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddwe,    0x0a,0x1000000,ENCODE_ZEROA|EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddw,    0x09,0x14,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction vemulf,   0x0a,0x1000031,FLAG1_SET|FLAG2_SET|FLAG_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
        .extInstruction vadd,     0x0a,0x1000020,SUFFIX_MASK|SUFFIX_COND|EXTENDED|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vadde,     0x0a,0x1000020,SUFFIX_MASK|SUFFIX_COND|EXTENDED|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vadde,     0x0a,0x1000020,ENCODE_ZEROA|SUFFIX_MASK|SUFFIX_COND|EXTENDED|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vadd,     0x0a,0x1000020,ENCODE_ZEROA|SUFFIX_MASK|SUFFIX_COND|EXTENDED|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vadd,     0x0a,0x1000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vadd,     0x0a,0x1000004,ENCODE_ZEROA|SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vadde,     0x0a,0x1000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vadde,     0x0a,0x1000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vadd,     0x09,0x0,FLAGS_NONE,  SYNTAX_VVV
        .extInstruction vadda,     0x0a,0x2000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vadda,     0x0a,0x2000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vaddae,     0x0a,0x2000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vaddae,     0x0a,0x2000004,SUFFIX_MASK|SUFFIX_COND|EXTENDED|FLAG2_SET|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG,  SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vadda,     0x0a,0x2000003,FLAG1_SET|FLAG2_SET|EXTENDED,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddae,     0x0a,0x2000003,FLAG1_SET|FLAG2_SET|EXTENDED,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vadda,     0x0a,0x2000020,SUFFIX_MASK|SUFFIX_COND|EXTENDED|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vadda,     0x0a,0x2000020,SUFFIX_MASK|SUFFIX_COND|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vaddae,     0x0a,0x2000020,SUFFIX_MASK|SUFFIX_COND|EXTENDED|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vaddae,     0x0a,0x2000020,SUFFIX_MASK|SUFFIX_COND|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND,  SYNTAX_VVV
        .extInstruction vadda,     0x09,0,FLAG_SET,SYNTAX_VVV	
        .extInstruction vsubnaw,     0x0a,0x000001,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubnaw,     0x0a,0x000001,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubnawe,     0x0a,0x000001,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubnawe,     0x0a,0x000001,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubna,     0x0a,0x000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubna,     0x0a,0x000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubnae,     0x0a,0x000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubnae,     0x0a,0x000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsub,     0x0a,0x1000005,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsub,     0x0a,0x1000005,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsube,     0x0a,0x1000005,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsube,     0x0a,0x1000005,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsube,     0x0a,0x1000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsube,     0x0a,0x1000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsub,     0x0a,0x1000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsub,     0x0a,0x1000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsub,     0x09,0x02,FLAGS_NONE, SYNTAX_VVV
	
        .extInstruction vsuba,    0x0a,0x2000006,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsuba,    0x0a,0x2000006,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsubae,    0x0a,0x2000006,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsubae,    0x0a,0x2000006,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vsuba,    0x0a,0x2000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsuba,    0x0a,0x2000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubae,    0x0a,0x2000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|SUFFIX_FLAG, SYNTAX_VVV
        .extInstruction vsubae,    0x0a,0x2000021,SUFFIX_COND|SUFFIX_MASK|EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VVV
	.extInstruction vsuba,    0x09,0x02,FLAG_SET,SYNTAX_VVV
	.extInstruction vjl,      0x0a,0x12f,EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_V00|SYNTAX_VC0
	.extInstruction vjl,      0x0a,0x12f,EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_V00|SYNTAX_VL0
	.extInstruction vjl,      0x0a,0x117,EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_U8|ENCODE_LIMM|ENCODE_SETLM,SYNTAX_VC0|SYNTAX_V00
	.extInstruction vjli,      0x0a,0x22f,EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_U6|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA
	.extInstruction vjl,      0x0a,0x22f,EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_U6|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA
	.extInstruction vjb,      0x0a,0x32f,EXTENDED|EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_VC0|SYNTAX_V00
	.extInstruction vjb,      0x0a,0x32f,EXTENDED|EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_VL0|SYNTAX_V00
	.extInstruction vjb,      0x0a,0x42f,EXTENDED|EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA
	.extInstruction vjb,      0x0a,0x417,EXTENDED|EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_U8|ENCODE_LIMM,SYNTAX_V00
	.extInstruction vjbi,      0x0a,0x42f,EXTENDED|SUFFIX_MASK|EXTEND2|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA
	.extInstruction via,       0x0a,0x19,EXTENDED|SUFFIX_COND|SUFFIX_MASK|FLAG1_SET|ENCODE_U16|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA|SYNTAX_IREGB
	.extInstruction	vsubw,    0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vsubw,    0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vsubw,    0x0a,0x1000005,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vsubwe,    0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vsubwe,    0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vsubwe,    0x0a,0x1000005,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbsubwe,    0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbsubwe,    0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbsubwe,    0x0a,0x1000005,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vsubw,    0x0a,0x1000001,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vsubw,    0x0a,0x1000001,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vsubwe,    0x0a,0x1000001,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vsubwe,    0x0a,0x1000001,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vsubw,    0x09,0x16,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmulw,    0x0a,0x1000002,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VVV
	.extInstruction	vmulwe,    0x0a,0x1000002,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VVV
	.extInstruction	vmulw,     0x0a,0x1000007,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulw,     0x0a,0x1000007,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vmulwe,     0x0a,0x1000007,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulwe,     0x0a,0x1000007,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL        
	.extInstruction	vmulw,     0x0a,0x1000009,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vimulw,     0x0a,0x1000009,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmulwe,     0x0a,0x1000009,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
       	.extInstruction	vmulw,     0x0a,0x1000022,EXTENDED|ENCODE_LIMM|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
       	.extInstruction	vmulwe,     0x0a,0x1000022,EXTENDED|ENCODE_LIMM|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vmulw,    0x09,0x18,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmulw,    0x09,0x19,FLAGS_NONE,	SYNTAX_VV0|SYNTAX_VVC

       	.extInstruction	vmul,     0x0a,0x1000022,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0
       	.extInstruction	vmule,     0x0a,0x1000022,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0
       	.extInstruction	vmul,     0x0a,0x1000022,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vmule,     0x0a,0x1000022,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction vmul,     0x09,0x4,FLAGS_NONE,SYNTAX_VVV
	.extInstruction	vmula,     0x0a,0x2000022,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmulae,     0x0a,0x2000022,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmula,     0x0a,0x2000023,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmula,     0x0a,0x2000023,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vmulae,     0x0a,0x2000023,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulae,     0x0a,0x2000023,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
       	.extInstruction vmula,     0x09,4,FLAG_SET,SYNTAX_VVV
	.extInstruction	vmulfw,    0x0a,0x100000b,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vimulfw,    0x0a,0x100000b,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmulfwe,    0x0a,0x100000b,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmulfw,    0x0a,0x1000008,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulfw,    0x0a,0x1000008,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbmulfw,    0x0a,0x1000008,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbmulfw,    0x0a,0x1000008,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbmulfwe,    0x0a,0x1000008,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbmulfwe,    0x0a,0x1000008,SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vmulfw,    0x0a,0x1000003,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VVV
	.extInstruction	vmulfw,    0x0a,0x1000003,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VVV
	.extInstruction	vmulfwe,    0x0a,0x1000003,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VVV
	.extInstruction	vmulfwe,    0x0a,0x1000003,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,	SYNTAX_VVV
	.extInstruction	vmulfw,   0x09,0x1a,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmulfa,   0x0a,0x2000024,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmulfae,   0x0a,0x2000024,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vbaddw,   0x09,0x15,ENCODE_LIMM|ENCODE_ZEROC,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbaddw,   0x09,0x15,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbsubw,   0x09,0x17,ENCODE_LIMM|ENCODE_ZEROC,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbsubw,   0x09,0x17,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction vbmulaw, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulaw, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbmulw,   0x0a,0x1000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulw,   0x0a,0x1000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbmulwe,   0x0a,0x1000007,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulwe,   0x0a,0x1000007,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbmulw,   0x09,0x19,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulw,   0x09,0x19,ENCODE_LIMM,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbmulwa,  0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbmulwa,  0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulwa,  0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vbmulwae,  0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulwae,  0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction vaddaw, 0x09,0x0000014,FLAG_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vaddaw, 0x0a,0x2000003,SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddaw, 0x0a,0x2000003,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddawe, 0x0a,0x2000003,SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddawe, 0x0a,0x2000003,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viaddaw, 0x0a,0x2000003,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viaddaw, 0x0a,0x2000003,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vaddaw, 0x09,0x2000016,FLAG_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vaddaw, 0x09,0x2000016,FLAG_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vaddaw,    0x0a,0x2000000,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddaw,    0x0a,0x2000000,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddawe,    0x0a,0x2000000,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddawe,    0x0a,0x2000000,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vaddaw,   0x09,0x14,FLAG_SET,   SYNTAX_VVV
	
	.extInstruction visubaw, 0x0a,0x2000005,SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction visubaw, 0x0a,0x2000005,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubaw, 0x0a,0x2000005,SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubaw, 0x0a,0x2000005,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubawe, 0x0a,0x2000005,SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubawe, 0x0a,0x2000005,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vsubaw,   0x0a,0x2000001,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,   SYNTAX_VVV
	.extInstruction	vsubaw,   0x0a,0x2000001,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,   SYNTAX_VVV
	.extInstruction	vsubawe,   0x0a,0x2000001,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,   SYNTAX_VVV
	.extInstruction	vsubawe,   0x0a,0x2000001,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,   SYNTAX_VVV
	.extInstruction	vsubaw,   0x09,0x16,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vaddsuw,  0x0a,0x100002f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV
	.extInstruction	vaddsuwe,  0x0a,0x100002f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV
	.extInstruction	vaddsuw,  0x09,0x002f,EXTEND1,	SYNTAX_VV
	.extInstruction	vaddsu,   0x0a,0x100082f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vaddsu,   0x0a,0x100082f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vaddsue,   0x0a,0x100082f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vaddsue,   0x0a,0x100082f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsw,    0x0a,0x100012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsw,    0x0a,0x100012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabswe,    0x0a,0x100012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabswe,    0x0a,0x100012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction vabsw,    0x09,0x12f,EXTEND1,SYNTAX_VV
	.extInstruction	vabs,     0x0a,0x100092f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabs,     0x0a,0x100092f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabse,     0x0a,0x100092f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabse,     0x0a,0x100092f,EXTENDED|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsaw,   0x0a,0x200012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsaw,   0x0a,0x200012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsawe,   0x0a,0x200012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsawe,   0x0a,0x200012f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vabsaw,   0x09,0x12f,EXTEND1|FLAG_SET,   SYNTAX_VV
	.extInstruction	vasrw,    0x0a,0x1000009,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vasrw,    0x0a,0x1000009,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vasrwe,    0x0a,0x1000009,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vasrwe,    0x0a,0x1000009,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vasrw,   0x0a,0x1000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6 |FLAG2_SET,  SYNTAX_VV0 ; added
	.extInstruction vasrwe,   0x0a,0x1000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6 |FLAG2_SET,  SYNTAX_VV0 ; added
	.extInstruction	vasrw,    0x0a,0x1000009,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vasrw,    0x0a,0x1000009,EXTENDED|FLAG2_SET|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vasrwe,    0x0a,0x1000009,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vasrwe,    0x0a,0x1000009,EXTENDED|FLAG2_SET|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vasrw,    0x09,0x20,ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction	vasrw,    0x09,0x20,FLAGS_NONE,	SYNTAX_VVI|SYNTAX_VV0
	.extInstruction vasrwi,   0x0a,0x1000009,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,  SYNTAX_VV0 
	.extInstruction vasrwi,   0x0a,0x1000009,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,  SYNTAX_VV0 
	.extInstruction vasrwi,   0x09,0x20,ENCODE_U6 |FLAG2_SET,  SYNTAX_VV0 ; added
	.extInstruction vasrwie,   0x0a,0x1000009,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,  SYNTAX_VV0
	.extInstruction vasrwie,   0x0a,0x1000009,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,  SYNTAX_VV0
 	.extInstruction	vasrrw,  0x0a,0x1000035,EXTENDED|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrw,  0x0a,0x1000035,EXTENDED|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrwe,  0x0a,0x1000035,EXTENDED|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrwe,  0x0a,0x1000035,EXTENDED|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrwie,  0x0a,0x1000035,EXTENDED|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrwie,  0x0a,0x1000035,EXTENDED|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrw,   0x09,0x21,ENCODE_U6|FLAG2_SET,	SYNTAX_VV0
 	.extInstruction	vasrri,   0x0a,0x1000039,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVC|SYNTAX_VV0
 	.extInstruction	vasrri,   0x0a,0x1000039,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction	vasrrwi,  0x09,0x21,ENCODE_U6 |FLAG2_SET, SYNTAX_VV0 
 	.extInstruction	vasrsr,  0x0a,0x100003c,SUFFIX_FLAG|EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrsr,  0x0a,0x100003c,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrsre,  0x0a,0x100003c,SUFFIX_FLAG|EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrsre,  0x0a,0x100003c,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrsrie,  0x0a,0x100003c,SUFFIX_FLAG|EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrsrie,  0x0a,0x100003c,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrsr,  0x09,0x35,FLAG2_SET|ENCODE_U6,	SYNTAX_VV0
	.extInstruction	vasrsrw, 0x0a,0x1000036,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG|FLAG2_SET,SYNTAX_VV0
	.extInstruction	vasrsrw, 0x0a,0x1000036,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET,SYNTAX_VV0
	.extInstruction	vasrsrwe, 0x0a,0x1000036,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG|FLAG2_SET,SYNTAX_VV0
	.extInstruction	vasrsrwe, 0x0a,0x1000036,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET,SYNTAX_VV0
	.extInstruction	vasrsrwie, 0x0a,0x1000036,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG|FLAG2_SET,SYNTAX_VV0
	.extInstruction	vasrsrwie, 0x0a,0x1000036,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET,SYNTAX_VV0
 	.extInstruction	vasrsrw,  0x09,0x37,ENCODE_U6|FLAG2_SET,SYNTAX_VV0
 	.extInstruction viv,      0x0a,0x232f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_IREGB|SYNTAX_VV0
	.extInstruction	vsignw,   0x0a,0x100022f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsignw,   0x0a,0x100022f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsignwe,   0x0a,0x100022f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsignwe,   0x0a,0x100022f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsignw,   0x09,0x22f,EXTEND1,	SYNTAX_VV
	.extInstruction	vsign,    0x0a,0x1000a2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsign,    0x0a,0x1000a2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsigne,    0x0a,0x1000a2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vsigne,    0x0a,0x1000a2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vnormw,   0x0a,0x1000c2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vnormw,   0x0a,0x1000c2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vnormwe,   0x0a,0x1000c2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vnormwe,   0x0a,0x1000c2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vnorme,   0x0a,0x1000d2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vnorme,   0x0a,0x1000d2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROB|SUFFIX_FLAG,	SYNTAX_VV
	.extInstruction	vimaxw,    0x0a,0x100000d,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vimaxw,    0x0a,0x100000d,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmaxw,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxw,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxwe,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxwe,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxw, 0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmaxw, 0x0a,0x100000c,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmaxwe, 0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmaxwe, 0x0a,0x100000c,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmaxw,    0x0a,0x100000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxw,    0x0a,0x100000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxwe,    0x0a,0x100000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxwe,    0x0a,0x100000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxw,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmaxw,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmaxw,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmaxw,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmaxw,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmaxw,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmaxwe,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmaxwe,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmaxwe,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmaxwe,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmaxwe,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmaxwe,    0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
       	.extInstruction	vmaxw,    0x0a,0x100000d,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV|SYNTAX_VVI
       	.extInstruction	vmaxw,    0x0a,0x100000d,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV|SYNTAX_VVI
       	.extInstruction	vmaxwe,    0x0a,0x100000d,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV|SYNTAX_VVI
       	.extInstruction	vmaxwe,    0x0a,0x100000d,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV|SYNTAX_VVI
	.extInstruction	vmaxw,    0x09,0x24,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmaxw,    0x09,0x25,FLAGS_NONE,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vmaxa,    0x0a,0x2000028,SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmaxa,    0x0a,0x2000028,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmaxae,    0x0a,0x2000028,SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmaxae,    0x0a,0x2000028,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmax,     0x0a,0x1000028,SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmax,     0x0a,0x1000028,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmaxe,     0x0a,0x1000028,SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmaxe,     0x0a,0x1000028,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmax,     0x0a,0x100000d,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmax,     0x0a,0x100000d,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmaxe,     0x0a,0x100000d,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmaxe,     0x0a,0x100000d,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vmax,     0x09,0x12,FLAGS_NONE,SYNTAX_VVV	
        .extInstruction vmaxa,     0x09,0x12,FLAG_SET,SYNTAX_VVV	
	.extInstruction	vmin,     0x0a,0x1000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmin,     0x0a,0x1000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmine,     0x0a,0x1000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmine,     0x0a,0x1000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VVV
        .extInstruction	vmin,     0x0a,0x1000010,FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV0|SYNTAX_VVI
        .extInstruction	vmin,     0x0a,0x1000010,FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VV0|SYNTAX_VVI
        .extInstruction	vmine,     0x0a,0x1000010,FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VV0|SYNTAX_VVI
        .extInstruction	vmine,     0x0a,0x1000010,FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vmin,     0x09,0x13,FLAGS_NONE,SYNTAX_VVV	
	.extInstruction viminw,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viminw,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viminwe,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viminwe,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vminw, 0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vminw, 0x0a,0x100000d,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction viminwe,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viminwe,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vminwe, 0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vminwe, 0x0a,0x100000d,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vminw,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vminw,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vminwe,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vminwe,    0x0a,0x100000f,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vminw,    0x0a,0x100000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminw,    0x0a,0x100000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminwe,    0x0a,0x100000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminwe,    0x0a,0x100000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vminw,   0x09,0x27,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vminw,    0x09,0x26,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction vmaxaw,   0x0a,0x200000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxaw,   0x0a,0x200000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxawe,   0x0a,0x200000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vmaxawe,   0x0a,0x200000a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
       	.extInstruction	vmaxaw,   0x09,0x24,FLAG_SET,   SYNTAX_VVV
       	.extInstruction	vmaxawe,   0x09,0x24,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmina,    0x0a,0x2000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmina,    0x0a,0x2000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VVV
       	.extInstruction	vmaxawe,   0x09,0x24,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vminae,    0x0a,0x2000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vminae,    0x0a,0x2000029,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VVV
        .extInstruction vmina,     0x09,0x13,FLAG_SET,SYNTAX_VVV	
	.extInstruction vminaw,   0x0a,0x200000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminaw,   0x0a,0x200000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminawe,   0x0a,0x200000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vminawe,   0x0a,0x200000b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction	vminaw,   0x09,0x26,FLAG_SET,   SYNTAX_VVV

	.extInstruction vbmaxw,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vbmaxw,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vbmaxwe,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vbmaxwe,   0x0a,0x100000c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction	vbmaxw,   0x09,0x25,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmaxwe,   0x09,0x25,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0

	.extInstruction vbminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vbminw,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vbminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vbminwe,   0x0a,0x100000d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction	vbminw,   0x09,0x27,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbminwe,   0x09,0x27,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction vdifw,    0x0a,0x100000e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vdifw,    0x0a,0x100000e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vdifwe,    0x0a,0x100000e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction vdifwe,    0x0a,0x100000e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VVV
	.extInstruction	vdifw,    0x09,0x1c,FLAGS_NONE,	SYNTAX_VVV

       	.extInstruction	vdif,     0x0a,0x1000027,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vdif,     0x0a,0x1000027,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vdife,     0x0a,0x1000027,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vdife,     0x0a,0x1000027,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction vdif,     0x09,0x10,FLAGS_NONE,SYNTAX_VVV
       	.extInstruction	vdifa,     0x0a,0x2000027,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vdifa,     0x0a,0x2000027,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vdifae,     0x0a,0x2000027,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
       	.extInstruction	vdifae,     0x0a,0x2000027,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction vdifa,     0x09,0x10,FLAG_SET,SYNTAX_VVV
	.extInstruction vdifaw,   0x0a,0x200000e,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV	
	.extInstruction vdifaw,   0x0a,0x200000e,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV	
	.extInstruction vdifawe,   0x0a,0x200000e,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV	
	.extInstruction vdifawe,   0x0a,0x200000e,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV	
	.extInstruction	vdifaw,   0x09,0x1c,FLAG_SET,   SYNTAX_VVV
    	.extInstruction	vsummw,   0x0a,0x1d,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U8,	SYNTAX_VV0
    	.extInstruction	vsummw,   0x0a,0x1d,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U8,	SYNTAX_VV0
    	.extInstruction	vsummwe,   0x0a,0x1d,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U8,	SYNTAX_VV0
    	.extInstruction	vsummwe,   0x0a,0x1d,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U8,	SYNTAX_VV0
    	.extInstruction	vsummw,   0x09,0x1e,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vsumm,   0x09,0x11,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction vxsum,    0x0a,0x3b,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vxsum,    0x0a,0x3b,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vxsume,    0x0a,0x3b,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vxsume,    0x0a,0x3b,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vxsum,    0x0a,0x1e,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxsum,    0x0a,0x1e,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxsumi,    0x0a,0x3c,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U16,SYNTAX_VV0
	.extInstruction vxsumi,    0x0a,0x3c,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U16,SYNTAX_VV0
	.extInstruction vxsume,    0x0a,0x3c,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U16,SYNTAX_VV0
	.extInstruction vxsume,    0x0a,0x3c,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U16,SYNTAX_VV0
	.extInstruction vxsum2,    0x0a,0x1e,EXTENDED|FLAG1_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxsum2,    0x0a,0x1e,EXTENDED|FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction	vxsumwi,   0x0a,0x3b,FLAG1_SET|EXTENDED|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction	vxsumwi,   0x0a,0x3b,FLAG1_SET|EXTENDED|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction	vxsumw,   0x0a,0x3b,FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG|ENCODE_U16,SYNTAX_VV0
	.extInstruction	vxsumw,   0x0a,0x3b,FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U16,SYNTAX_VV0
	.extInstruction	vxsumwe,   0x0a,0x3b,FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG|ENCODE_U16,SYNTAX_VV0
	.extInstruction	vxsumwe,   0x0a,0x3b,FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U16,SYNTAX_VV0
	.extInstruction	vxsumw,   0x0a,0x35,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsumw,   0x0a,0x35,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vixsumw,   0x0a,0x35,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vixsumw,   0x0a,0x35,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsumwe,   0x0a,0x35,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsumwe,   0x0a,0x35,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsum,   0x0a,0x3c,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction	vxsum,   0x0a,0x3c,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction	vxsume,   0x0a,0x3c,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction	vxsume,   0x0a,0x3c,FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_U16|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0
	.extInstruction	vxsum,   0x0a,0x36,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsum,   0x0a,0x36,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsume,   0x0a,0x36,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vxsume,   0x0a,0x36,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vand, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vande, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vband,     0x0a,0x1000039,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vband,     0x0a,0x1000039,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbande,     0x0a,0x1000039,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vbande,     0x0a,0x1000039,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vand, 0x0a,0x100000f,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vande, 0x0a,0x100000f,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vand, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vand, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vande, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vande, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vand,     0x09,0x30,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vor,      0x0a,0x1000010,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vore,      0x0a,0x1000010,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vor,      0x09,0x32,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vxor,     0x0a,0x1000011,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vxore,     0x0a,0x1000011,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vxor,     0x09,0x34,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vbic,     0x0a,0x1000012,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vbice,     0x0a,0x1000012,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vbic,     0x09,0x36,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vbica,     0x09,0x3c,FLAG_SET,	SYNTAX_VVV
	.extInstruction	vandaw,   0x09,0x30,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vanda,    0x09,0x38,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vbicaw,   0x09,0x36,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vxoraw,   0x09,0x34,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vxora,    0x09,0x3b,FLAG_SET,   SYNTAX_VVV

	.extInstruction	viltw,     0x0a,0x00001b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	viltw,     0x0a,0x00001b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltw,     0x0a,0x00001b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltw,     0x0a,0x00001b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltw,      0x0a,0x100002d,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vltw,      0x0a,0x100002d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vltwe,      0x0a,0x100002d,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vltwe,      0x0a,0x100002d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vltw,     0x0a,0x00001b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM|FLAG1_SET|FLAG2_SET,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltw,     0x0a,0x00001b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM|FLAG1_SET|FLAG2_SET,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltwe,     0x0a,0x1000023,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltwe,     0x0a,0x1000023,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vltw,     0x0a,0x1000016,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vltw,     0x0a,0x1000016,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vltwe,     0x0a,0x1000016,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vltwe,     0x0a,0x1000016,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vltw,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vltw,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction viltw,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction viltw,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vbiltw,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbiltw,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vltwe,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vltwe,    0x0a,0x0000023,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
       	.extInstruction	vltw,     0x09,0x3e,FLAGS_NONE,	SYNTAX_VVV
        .extInstruction	vlew,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC        
        .extInstruction	vlew,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL        
        .extInstruction	vlew,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC	
        .extInstruction	vlewe,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
        .extInstruction	vlewe,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
        .extInstruction	vlewe,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vilew,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vilew,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vilew,      0x0a,0x1000012,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vbilew,      0x0a,0x000022,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vbilew,      0x0a,0x000022,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vbilew,      0x0a,0x000022,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlew,      0x0a,0x000022,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlew,      0x0a,0x000022,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vlew,      0x0a,0x000022,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_MASK|SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlew,     0x0a,0x1000022,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlew,     0x0a,0x1000022,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlewe,     0x0a,0x1000022,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlewe,     0x0a,0x1000022,SUFFIX_FLAG|ENCODE_ZEROA|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction veqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction veqw,    0x0a,0x0000020,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veqwe,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veqwe,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction veqwe,    0x0a,0x0000020,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vieqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vieqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vieqw,    0x0a,0x0000020,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbieqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbieqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vbieqw,    0x0a,0x0000020,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veqw,    0x0a,0x0000020,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction veqw,    0x0a,0x0000020,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	veqw,     0x0a,0x1000013,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	veqw,     0x0a,0x1000013,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	veqwe,     0x0a,0x1000013,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	veqwe,     0x0a,0x1000013,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	veqw,     0x09,0x38,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vlt,      0x0a,0x1000014,SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlt,      0x0a,0x1000014,SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vlte,      0x0a,0x1000014,SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlte,      0x0a,0x1000014,SUFFIX_MASK|EXTENDED|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vlt,      0x0a,0x1000015,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlt,      0x0a,0x1000015,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vlte,      0x0a,0x1000015,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vlte,      0x0a,0x1000015,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vlew,    0x0a,0x0000022,FLAG1_SET|FLAG2_SET|EXTENDED,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vlewe,    0x0a,0x0000022,FLAG1_SET|FLAG2_SET|EXTENDED,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vlew,    0x0a,0x22,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vlew,    0x0a,0x22,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vlewe,    0x0a,0x22,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vlewe,    0x0a,0x22,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vilew,    0x0a,0x22,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vilew,    0x0a,0x22,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
        .extInstruction	vlew,      0x0a,0x1000015,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
        .extInstruction	vlew,      0x0a,0x1000015,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
        .extInstruction	vlewe,      0x0a,0x1000015,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
        .extInstruction	vlewe,      0x0a,0x1000015,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vlew,     0x09,0x3c,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction vle,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vle,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vlee,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vlee,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vle,      0x0a,0x0000026,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vlee,      0x0a,0x0000026,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vle,      0x0a,0x100002c,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vle,      0x0a,0x100002c,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vlee,      0x0a,0x100002c,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vlee,      0x0a,0x100002c,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
        .extInstruction	vle,      0x0a,0x1000016,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
        .extInstruction	vle,      0x0a,0x1000016,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
        .extInstruction	vlee,      0x0a,0x1000016,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
        .extInstruction	vlee,      0x0a,0x1000016,SUFFIX_MASK|EXTENDED,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vle,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vle,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vlee,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vlee,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vle,     0x09,0x3d,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction vlt,    0x0a,0x27,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vlt,    0x0a,0x27,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vlte,    0x0a,0x27,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vlte,    0x0a,0x27,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
        .extInstruction vlt,    0x0a,0x0000027,FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
        .extInstruction vlte,    0x0a,0x0000027,FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction	vlt,      0x0a,0x100002d,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlt,      0x0a,0x100002d,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlte,      0x0a,0x100002d,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlte,      0x0a,0x100002d,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV
	.extInstruction	vlt,      0x0a,0x100002d,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vlt,      0x0a,0x100002d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vlte,      0x0a,0x100002d,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vlte,      0x0a,0x100002d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vlt,     0x09,0x3f,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction vbine,    0x0a,0x25,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbine,    0x0a,0x25,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vbieq,    0x0a,0x24,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbieq,    0x0a,0x24,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vbile,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbile,    0x0a,0x26,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vbilt,    0x0a,0x27,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbilt,    0x0a,0x27,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vexch1,   0x0a,0x000032f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch1,   0x0a,0x000032f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch1e,   0x0a,0x000032f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch1e,   0x0a,0x000032f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch1,   0x09,0x42f,EXTEND1,SYNTAX_VV
	.extInstruction	vexch2,   0x0a,0x000042f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch2,   0x0a,0x000042f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch2e,   0x0a,0x000042f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch2e,   0x0a,0x000042f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch2,   0x09,0x052f,EXTEND1,	SYNTAX_VV
	.extInstruction	vexch4,   0x0a,0x000052f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch4,   0x0a,0x000052f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch4e,   0x0a,0x000052f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch4e,   0x0a,0x000052f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vexch4,   0x09,0x062f,EXTEND1,	SYNTAX_VV
	.extInstruction	vupbw,    0x0a,0x100062f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbw,    0x0a,0x100062f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbwe,    0x0a,0x100062f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbwe,    0x0a,0x100062f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbw,    0x09,0x032f,EXTEND1,	SYNTAX_VV
	.extInstruction vup,      0x0a,0x1000b2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
	.extInstruction vupe,      0x0a,0x1000b2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
	.extInstruction	vup,      0x09,0x132f,FLAGS_NONE|EXTEND1,SYNTAX_VV
	.extInstruction	vups,     0x0a,0x1000e2f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vupse,     0x0a,0x1000e2f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vups,     0x09,0x172f,EXTEND1,SYNTAX_VV
       	.extInstruction	vupa,     0x0a,0x2000b2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
       	.extInstruction	vupae,     0x0a,0x2000b2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
       	.extInstruction	vupa,     0x09,0x132f,FLAG_SET|EXTEND1,SYNTAX_VV
       	.extInstruction	vupae,     0x09,0x132f,FLAG_SET|EXTEND1,SYNTAX_VV
	.extInstruction	vupsa,    0x0a,0x2000e2f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
       	.extInstruction	vupae,     0x09,0x132f,FLAG_SET|EXTEND1,SYNTAX_VV
	.extInstruction	vupsae,    0x0a,0x2000e2f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
 	.extInstruction	vupsa,    0x09,0x172f,FLAG_SET|EXTEND1,SYNTAX_VV
	.extInstruction	vswpw,    0x0a,0x1000f2f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpwe,    0x0a,0x1000f2f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpw,    0x09,0x302f,EXTEND1,SYNTAX_VV
	.extInstruction	vswp,     0x0a,0x100102f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpe,     0x0a,0x100102f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswp,     0x09,0x322f,EXTEND1,SYNTAX_VV
	.extInstruction	vswpnlw,  0x0a,0x100112f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnlwe,  0x0a,0x100112f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnlw,  0x09,0x342f,EXTEND1,SYNTAX_VV
	.extInstruction	vswpnl,  0x0a,0x100122f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnle,  0x0a,0x100122f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnl,   0x09,0x362f,EXTEND1,SYNTAX_VV
	.extInstruction	vswpnhw,  0x0a,0x100132f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnhwe,  0x0a,0x100132f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV

	.extInstruction	vswpnhw,  0x09,0x382f,EXTEND1,SYNTAX_VV
	.extInstruction	vswpnh,   0x0a,0x100142f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnhe,   0x0a,0x100142f,EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction	vswpnh,   0x09,0x3a2f,EXTEND1,SYNTAX_VV
        .extInstruction	vnorm,    0x0a,0x1000d2f,EXTEND2|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnorm,    0x0a,0x1000d2f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnorme,    0x0a,0x1000d2f,EXTEND2|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnorme,    0x0a,0x1000d2f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnorm,    0x09,0x162f,EXTEND1,SYNTAX_VV
        .extInstruction	vnormw,   0x0a,0x1000c2f,EXTEND2|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnormw,   0x0a,0x1000c2f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnormwe,   0x0a,0x1000c2f,EXTEND2|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnormwe,   0x0a,0x1000c2f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
        .extInstruction	vnormw,   0x09,0x152f,EXTEND1,SYNTAX_VV
        .extInstruction	vsign,    0x09,0x122f,EXTEND1,SYNTAX_VV
	.extInstruction vabsa,    0x0a,0x200092f,EXTEND2|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
	.extInstruction vabsa,    0x0a,0x200092f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
	.extInstruction vabsae,    0x0a,0x200092f,EXTEND2|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
	.extInstruction vabsae,    0x0a,0x200092f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV
        .extInstruction	vabsa,    0x09,0x112f,FLAG_SET|EXTEND1,SYNTAX_VV
        .extInstruction	vabs,     0x09,0x112f,EXTEND1,SYNTAX_VV
        .extInstruction	vaddsu,   0x0a,0x100082f,SUFFIX_COND|SUFFIX_MASK|EXTEND2,SYNTAX_VV
        .extInstruction	vaddsue,   0x0a,0x100082f,SUFFIX_COND|SUFFIX_MASK|EXTEND2,SYNTAX_VV
        .extInstruction	vaddsu,   0x09,0x102f,EXTEND1,SYNTAX_VV
	.extInstruction	vupsbw,   0x0a,0x100072f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbw,   0x0a,0x100072f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbwe,   0x0a,0x100072f,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbwe,   0x0a,0x100072f,SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbw,   0x09,0x072f,EXTEND1,	SYNTAX_VV
 	.extInstruction	vasrpi,   0x0a,0x100003a,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrp,   0x0a,0x100003a,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrpe,   0x0a,0x100003a,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrpie,   0x0a,0x100003a,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrp,   0x0a,0x1000015,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0|SYNTAX_VVI
 	.extInstruction	viasrp,   0x0a,0x1000015,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
 	.extInstruction	vasrpe,   0x0a,0x1000015,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vasrp,   0x09,0x31,FLAG2_SET|ENCODE_U6,SYNTAX_VV0
 	.extInstruction	vasrrp,  0x0a,0x100003b,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrpe,  0x0a,0x100003b,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
 	.extInstruction	vasrrpie,  0x0a,0x100003b,EXTENDED|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,	SYNTAX_VV0
	.extInstruction vasrrp,   0x09,0x33,FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vasrpwb, 0x0a,0x1000037,ENCODE_U6|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0
	.extInstruction	vasrpwbe, 0x0a,0x1000037,ENCODE_U6|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0
	.extInstruction	vasrpwbie, 0x0a,0x1000037,ENCODE_U6|FLAG2_SET|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0
 	.extInstruction	vasrpwb,  0x09,0x22,FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vasrrpwb, 0x0a,0x1000038,FLAG2_SET|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0
	.extInstruction	vasrrpwbe, 0x0a,0x1000038,FLAG2_SET|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0
	.extInstruction	vasrrpwbie, 0x0a,0x1000038,FLAG2_SET|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0
 	.extInstruction	vasrrpwb, 0x09,0x23,FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vupbaw,   0x0a,0x200062f,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbaw,   0x0a,0x200062f,ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbawe,   0x0a,0x200062f,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbawe,   0x0a,0x200062f,ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupbaw,   0x09,0x032f,EXTEND1|FLAG_SET,   SYNTAX_VV
	.extInstruction	vupsbaw,  0x0a,0x200072f,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbaw,  0x0a,0x200072f,ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbawe,  0x0a,0x200072f,SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbawe,  0x0a,0x200072f,ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTEND2,SYNTAX_VV
	.extInstruction	vupsbaw,  0x09,0x072f,EXTEND1|FLAG_SET,   SYNTAX_VV

	.extInstruction vsr8,     0x0a,0x1000017,EXTENDED|SUFFIX_MASK|FLAG2_SET|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vsr8e,     0x0a,0x1000017,EXTENDED|SUFFIX_MASK|FLAG2_SET|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vsr8ie,     0x0a,0x1000017,EXTENDED|SUFFIX_MASK|FLAG2_SET|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vsr8,     0x0a,0x1000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsr8e,     0x0a,0x1000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsr8,     0x09,0x0a,ENCODE_U6|FLAG2_SET, SYNTAX_VV0
	.extInstruction	vsr8,     0x09,0x0a,FLAGS_NONE,SYNTAX_VVI|SYNTAX_VV0
	
       	.extInstruction	vsr8a,     0x0a,0x200000c,EXTENDED|FLAG2_SET|ENCODE_U6|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
       	.extInstruction	vsr8a,     0x0a,0x200000c,EXTENDED|FLAG2_SET|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
       	.extInstruction	vsr8ae,     0x0a,0x200000c,EXTENDED|FLAG2_SET|ENCODE_U6|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
       	.extInstruction	vsr8ae,     0x0a,0x200000c,EXTENDED|FLAG2_SET|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
       	.extInstruction	vsr8aie,     0x0a,0x200000c,EXTENDED|FLAG2_SET|ENCODE_U6|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
       	.extInstruction	vsr8aie,     0x0a,0x200000c,EXTENDED|FLAG2_SET|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
       	.extInstruction	vsr8a,     0x0a,0x2000038,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVI|SYNTAX_VV0
       	.extInstruction	vsr8a,     0x0a,0x2000038,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVI|SYNTAX_VV0
       	.extInstruction	vsr8ae,     0x0a,0x2000038,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVI|SYNTAX_VV0
       	.extInstruction	vsr8ae,     0x0a,0x2000038,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVI|SYNTAX_VV0
	.extInstruction	vsr8a,    0x09,0x39,FLAG_SET,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vsr8a,    0x09,0x39,FLAG_SET|FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction	vsr8w,     0x09,0x0a,ENCODE_U8|FLAG2_SET|FLAG_SET,SYNTAX_VV0

	.extInstruction	vupsbaw,  0x0a,0x1000027,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vupsbawe,  0x0a,0x1000027,SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	
	.extInstruction vsr8aw,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8aw,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8awi,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8awi,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8awie,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8awie,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8awe,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction vsr8awe,  0x0a,0x2000017,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET, SYNTAX_VV0
	.extInstruction	vsr8aw,   0x0a,0x2000017,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vsr8aw,   0x0a,0x2000017,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vsr8awe,   0x0a,0x2000017,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vsr8awe,   0x0a,0x2000017,EXTENDED|ENCODE_U6|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG, SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vsr8aw,   0x09,0x0a,FLAG_SET,  SYNTAX_VVI|SYNTAX_VV0
       	.extInstruction	vsr8aw,   0x09,0x0a,ENCODE_U6|FLAG2_SET|FLAG_SET, SYNTAX_VV0
	.extInstruction vlslv,    0x0a,0x1000014,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlslv,    0x0a,0x1000014,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlslvw,    0x0a,0x1000013,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlslvw,    0x0a,0x1000013,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vkadd,    0x0a,0x1000004,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkadd,    0x0a,0x1000004,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vadd,    0x0a,0x1000004,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vadd,    0x0a,0x1000004,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkadda,    0x0a,0x2000004,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkadda,    0x0a,0x2000004,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vadda,    0x0a,0x2000004,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vadda,    0x0a,0x2000004,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vksub,    0x0a,0x1000006,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vksub,    0x0a,0x1000006,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsub,    0x0a,0x1000006,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsub,    0x0a,0x1000006,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vksuba,    0x0a,0x2000006,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vksuba,    0x0a,0x2000006,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsuba,    0x0a,0x2000006,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsuba,    0x0a,0x2000006,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkrsub,    0x0a,0x1000008,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkrsub,    0x0a,0x1000008,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vrsub,    0x0a,0x1000008,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vrsub,    0x0a,0x1000008,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkrsuba,    0x0a,0x2000008,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkrsuba,    0x0a,0x2000008,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vrsuba,    0x0a,0x2000008,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vrsuba,    0x0a,0x2000008,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkmax,    0x0a,0x100000e,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkmax,    0x0a,0x100000e,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmax,    0x0a,0x100000e,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmax,    0x0a,0x100000e,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkmin,    0x0a,0x1000010,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vkmin,    0x0a,0x1000010,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmin,    0x0a,0x1000010,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmin,    0x0a,0x1000010,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vasrv,    0x0a,0x1000012,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vasrv,    0x0a,0x1000012,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vasrvw,    0x0a,0x1000011,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vasrvw,    0x0a,0x1000011,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vasr,     0x0a,0x1000026,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vasr,     0x0a,0x1000026,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vasre,     0x0a,0x1000026,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vasre,     0x0a,0x1000026,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vasr,    0x0a,0x1000026,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vasr,    0x0a,0x1000026,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vasre,    0x0a,0x1000026,SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vasre,    0x0a,0x1000026,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vasrie,    0x0a,0x1000026,SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vasrie,    0x0a,0x1000026,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vasr,    0x09,0xb,FLAGS_NONE,SYNTAX_VVV|SYNTAX_VVI
	.extInstruction vasr,    0x09,0xb,ENCODE_U6|FLAG2_SET,SYNTAX_VV0
	.extInstruction vblsrvw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vblsrvw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vblsrvw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vblsrvw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vlsrw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlsrw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlsrvw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlsrvw,    0x0a,0x1000028,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vblsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vblsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vblsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vblsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vlsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vlsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vlsrw,   0x0a,0x100002a,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|EXTENDED,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vblsr,    0x0a,0x1000029,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vblsr,    0x0a,0x1000029,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vblsr,    0x0a,0x100002b,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vblsr,    0x0a,0x100002b,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vblsr,    0x0a,0x100002b,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vlsr,    0x0a,0x100002b,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vlsr,    0x0a,0x100002b,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vlsr,    0x0a,0x100002b,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vixmax,    0x0a,0x100002d,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vixmax,    0x0a,0x100002d,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vixmin,    0x0a,0x1000030,SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vixmin,    0x0a,0x1000030,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vixsum,    0x0a,0x1000036,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vixsum,    0x0a,0x1000036,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vgm,    0x0a,0x000072f,EXTEND2|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGB
	.extInstruction vgm,    0x0a,0x000072f,EXTEND2|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGB
	.extInstruction vgmw,    0x0a,0x000072f,EXTEND2|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGB
	.extInstruction vgmw,    0x0a,0x000072f,EXTEND2|ENCODE_ZEROB|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGB
	.extInstruction vieq,    0x0a,0x000001c,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction veq,    0x0a,0x000001c,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vine,    0x0a,0x000001d,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vne,    0x0a,0x000001d,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vile,    0x0a,0x000001e,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vle,    0x0a,0x000001e,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vilt,    0x0a,0x000001f,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vlt,    0x0a,0x000001f,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vieqw,    0x0a,0x0000018,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction veqw,    0x0a,0x0000018,SUFFIX_MASK|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vbinew,    0x0a,0x21,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vbinew,    0x0a,0x21,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vinew,    0x0a,0x21,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vinew,    0x0a,0x21,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vinew,    0x0a,0x19,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vnew,    0x0a,0x19,EXTENDED|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vnew,    0x0a,0x19,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vilew,    0x0a,0x000001a,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vilew,    0x0a,0x000001a,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vlew,    0x0a,0x000001a,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction viltw,    0x0a,0x000001b,SUFFIX_MASK|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA	
	.extInstruction viltw,    0x0a,0x000001b,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|EXTENDED|ENCODE_SETLM,SYNTAX_VVV|SYNTAX_IREGA	
	.extInstruction	veq,      0x0a,0x100002a,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	veq,      0x0a,0x100002a,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	veqe,      0x0a,0x100002a,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	veqe,      0x0a,0x100002a,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction veq,    0x0a,0x24,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veq,    0x0a,0x24,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction veqe,    0x0a,0x24,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction veqe,    0x0a,0x24,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction veq,    0x09,0x39,FLAGS_NONE,SYNTAX_VVV
	.extInstruction vne,    0x0a,0x25,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vne,    0x0a,0x25,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vnee,    0x0a,0x25,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vnee,    0x0a,0x25,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vne,     0x0a,0x100002b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vne,     0x0a,0x100002b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vnee,     0x0a,0x100002b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vnee,     0x0a,0x100002b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vne,     0x0a,0x000025,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vne,     0x0a,0x000025,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vnee,     0x0a,0x000025,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vnee,     0x0a,0x000025,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction	vne,     0x0a,0x100002b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vne,     0x0a,0x100002b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vnee,     0x0a,0x100002b,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vnee,     0x0a,0x100002b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vne,     0x0a,0x0000025,FLAG1_SET|FLAG2_SET|EXTENDED,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vnee,     0x0a,0x0000025,FLAG1_SET|FLAG2_SET|EXTENDED,SYNTAX_VVV|SYNTAX_IREGA
	.extInstruction vne,     0x09,0x3b,FLAGS_NONE,SYNTAX_VVV
	.extInstruction vnew,    0x0a,0x000021,FLAG1_SET|SUFFIX_FLAG|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vnew,    0x0a,0x000021,FLAG1_SET|SUFFIX_FLAG|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vnew,    0x0a,0x000021,FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vnewe,    0x0a,0x000021,FLAG1_SET|SUFFIX_FLAG|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vnewe,    0x0a,0x000021,FLAG1_SET|SUFFIX_FLAG|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vnewe,    0x0a,0x000021,FLAG1_SET|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction	vnew,     0x0a,0x1000014,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vnew,     0x0a,0x1000014,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vnewe,     0x0a,0x1000014,SUFFIX_MASK|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vnewe,     0x0a,0x1000014,SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|EXTENDED,SYNTAX_VVV
	.extInstruction	vnew,     0x09,0x3a,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vnewe,     0x09,0x3a,FLAGS_NONE,	SYNTAX_VVV
        .extInstruction	vkv,      0x0a,0x242f,ENCODE_KREG|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_KREG,SYNTAX_VV0|SYNTAX_IREGB
        .extInstruction	vkm,      0x0a,0x1a,ENCODE_KREG|FLAG1_SET|EXTENDED|SUFFIX_MASK|ENCODE_U6|ENCODE_KREG|ENCODE_SETLM,SYNTAX_V00|SYNTAX_IREGA|SYNTAX_IREGB
	.extInstruction	vmr1w,    0x0a,0x1000018,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmr1w,    0x0a,0x1000018,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmr1we,    0x0a,0x1000018,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmr1we,    0x0a,0x1000018,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
        .extInstruction	vmr1w,    0x09,0x28,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmr2w,    0x0a,0x1000019,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2w,    0x0a,0x1000019,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2we,    0x0a,0x1000019,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2we,    0x0a,0x1000019,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2w,    0x09,0x29,FLAGS_NONE,	SYNTAX_VVV 
	.extInstruction	vmr2a,    0x0a,0x2000035,SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2a,    0x0a,0x2000035,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2ae,    0x0a,0x2000035,SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr2ae,    0x0a,0x2000035,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction vmr2a,    0x09,0x3d,FLAG_SET,SYNTAX_VVV
	.extInstruction	vmr3w,    0x0a,0x100001a,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr3w,    0x0a,0x100001a,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr3we,    0x0a,0x100001a,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr3we,    0x0a,0x100001a,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr3w,    0x09,0x2a,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmr4w,    0x0a,0x100001b,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4w,    0x0a,0x100001b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4we,    0x0a,0x100001b,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4we,    0x0a,0x100001b,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4w,    0x09,0x2b,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmr4a,    0x0a,0x2000036,SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4a,    0x0a,0x2000036,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4ae,    0x0a,0x2000036,SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr4ae,    0x0a,0x2000036,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction vmr4a,    0x09,0x3e,FLAG_SET,SYNTAX_VVV
	.extInstruction	vmr5w,    0x0a,0x100001c,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr5w,    0x0a,0x100001c,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr5we,    0x0a,0x100001c,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr5we,    0x0a,0x100001c,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr5w,    0x09,0x2c,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmr6w,    0x0a,0x100001d,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6w,    0x0a,0x100001d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6we,    0x0a,0x100001d,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6we,    0x0a,0x100001d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6a,    0x0a,0x2000037,SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6a,    0x0a,0x2000037,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6ae,    0x0a,0x2000037,SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr6ae,    0x0a,0x2000037,ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction vmr6a,    0x09,0x3f,FLAG_SET,SYNTAX_VVV
	.extInstruction	vmr6w,    0x09,0x2d,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vmr7w,    0x0a,0x100001e,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr7w,    0x0a,0x100001e,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr7we,    0x0a,0x100001e,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr7we,    0x0a,0x100001e,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,	SYNTAX_VVV
	.extInstruction	vmr7w,    0x09,0x2e,FLAGS_NONE,	SYNTAX_VVV

	

	.extInstruction	vmrb,     0x0a,0x100001f,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmrb,     0x0a,0x100001f,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmrbe,     0x0a,0x100001f,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmrbe,     0x0a,0x100001f,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,	SYNTAX_VVV
	.extInstruction	vmrb,     0x09,0x1d,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vavb,     0x09,0x08,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vavrb,    0x09,0x09,FLAGS_NONE,	SYNTAX_VVV

	.extInstruction	vmr1aw,   0x0a,0x2000018,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr1aw,   0x0a,0x2000018,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr1awe,   0x0a,0x2000018,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr1awe,   0x0a,0x2000018,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr1aw,   0x09,0x28,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmr2aw,   0x0a,0x2000019,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr2aw,   0x0a,0x2000019,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr2awe,   0x0a,0x2000019,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr2awe,   0x0a,0x2000019,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
        
	.extInstruction	vmr2aw,   0x09,0x29,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmr3aw,   0x0a,0x200001a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr3aw,   0x0a,0x200001a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr3awe,   0x0a,0x200001a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr3awe,   0x0a,0x200001a,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr3aw,   0x09,0x2a,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmr4aw,   0x0a,0x200001b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr4aw,   0x0a,0x200001b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr4awe,   0x0a,0x200001b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr4awe,   0x0a,0x200001b,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr4aw,   0x09,0x2b,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmr5aw,   0x0a,0x200001c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr5aw,   0x0a,0x200001c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr5awe,   0x0a,0x200001c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr5awe,   0x0a,0x200001c,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr5aw,   0x09,0x2c,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmr6aw,   0x0a,0x200001d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr6aw,   0x0a,0x200001d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr6awe,   0x0a,0x200001d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr6awe,   0x0a,0x200001d,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr6aw,   0x09,0x2d,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmr7aw,   0x0a,0x200001e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr7aw,   0x0a,0x200001e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr7awe,   0x0a,0x200001e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr7awe,   0x0a,0x200001e,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,   SYNTAX_VVV
	.extInstruction	vmr7aw,   0x09,0x2e,FLAG_SET,   SYNTAX_VVV

	.extInstruction	vnop,     0x09,0x3f2f,EXTEND3|ENCODE_ZEROC,SYNTAX_0
	.extInstruction	vnop,     0x09,0x3f2f,EXTEND3|ENCODE_ZEROC|ENCODE_LIMM,SYNTAX_C|SYNTAX_0
	.extInstruction	vrec,     0x09,0x00,ENCODE_LIMM,SYNTAX_C|SYNTAX_0
	.extInstruction	vrecrun,  0x09,0x01,ENCODE_LIMM,SYNTAX_C|SYNTAX_0
	.extInstruction	vrun,     0x09,0x02,ENCODE_LIMM,SYNTAX_C|SYNTAX_0
	.extInstruction	vendrec,  0x09,0x03,ENCODE_LIMM,SYNTAX_C|SYNTAX_0

	.extInstruction	vdirun,   0x09,0x082f,EXTEND1,SYNTAX_CC|SYNTAX_C0
	.extInstruction	vdorun,   0x09,0x092f,EXTEND1,SYNTAX_CC|SYNTAX_C0
	.extInstruction	vdiwr,    0x09,0x0a2f,EXTEND1|ENCODE_LIMM,SYNTAX_DC|SYNTAX_D0
	.extInstruction	vdiwr,    0x09,0x0a2f,EXTEND1,SYNTAX_VL0|SYNTAX_D0
	.extInstruction	vdowr,    0x09,0x0b2f,EXTEND1|ENCODE_LIMM,SYNTAX_DC|SYNTAX_D0
	.extInstruction	vdowr,    0x09,0x0b2f,EXTEND1,SYNTAX_VL0|SYNTAX_D0
	.extInstruction	vdird,    0x09,0x0c,FLAGS_NONE,	SYNTAX_VD
	.extInstruction	vdord,    0x09,0x0d,FLAGS_NONE,	SYNTAX_VD

	.extInstruction	vh264fw,  0x09,0x01,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vh264ft,  0x09,0x0f,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vh264f,   0x09,0x0e,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vvc1ft,   0x09,0x0d,FLAGS_NONE,	SYNTAX_VVV
	.extInstruction	vvc1f,    0x09,0x0c,FLAGS_NONE,	SYNTAX_VVV

        .extInstruction	vinti,     0x0a,0x0003f2f,EXTEND3|EXTENDED|ENCODE_U6|FLAG2_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_0
        .extInstruction	vintie,     0x0a,0x0003f2f,EXTEND3|EXTENDED|ENCODE_U6|FLAG2_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_0
	.extInstruction	vinti,     0x09,0x043f2f,EXTEND1|ENCODE_U6|FLAG2_SET, SYNTAX_0	
        .extInstruction	vint,     0x0a,0x0003f2f,EXTEND3|EXTENDED|ENCODE_U6|FLAG2_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_0
        .extInstruction	vinte,     0x0a,0x0003f2f,EXTEND3|EXTENDED|ENCODE_U6|FLAG2_SET|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_0
	.extInstruction	vint,     0x09,0x43f2f,EXTEND1|ENCODE_U6|FLAG2_SET, SYNTAX_0	
	.extInstruction	vmiv,     0x0a,0x100172f,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTEND2, SYNTAX_V00|SYNTAX_IREGA
	.extInstruction	vmiv,     0x0a,0x100172f,ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTEND2, SYNTAX_V00|SYNTAX_IREGA
	.extInstruction	vmivw,    0x0a,0x100162f,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTEND2, SYNTAX_V00|SYNTAX_IREGA
	.extInstruction	vmivw,    0x0a,0x100162f,ENCODE_ZEROB|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTEND2, SYNTAX_V00|SYNTAX_IREGA
        .extInstruction vbrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vbrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
        .extInstruction vbrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vbrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
        .extInstruction vbrsubwe,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vbrsubwe,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
        .extInstruction vbrsubwe,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VVV|SYNTAX_VVL
        .extInstruction vrsubw,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vrsubwe,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vrsubwe,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVL
        .extInstruction vrsubwe,   0x0a,0x1000006,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vrsubw,   0x0a,0x1000007,EXTENDED|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubw,   0x0a,0x1000007,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction virsubw,   0x0a,0x1000007,EXTENDED|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction virsubw,   0x0a,0x1000007,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubwe,   0x0a,0x1000007,EXTENDED|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubwe,   0x0a,0x1000007,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vbrsubw,  0x09,0x1f,FLAGS_NONE,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vrsubw,   0x09,0x1f,FLAGS_NONE,SYNTAX_VVC|SYNTAX_VV0
	
        .extInstruction vrsubaw,   0x0a,0x2000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubaw,   0x0a,0x2000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction virsubaw,   0x0a,0x2000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction virsubaw,   0x0a,0x2000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubawe,   0x0a,0x2000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubawe,   0x0a,0x2000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsuba,   0x0a,0x2000006,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsuba,   0x0a,0x2000006,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubae,   0x0a,0x2000006,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsubae,   0x0a,0x2000006,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_MASK|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsub,   0x0a,0x1000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsub,   0x0a,0x1000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsube,   0x0a,0x1000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vrsube,   0x0a,0x1000007,EXTENDED|FLAG2_SET|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	
	.extInstruction vsendr,     0x0a,0x35,FLAG1_SET|EXTENDED|ENCODE_S12|ENCODE_SETLM,SYNTAX_C00
	.extInstruction vsend,      0x0a,0x35,FLAG1_SET|EXTENDED|ENCODE_S12|ENCODE_SETLM,SYNTAX_C00
	.extInstruction vsendi,     0x0a,0x36,FLAG1_SET|EXTENDED|ENCODE_S12|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C00|SYNTAX_IREGB
	.extInstruction vsend,      0x0a,0x36,FLAG1_SET|EXTENDED|ENCODE_S12|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C00|SYNTAX_IREGB
        .extInstruction vchstat,   0x0a,0x023f2f,FLAG1_SET|FLAG2_SET|SUFFIX_MASK|EXTEND3|ENCODE_SETLM,SYNTAX_0|SYNTAX_VVI
        .extInstruction	chstat,   0x05, 0xd2f, EXTEND1, SYNTAX_C0|SYNTAX_CC
	.extInstruction vscq,      0x0a,0x012f,FLAG2_SET|SUFFIX_MASK|EXTEND2|ENCODE_U6|ENCODE_SETLM,SYNTAX_C00|SYNTAX_IREGB
	.extInstruction vbadd, 0x0a,0x1000004,EXTENDED|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0
	.extInstruction vbadd, 0x0a,0x1000004,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0
	.extInstruction vbsubw, 0x0a,0x1000025,EXTENDED|SUFFIX_FLAG|ENCODE_LIMM|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbsubw, 0x0a,0x1000025,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_LIMM|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsubw, 0x0a,0x1000025,EXTENDED|SUFFIX_FLAG|ENCODE_LIMM|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsubw, 0x0a,0x1000025,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_LIMM|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbsub, 0x0a,0x1000005,EXTENDED|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0
	.extInstruction vbsub, 0x0a,0x1000005,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0
	.extInstruction vbmulf, 0x09,0x7,FLAGS_NONE,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulfa, 0x09,0x7,FLAG_SET,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction viaddaw, 0x0a,0x2000003,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction viaddaw, 0x0a,0x2000003,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET,SYNTAX_VV0|SYNTAX_VVI
	
	.extInstruction vmulaw, 0x0a,0x2000002,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vmulawe, 0x0a,0x2000002,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vmulaw, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmulaw, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulawe, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulawe, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmulawe, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vmulawe, 0x0a,0x2000007,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vmulaw, 0x0a,0x2000009,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vimulaw, 0x0a,0x2000009,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmulawe, 0x0a,0x2000009,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmulaw,   0x09,0x18,FLAG_SET,   SYNTAX_VVV
	.extInstruction	vmulaw,   0x09,0x19,FLAG_SET,   SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulf,    0x0a,0x1000025,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulf,    0x0a,0x1000025,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vmulfe,    0x0a,0x1000025,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVC
	.extInstruction	vmulfe,    0x0a,0x1000025,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vmulf,    0x0a,0x1000024,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction	vmulfe,    0x0a,0x1000024,EXTENDED|SUFFIX_MASK|SUFFIX_COND,	SYNTAX_VVV
	.extInstruction vmulf,    0x09,0x6,FLAGS_NONE,SYNTAX_VVV
	.extInstruction vmulfa,    0x09,0x6,FLAG_SET,SYNTAX_VVV
	.extInstruction vmulfaw, 0x0a,0x2000003,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vmulfawe, 0x0a,0x2000003,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vmulfaw, 0x0a,0x200000b,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vimulfaw, 0x0a,0x200000b,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmulfawe, 0x0a,0x200000b,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction	vmulfaw,  0x09,0x1a,FLAG_SET,   SYNTAX_VVV
	.extInstruction vbmul, 0x0a,0x1000023,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmul, 0x0a,0x1000023,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmule, 0x0a,0x1000023,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmule, 0x0a,0x1000023,EXTENDED|ENCODE_U6|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmul, 0x09,0x5,ENCODE_U6,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmula, 0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmula, 0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulae, 0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulae, 0x0a,0x2000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmula, 0x09,0x5,FLAG_SET|ENCODE_U6,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulf, 0x0a,0x1000008,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulf, 0x0a,0x1000008,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulf, 0x0a,0x1000008,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulfe, 0x0a,0x1000008,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulfe, 0x0a,0x1000008,EXTENDED|SUFFIX_MASK|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulfe, 0x0a,0x1000008,EXTENDED|SUFFIX_MASK|ENCODE_ZEROA|SUFFIX_FLAG|ENCODE_U6|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbmulf, 0x09,0x7,FLAGS_NONE,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vstoprec, 0x09,0x0053f2f,EXTEND1,SYNTAX_0|SYNTAX_C
	.extInstruction vseqw, 0x0a,0x2c2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vseqw, 0x0a,0x342f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vseqw, 0x0a,0x342f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbseqw, 0x0a,0x342f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbseqw, 0x0a,0x342f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsltw, 0x0a,0x2f2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vsltw, 0x0a,0x372f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vsltw, 0x0a,0x372f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbsltw, 0x0a,0x372f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbsltw, 0x0a,0x372f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vseqw, 0x0a,0x2c2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_V00
	.extInstruction vbseqw, 0x0a,0x2c2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_V00
	.extInstruction vsnew, 0x0a,0x352f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vsnew, 0x0a,0x352f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbsnew, 0x0a,0x352f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbsnew, 0x0a,0x352f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsnew, 0x0a,0x2d2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vsnew, 0x0a,0x2d2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_V00
	.extInstruction vbsnew, 0x0a,0x2d2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_V00
	.extInstruction vseqw, 0x0a,0x342f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vseqw, 0x0a,0x342f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vslew, 0x0a,0x2e2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vslew, 0x0a,0x362f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vslew, 0x0a,0x362f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbslew, 0x0a,0x362f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbslew, 0x0a,0x362f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbseq, 0x0a,0x382f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbseq, 0x0a,0x382f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vseq, 0x0a,0x302f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vseq, 0x0a,0x382f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vseq, 0x0a,0x382f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsne, 0x0a,0x392f,FLAG_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vsne, 0x0a,0x392f,FLAG_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbsne, 0x0a,0x392f,FLAG_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbsne, 0x0a,0x392f,FLAG_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsne, 0x0a,0x312f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vbsne, 0x0a,0x312f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND|ENCODE_U6,SYNTAX_VV0
	.extInstruction vsle, 0x0a,0x322f,FLAG_SET|EXTEND2|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
	.extInstruction vbsle, 0x0a,0x322f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vbsle, 0x0a,0x3a2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbsle, 0x0a,0x3a2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vsle, 0x0a,0x3a2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vsle, 0x0a,0x3a2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vslt, 0x0a,0x332f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vslt, 0x0a,0x3b2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vslt, 0x0a,0x3b2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbslt, 0x0a,0x3b2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vbslt, 0x0a,0x3b2f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbslt, 0x0a,0x332f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VC0
	.extInstruction vbslt, 0x0a,0x332f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VL0
	.extInstruction vslt, 0x0a,0x332f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VC0
	.extInstruction vslt, 0x0a,0x332f,FLAG_SET|EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VL0
	.extInstruction vband, 0x0a,0x1000039,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulf, 0x0a,0x1000025,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulf, 0x0a,0x1000025,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmul, 0x0a,0x1000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmule, 0x0a,0x1000023,EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmula, 0x0a,0x2000023,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmula, 0x0a,0x2000023,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulae, 0x0a,0x2000023,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulae, 0x0a,0x2000023,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulfw, 0x0a,0x1000003,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVV
	.extInstruction vbmulfw, 0x0a,0x1000003,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVV
	.extInstruction vbmulfwe, 0x0a,0x1000003,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVV
	.extInstruction vbmulfwe, 0x0a,0x1000003,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VVV
	.extInstruction vbmulfw, 0x0a,0x1000008,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulfwe, 0x0a,0x1000008,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulfwe, 0x0a,0x1000008,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction	vbmulfw,  0x09,0x1b,ENCODE_LIMM,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulfw,  0x09,0x1b,ENCODE_ZEROC,SYNTAX_VVL|SYNTAX_VV0
	.extInstruction vbmulaw, 0x0a,0x2000007,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulawe, 0x0a,0x2000007,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulawe, 0x0a,0x2000007,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND|ENCODE_LIMM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vbmulaw, 0x0a,0x2000002,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vbmulaw, 0x0a,0x2000002,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vbmulawe, 0x0a,0x2000002,EXTENDED|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vbmulawe, 0x0a,0x2000002,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction	vbmulaw,  0x09,0x19,FLAG_SET,SYNTAX_VVC|SYNTAX_VV0
	.extInstruction	vbmulaw,  0x09,0x19,FLAG_SET|ENCODE_LIMM, SYNTAX_VVL|SYNTAX_VV0
	.extInstruction visubw,  0x0a,0x1000005,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction visubw,  0x0a,0x1000005,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubw,  0x0a,0x1000005,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubw,  0x0a,0x1000005,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubwe,  0x0a,0x1000005,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsubwe,  0x0a,0x1000005,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vclrstk, 0x0a,0x43f2f,SUFFIX_COND|SUFFIX_MASK|FLAG_SET|FLAG2_SET|EXTEND2|EXTEND3,SYNTAX_0
	.extInstruction vsetstk, 0x0a,0x53f2f,SUFFIX_COND|SUFFIX_MASK|FLAG_SET|FLAG2_SET|EXTEND2|EXTEND3,SYNTAX_0
	.extInstruction vdmawait, 0x0a,0x3f2f,EXTEND3|SUFFIX_MASK|FLAG1_SET|ENCODE_U16|ENCODE_U6|ENCODE_SETLM,SYNTAX_C0
	.extInstruction vdmaosetr, 0x0a,0x1e2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC
	.extInstruction vdmaosetr, 0x0a,0x1e2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0
	.extInstruction vdmaoset, 0x0a,0x1e2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC
	.extInstruction vdmaoset, 0x0a,0x1e2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0
	.extInstruction vdmaosetk, 0x0a,0x222f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM|ENCODE_KREG,SYNTAX_D0|SYNTAX_VVI
	.extInstruction vdmaoset, 0x0a,0x222f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM|ENCODE_KREG,SYNTAX_D0|SYNTAX_VVI
	.extInstruction vdmairuni, 0x0a,0x1f2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VVI
	.extInstruction vdmairun, 0x0a,0x1f2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_VVI
	.extInstruction vdmairun, 0x0a,0x1b2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC|SYNTAX_CC
	.extInstruction vdmairun, 0x0a,0x1b2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0|SYNTAX_CC
	.extInstruction vdmairunr, 0x0a,0x1b2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC|SYNTAX_CC
	.extInstruction vdmairunr, 0x0a,0x1b2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0|SYNTAX_CC
	.extInstruction vdmaoruni, 0x0a,0x202f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_VVI
	.extInstruction vdmaorun, 0x0a,0x202f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_C0|SYNTAX_IREGB|SYNTAX_VVI
	.extInstruction vdmaorun, 0x0a,0x1c2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC|SYNTAX_CC
	.extInstruction vdmaorun, 0x0a,0x1c2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0|SYNTAX_CC
	.extInstruction vdmaorunr, 0x0a,0x1c2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC|SYNTAX_CC
	.extInstruction vdmaorunr, 0x0a,0x1c2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0|SYNTAX_CC
	.extInstruction vdmaisetr, 0x0a,0x1d2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC
	.extInstruction vdmaisetr, 0x0a,0x1d2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0
	.extInstruction vdmaiset, 0x0a,0x1d2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_DC
	.extInstruction vdmaiset, 0x0a,0x1d2f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_D0|SYNTAX_VL0
	.extInstruction vdmaisetk, 0x0a,0x212f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM|ENCODE_KREG,SYNTAX_D0|SYNTAX_VVI
	.extInstruction vdmaiset, 0x0a,0x212f,EXTEND2|SUFFIX_MASK|ENCODE_SETLM|ENCODE_KREG,SYNTAX_D0|SYNTAX_VVI
	.extInstruction vpushw,   0x0a,0x52f,FLAG1_SET|FLAG2_SET|EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_VVC
	.extInstruction vpushw,   0x0a,0x52f,FLAG1_SET|FLAG2_SET|EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_VVL
	.extInstruction vpushiw,  0x0a,0x62f,FLAG1_SET|FLAG2_SET|EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vpushw,  0x0a,0x62f,FLAG1_SET|FLAG2_SET|EXTEND2|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vpopw,    0x0a,0x1,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVC
	.extInstruction vpopw,    0x0a,0x1,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVL
	.extInstruction vpopw,    0x0a,0x1,FLAG1_SET|FLAG2_SET|ENCODE_U6|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA
	.extInstruction vpopw,    0x0a,0x2,FLAG1_SET|FLAG2_SET|ENCODE_U6|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_IREGA|SYNTAX_VVI
	.extInstruction vpopiw,   0x0a,0x2,FLAG1_SET|FLAG2_SET|EXTENDED|SUFFIX_MASK|ENCODE_SETLM,SYNTAX_VV0|SYNTAX_VVI|SYNTAX_IREGA
	.extInstruction vsmw,     0x0a,0x100182f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsmw,     0x0a,0x100182f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsm,      0x0a,0x100192f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vsm,      0x0a,0x100192f,EXTEND2|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROB|SUFFIX_FLAG,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vmrg,     0x0a,0x1000030,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vmrg,     0x0a,0x1000030,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vmrgw,    0x0a,0x100002e,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vmrgw,    0x0a,0x100002e,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vadcw,   0x0a,0x1000031,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vadcw,   0x0a,0x1000031,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vadc,    0x0a,0x1000032,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vadc,    0x0a,0x1000032,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vsbcw,   0x0a,0x1000033,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vsbcw,   0x0a,0x1000033,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vsbc,    0x0a,0x1000034,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vsbc,    0x0a,0x1000034,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vaddnaw,  0x0a,0x00,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vaddnaw,  0x0a,0x00,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vaddna,   0x0a,0x20,SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vaddna,   0x0a,0x20,ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|EXTENDED,SYNTAX_VVV
	.extInstruction vswap,    0x0a,0x001a2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0
	.extInstruction vasrr,    0x0a,0x1000039,EXTENDED|SUFFIX_COND|SUFFIX_FLAG|SUFFIX_MASK|FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrr,    0x0a,0x1000039,EXTENDED|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrrie,    0x0a,0x1000039,EXTENDED|SUFFIX_COND|SUFFIX_FLAG|SUFFIX_MASK|FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrrie,    0x0a,0x1000039,EXTENDED|SUFFIX_COND|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_MASK|FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrr,    0x09,0x3,FLAG2_SET|ENCODE_U6,SYNTAX_VV0
	.extInstruction vspib,    0x0a,0x100003b,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA,SYNTAX_VVV|SYNTAX_VVL
	.extInstruction vspib,    0x0a,0x100003b,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vsadbw,   0x0a,0x100003a,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA,SYNTAX_VVV|SYNTAX_VVL
	.extInstruction vsadbw,   0x0a,0x100003a,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vsadbaw,   0x0a,0x200003a,EXTENDED|SUFFIX_COND|SUFFIX_MASK|ENCODE_ZEROA,SYNTAX_VVV|SYNTAX_VVL
	.extInstruction vsadbaw,   0x0a,0x200003a,EXTENDED|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
	.extInstruction vjlt,     0x0a,0x0,EXTENDED|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_VVI|SYNTAX_VV0
	.extInstruction vxminw,   0x0a,0x100001c,SUFFIX_FLAG|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxminw,   0x0a,0x100001c,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxmaxw,   0x0a,0x100001b,SUFFIX_FLAG|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxmaxw,   0x0a,0x100001b,ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND|ENCODE_U8,SYNTAX_VV0
	.extInstruction vxminw,   0x0a,0x100002e,SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vxminw,   0x0a,0x100002e,ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vixminw,   0x0a,0x100002e,SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vixminw,   0x0a,0x100002e,ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vxmaxw,   0x0a,0x100002c,SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vxmaxw,   0x0a,0x100002c,ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vixmaxw,   0x0a,0x100002c,SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vixmaxw,   0x0a,0x100002c,ENCODE_ZEROA|SUFFIX_FLAG|FLAG2_SET|FLAG1_SET|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VV0|SYNTAX_VVI
	.extInstruction vbreak,   0x0a,0x33f2f,EXTEND3|SUFFIX_MASK|FLAG2_SET|ENCODE_SETLM,SYNTAX_0
	.extInstruction vasrlp,   0x0a,0x16,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV|SYNTAX_VVI
	.extInstruction vasrlp,   0x0a,0x16,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV|SYNTAX_VVI
	.extInstruction viasrlp,   0x0a,0x16,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV|SYNTAX_VVI
	.extInstruction viasrlp,   0x0a,0x16,EXTENDED|FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV|SYNTAX_VVI
	.extInstruction vasrlp,   0x0a,0x3e,EXTENDED|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrlp,   0x0a,0x3e,EXTENDED|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrlpi,   0x0a,0x3e,EXTENDED|FLAG2_SET|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,SYNTAX_VV0
	.extInstruction vasrlpi,   0x0a,0x3e,EXTENDED|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|ENCODE_U6,SYNTAX_VV0
	.extInstruction veaccvs32, 0x0a,0x33f2f,EXTEND3|FLAG2_SET|FLAG1_SET|EXTEND2|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction veaccvs32, 0x0a,0x33f2f,EXTEND3|FLAG2_SET|FLAG1_SET|EXTEND2|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV
	.extInstruction vevaccl,   0x0a,0x1000b2f,EXTEND2|SUFFIX_MASK|FLAG1_SET|FLAG2_SET|ENCODE_SETLM,SYNTAX_VV
        .extInstruction veaccvl,   0x0a,0xc2f,EXTEND2|SUFFIX_MASK|SUFFIX_COND|FLAG1_SET|FLAG2_SET,SYNTAX_VV
        .extInstruction vemulfhia, 0x0a,0x2000034,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
        .extInstruction vemulfloa, 0x0a,0x2000033,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
        .extInstruction vbemulfa,  0x0a,0x2000032,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vbemulfa,  0x0a,0x2000032,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
        .extInstruction vemulfa,   0x0a,0x2000031,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
        .extInstruction vemulfhi,  0x0a,0x1000034,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
        .extInstruction vemulflo,  0x0a,0x1000033,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
        .extInstruction vbemulf,   0x0a,0x1000032,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVC
        .extInstruction vbemulf,   0x0a,0x1000032,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVL
        .extInstruction vemulf,    0x0a,0x1000031,EXTENDED|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VVV
        .extInstruction vxmin,    0x0a,0x100003a,EXTENDED|ENCODE_U6|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxmin,    0x0a,0x100003a,EXTENDED|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxmin,    0x0a,0x1000030,EXTENDED|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxmin,    0x0a,0x1000030,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxminw,    0x0a,0x100002e,EXTENDED|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxminw,    0x0a,0x100002e,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxmax,    0x0a,0x100002d,EXTENDED|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxmax,    0x0a,0x100002d,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxmaxw,    0x0a,0x100002c,EXTENDED|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxmaxw,    0x0a,0x100002c,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|FLAG2_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0|SYNTAX_VVI
        .extInstruction vxminw,    0x0a,0x1000039,EXTENDED|ENCODE_U6|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxminw,    0x0a,0x1000039,EXTENDED|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxmax,    0x0a,0x1000038,EXTENDED|ENCODE_U6|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxmax,    0x0a,0x1000038,EXTENDED|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxmaxw,    0x0a,0x1000037,EXTENDED|ENCODE_U6|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
        .extInstruction vxmaxw,    0x0a,0x1000037,EXTENDED|ENCODE_U6|ENCODE_ZEROA|SUFFIX_FLAG|FLAG1_SET|SUFFIX_COND|SUFFIX_MASK,SYNTAX_VV0
	.extInstruction vlsrv,    0x0a,0x1000029,FLAG1_SET|FLAG2_SET|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlsrv,    0x0a,0x1000029,FLAG1_SET|FLAG2_SET|ENCODE_ZEROA|SUFFIX_FLAG|EXTENDED|SUFFIX_MASK|SUFFIX_COND,SYNTAX_VVV
	.extInstruction vlsr,      0x0a,0x1000029,EXTENDED|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|FLAG1_SET|FLAG2_SET,SYNTAX_VVV
	.extInstruction vlsr,      0x0a,0x1000029,EXTENDED|ENCODE_ZEROA|SUFFIX_FLAG|SUFFIX_COND|SUFFIX_MASK|FLAG1_SET|FLAG2_SET,SYNTAX_VVV
	.endif
.endif
