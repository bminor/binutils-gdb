;; arcextlib.s:	Extension library for ARC.
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

;; Driver option -Xtelephony causes all telephony extensions to be activated
	
.ifdef __Xtelephony
    ; ARC4-only options.
    .set   __Xins_tel,1
    .set   __Xxy_tel,1
    .set   __Xxmac_d16_tel,1
    .set   __Xdvbf,1
.endif
	
.ifdef __Xins_tel
	.ifndef __Xbarrel_shifter
            .set   __Xbarrel_shifter,1
        .endif
        .ifndef __Xswap	
            .set           __Xswap,1
        .endif  	
	.ifndef __Xmin_max
            .set   __Xmin_max,1
	.endif
.endif
	
.ifdef __Xdsp_packa
    .set   __Xmul32x16,1
    .ifndef __Xea
            .set   __Xea,1
    .endif
.endif
.ifdef __Xmult32	
        .ifndef __Xmul32x16
	.set __Xmul32x16,1
        .endif
	.ifndef __Xmul32x32
	.set __Xmul32x32,1
        .endif
.endif        
        	
.ifndef __ARCCOMPACT__
        .ifdef __ARC700__
               .set   __ARCCOMPACT__,1
        .endif
        .ifdef __ARC600__
                .set  __ARCCOMPACT__,1
        .endif
.endif        	
	
;;  SWAP Extensions
.ifdef __Xswap
    .ifndef __ARCCOMPACT__
	.ifndef __ARC700__	
		.extInstruction	swap, 0x05, 0x00, SUFFIX_FLAG, SYNTAX_2OP
        .else
                .extInstruction	swap, 0x03, 0x09, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .endif
     .endif		
.endif

;; Normalise Extensions
.ifdef __Xnorm
    .ifdef __ARCCOMPACT__
		.extInstruction	norm , 0x05, 0x01, SUFFIX_FLAG, SYNTAX_2OP
		.extInstruction	normw, 0x05, 0x08, SUFFIX_FLAG, SYNTAX_2OP
                .else
                .extInstruction	norm, 0x03, 0x0A, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
    .endif
.endif

;; Barrel Shifter Extensions
.ifdef __Xbarrel_shifter
    .ifdef __ARCCOMPACT__
	.extInstruction	asl,0x05,0x00,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	lsl,0x05,0x00,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	lsr,0x05,0x01,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	asr,0x05,0x02,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	ror,0x05,0x03,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
    .else
	.extInstruction	asl, 0x10, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction	lsl, 0x10, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction	lsr, 0x11, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction	asr, 0x12, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction	ror, 0x13, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .endif
.endif

;; Min/Max Extensions
.ifdef __Xmin_max
    .ifdef __A4__
	.extInstruction	min, 0x1f, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction	max, 0x1e, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .endif
.endif

;;  Dual and Single Operand Saturated Arithmetic Instructions
.ifdef __Xea
    .ifdef __ARCCOMPACT__
    	.extInstruction	sat16,  0x05, 0x02, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	rnd16,  0x05, 0x03, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	abssw,  0x05, 0x04, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	abss,   0x05, 0x05, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	negsw,  0x05, 0x06, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	negs,   0x05, 0x07, SUFFIX_FLAG, SYNTAX_2OP

        .extInstruction adds,   0x05, 0x06, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction subs,   0x05, 0x07, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    	.extInstruction	divaw,  0x05, 0x08, SUFFIX_COND, SYNTAX_3OP

        .extInstruction	asls,   0x05, 0x0A, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    	.extInstruction	asrs,   0x05, 0x0B, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP

        .extInstruction addsdw, 0x05, 0x28, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction subsdw, 0x05, 0x29, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .else
	.ifndef __A4__
        ;; Single Operand Instructions
        .extInstruction sat,    0x03, 0x10, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction sabss,  0x03, 0x11, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction labss,  0x03, 0x12, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction rnd16,  0x03, 0x13, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction snegs,  0x03, 0x14, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction lnegs,  0x03, 0x15, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction snorm,  0x03, 0x16, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction lnorm,  0x03, 0x17, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP

        .extInstruction adds,   0x1A, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction subs,   0x1B, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction diva,   0x14, 0x00, SUFFIX_COND            , SYNTAX_3OP
        .extInstruction asls,   0x16, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction asrs,   0x17, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.endif
    .endif

    .extCondCode     Ss,    0x10
    .extCondCode     Sc,    0x11

    .extAuxRegister  aux_macmode, 0x41, r|w

.endif

.ifndef __Xno_mpy
    .ifdef __ARC700__	
	.extInstruction mpy,  0x04, 0x1A, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction mpyh,  0x04, 0x1B, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction mpyhu, 0x04, 0x1C, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction mpyu, 0x04, 0x1D, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .endif
.endif

;; 32-bit Multiply Extensions
.ifdef __Xmult32
	.extCoreRegister mlo , 57, r, can_shortcut
	.extCoreRegister r57 , 57, r, can_shortcut
	.extCoreRegister mmid, 58, r, can_shortcut
	.extCoreRegister r58 , 58, r, can_shortcut
	.extCoreRegister mhi , 59, r, can_shortcut
	.extCoreRegister r59 , 59, r, can_shortcut

	.extAuxRegister	 mulhi, 0x12, w

	.ifdef __ARCCOMPACT__

	;; It's important that the longer syntax versions appear after
	;; the shorter syntax versions because internally that's how the
	;; assembler works. 

	        .extInstruction	mul64 , 0x05, 0x04, SUFFIX_COND, SYNTAX_2OP|OP1_IMM_IMPLIED|OP1_DEST_IGNORED
		.extInstruction	mul64 , 0x05, 0x04, SUFFIX_COND, SYNTAX_3OP|OP1_MUST_BE_IMM|OP1_DEST_IGNORED
		.extInstruction	mulu64, 0x05, 0x05, SUFFIX_COND, SYNTAX_2OP|OP1_IMM_IMPLIED|OP1_DEST_IGNORED
		.extInstruction	mulu64, 0x05, 0x05, SUFFIX_COND, SYNTAX_3OP|OP1_MUST_BE_IMM|OP1_DEST_IGNORED

        .else
	.extInstruction	mul64 , 0x14, 0x00,SUFFIX_COND, SYNTAX_2OP|OP1_IMM_IMPLIED
	.extInstruction	mul64 , 0x14, 0x00,SUFFIX_COND, SYNTAX_3OP|OP1_MUST_BE_IMM
	.extInstruction	mulu64, 0x15, 0x00,SUFFIX_COND, SYNTAX_2OP|OP1_IMM_IMPLIED
	.extInstruction	mulu64, 0x15, 0x00,SUFFIX_COND, SYNTAX_3OP|OP1_MUST_BE_IMM
	.endif
.endif
;; 32x16 MUL/MAC from DSP 3.1 Pack-A
.ifdef __Xmul32x16
	.extInstruction	mululw, 0x05,0x30,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	mullw,  0x05,0x31,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	mulflw, 0x05,0x32,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	maclw,  0x05,0x33,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	macflw, 0x05,0x34,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	machulw,0x05,0x35,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	machlw, 0x05,0x36,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	machflw,0x05,0x37,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	mulhlw, 0x05,0x38,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP
	.extInstruction	mulhflw,0x05,0x39,SUFFIX_COND|SUFFIX_FLAG,SYNTAX_3OP

        .extCondCode     Ss,    0x10
        .extCondCode     Sc,    0x11
        .extCondCode     AS,    0x12
        .extCondCode     ASc,   0x13
        .extCondCode     AZ,    0x18
        .extCondCode     AZc,   0x19
        .extCondCode     AN,    0x1A
        .extCondCode     AP,    0x1B
        .extCondCode     PS,    0x1C
        .extCondCode     PSc,   0x1D

        .extAuxRegister	 aux_macmode, 0x41, r|w
	.extAuxRegister  aux_xmac1632h, 0x9f, r|w
	.extAuxRegister  aux_xmac1632l, 0xa0, r|w

        .extCoreRegister acc1,  56, w|r, can_shortcut
        .extCoreRegister acc2,  57, w|r, can_shortcut
.ifndef __Xmult32
        .extCoreRegister r56,  56, w|r, can_shortcut
        .extCoreRegister r57,  57, w|r, can_shortcut
.endif

.endif

;; 2 New condition codes
.ifdef __ARC700__
	
    .extCondCode     qf,    0x1e
    .extCondCode     qnf,   0x1f

.endif
	
;;; arc simd control instructions
.ifdef __ARC700__	
    .extInstruction	chsd,     0x05, 0x3f, SUFFIX_NONE, SYNTAX_3OP
.endif
	
;; double-precision floating point extensions
.ifdef __Xdpfp
	.ifdef __ARCCOMPACT__
        .extInstruction dmulh11,0x06,0x08,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction dmulh12,0x06,0x09,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction dmulh21,0x06,0x0a,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction dmulh22,0x06,0x0b,SUFFIX_FLAG,SYNTAX_3OP

        .extInstruction daddh11,0x06,0x0c,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction daddh12,0x06,0x0d,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction daddh21,0x06,0x0e,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction daddh22,0x06,0x0f,SUFFIX_FLAG,SYNTAX_3OP

        .extInstruction dsubh11,0x06,0x10,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction dsubh12,0x06,0x11,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction dsubh21,0x06,0x12,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction dsubh22,0x06,0x13,SUFFIX_FLAG,SYNTAX_3OP

        .extInstruction drsubh11,0x06,0x14,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction drsubh12,0x06,0x15,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction drsubh21,0x06,0x16,SUFFIX_FLAG,SYNTAX_3OP
        .extInstruction drsubh22,0x06,0x17,SUFFIX_FLAG,SYNTAX_3OP

        .extInstruction dexcl1,0x06,0x18,SUFFIX_NONE,SYNTAX_3OP
        .extInstruction dexcl2,0x06,0x19,SUFFIX_NONE,SYNTAX_3OP

        .extAuxRegister dp_build,0x6c,r
        .extAuxRegister dp_status,0x305,r
	;; compiler uses these instead of the AUX_DPFPDx variety
        .extAuxRegister D1L,0x301,r|w
        .extAuxRegister D1H,0x302,r|w
        .extAuxRegister D2L,0x303,r|w
        .extAuxRegister D2H,0x304,r|w
        .endif
.endif

.ifdef __Xspfp
.ifdef __ARCCOMPACT__
	.extInstruction fadd, 0x6, 0x1, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction fmul, 0x6, 0x0, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.extInstruction fsub, 0x6, 0x2, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
.endif
.endif	

.ifdef __Xxy
	.ifndef __ARCCOMPACT__
	.extCoreRegister x0_nu,   48, r|w, cannot_shortcut
	.extCoreRegister x0_u,    49, r|w, cannot_shortcut
	.extCoreRegister x1_nu,   50, r|w, cannot_shortcut
	.extCoreRegister x1_u,    51, r|w, cannot_shortcut
	.extCoreRegister y0_nu,   52, r|w, cannot_shortcut
	.extCoreRegister y0_u,    53, r|w, cannot_shortcut
	.extCoreRegister y1_nu,   54, r|w, cannot_shortcut
	.extCoreRegister y1_u,    55, r|w, cannot_shortcut

	; Context switching temporary storage registers

	.extCoreRegister t0,      44, r|w, can_shortcut
	.extCoreRegister t1,      45, r|w, can_shortcut
	.extCoreRegister t2,      46, r|w, can_shortcut
	.extCoreRegister t3,      47, r|w, can_shortcut

	; Address and Modifier Register definitions
   
	.extAuxRegister ax0,      0x30, r|w
	.extAuxRegister ax1,      0x31, r|w
	.extAuxRegister ay0,      0x32, r|w
	.extAuxRegister ay1,      0x33, r|w
	.extAuxRegister mx0,      0x34, r|w
	.extAuxRegister mx1,      0x35, r|w
	.extAuxRegister my0,      0x36, r|w
	.extAuxRegister my1,      0x37, r|w

	.extAuxRegister xyconfig, 0x38, r|w
	.extAuxRegister burstsys, 0x3a, r|w
	.extAuxRegister burstxym, 0x3b, r|w
	.extAuxRegister burstsz,  0x3c, r|w
	.extAuxRegister burstval, 0x3d, r|w
                ;; 	add_check xy, 2
	.endif
.endif
	
.ifdef __Xxy	
        .ifdef __ARCCOMPACT__
;;	XY Memory extension (DSP version 3)
	.extCoreRegister x0_u0, 32,  r|w, cannot_shortcut,CORE
	.extCoreRegister x0_u1, 33,  r|w, cannot_shortcut,CORE
	.extCoreRegister x1_u0, 34,  r|w, cannot_shortcut,CORE
	.extCoreRegister x1_u1, 35,  r|w, cannot_shortcut,CORE
	.extCoreRegister x2_u0, 36,  r|w, cannot_shortcut,CORE
	.extCoreRegister x2_u1, 37,  r|w, cannot_shortcut,CORE
	.extCoreRegister x3_u0, 38,  r|w, cannot_shortcut,CORE
	.extCoreRegister x3_u1, 39,  r|w, cannot_shortcut,CORE

	.extCoreRegister y0_u0, 40,  r|w, cannot_shortcut,CORE
	.extCoreRegister y0_u1, 41,  r|w, cannot_shortcut,CORE
	.extCoreRegister y1_u0, 42,  r|w, cannot_shortcut,CORE
	.extCoreRegister y1_u1, 43,  r|w, cannot_shortcut,CORE
	.extCoreRegister y2_u0, 44,  r|w, cannot_shortcut,CORE
	.extCoreRegister y2_u1, 45,  r|w, cannot_shortcut,CORE
	.extCoreRegister y3_u0, 46,  r|w, cannot_shortcut,CORE
	.extCoreRegister y3_u1, 47,  r|w, cannot_shortcut,CORE

	.extCoreRegister x0_nu, 48,  r|w, cannot_shortcut,CORE
	.extCoreRegister x1_nu, 49,  r|w, cannot_shortcut,CORE
	.extCoreRegister x2_nu, 50,  r|w, cannot_shortcut,CORE
	.extCoreRegister x3_nu, 51,  r|w, cannot_shortcut,CORE
	.extCoreRegister y0_nu, 52,  r|w, cannot_shortcut,CORE
	.extCoreRegister y1_nu, 53,  r|w, cannot_shortcut,CORE
	.extCoreRegister y2_nu, 54,  r|w, cannot_shortcut,CORE
	.extCoreRegister y3_nu, 55,  r|w, cannot_shortcut,CORE

    ;Newly defined pointers  in Auxiliary memory space
	.extAuxRegister ax0,  0x80, r|w
	.extAuxRegister ax1,  0x81, r|w
	.extAuxRegister ax2,  0x82, r|w
	.extAuxRegister ax3,  0x83, r|w
	.extAuxRegister ay0,  0x84, r|w
	.extAuxRegister ay1,  0x85, r|w
	.extAuxRegister ay2,  0x86, r|w
	.extAuxRegister ay3,  0x87, r|w

	.extAuxRegister mx00, 0x88, r|w
	.extAuxRegister mx01, 0x89, r|w
	.extAuxRegister mx10, 0x8a, r|w
	.extAuxRegister mx11, 0x8b, r|w
	.extAuxRegister mx20, 0x8c, r|w
	.extAuxRegister mx21, 0x8d, r|w
	.extAuxRegister mx30, 0x8e, r|w
	.extAuxRegister mx31, 0x8f, r|w

	.extAuxRegister my00, 0x90, r|w
	.extAuxRegister my01, 0x91, r|w
	.extAuxRegister my10, 0x92, r|w
	.extAuxRegister my11, 0x93, r|w
	.extAuxRegister my20, 0x94, r|w
	.extAuxRegister my21, 0x95, r|w
	.extAuxRegister my30, 0x96, r|w
	.extAuxRegister my31, 0x97, r|w

	.extAuxRegister xyconfig, 0x98, r|w
	.extAuxRegister burstsys, 0x99, r|w
	.extAuxRegister burstxym, 0x9a, r|w
	.extAuxRegister burstsz,  0x9b, r|w
	.extAuxRegister burstval, 0x9c, r|w
	.extAuxRegister xylsbasex,0x9d, r|w
	.extAuxRegister xylsbasey,0x9e, r|w
        .endif
.endif
	

	
.ifdef __Xxmac_d16
	.ifndef __Xxmac
                .set  __Xxmac,1
        .endif
.endif        
.ifdef __Xxmac_24
	.ifndef _Xxmac
                .set  __Xxmac,1
	.endif
.endif        
.ifdef __Xmult32
	.ifndef __Xxmac
	.set  __Xxmac,1
        .endif          
	.ifndef __Xxmac_d16
	.set __Xxmac_d16,1
        .endif
.endif        	

.ifdef __Xxmac	
.ifdef __ARCCOMPACT__
        .extCoreRegister a1,  58, w|r, cannot_shortcut, CORE
        .extCoreRegister a2,  59, w|r, cannot_shortcut, CORE
        .extAuxRegister aux_macmode, 0x41, r|w

        .extCondCode     Ss,    0x10
        .extCondCode     Sc,    0x11
        .extCondCode     AS,    0x12
        .extCondCode     ASc,   0x13
        .extCondCode     AZ,    0x18
        .extCondCode     AZc,   0x19
        .extCondCode     AN,    0x1A
        .extCondCode     AP,    0x1B

	.ifdef __Xxmac_d16
        .extInstruction muldw,  0x05, 0x0c, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction muludw, 0x05, 0x0d, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction mulrdw, 0x05, 0x0e, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction macdw,  0x05, 0x10, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction macudw, 0x05, 0x11, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction macrdw, 0x05, 0x12, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction msubdw, 0x05, 0x14, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction cmacrdw,0x05, 0x26, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.endif
	
        .ifndef __ARC700__
	    ;; FFT butterfly accelerator (not supported for ARC 700)
	    .extInstruction fbfdw,  0x05, 0x0b, SUFFIX_FLAG, SYNTAX_2OP
	.endif

	.ifdef __Xxmac_24
        .extInstruction mult,   0x05, 0x18, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction mulut,  0x05, 0x19, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction mulrt,  0x05, 0x1a, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction mact,   0x05, 0x1c, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction macrt,  0x05, 0x1e, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction msubt,  0x05, 0x20, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.endif
    .else
	.ifndef __A4__
        .extInstruction msubnr, 0x15, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction macnr,  0x18, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction macunr, 0x19, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction mulnr,  0x1C, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction mulr,   0x1D, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.endif
                ;; 	add_check xmac_d16, 3
    .endif

    .ifdef __ARCCOMPACT__
        .extCoreRegister acc1,  56, w|r, can_shortcut, CORE
        .extCoreRegister acc2,  57, w|r, can_shortcut, CORE
    .else
        .extCoreRegister acc0,  56, w|r, can_shortcut, CORE
        .extCoreRegister acc1,  57, w|r, can_shortcut, CORE
    .endif

        .extAuxRegister aux_macmode, 0x41, r|w
        .extAuxRegister aux_xmac0,   0x44, r|w
        .extAuxRegister aux_xmac1,   0x45, r|w
        .extAuxRegister aux_xmac2,   0x46, r|w
	.ifdef __Xxmac_24
        .extAuxRegister aux_xmac0_24,   0x2c, r|w
        .extAuxRegister aux_xmac1_24,   0x2d, r|w
        .extAuxRegister aux_xmac2_24,   0x2e, r|w
	.endif
    .ifdef __ARCCOMPACT__
	.ifndef __ARC700__
	    .extAuxRegister aux_fbf_store_16, 0x2f,  r|w
	.endif
    .endif

        .extCondCode     Ss,    0x10
        .extCondCode     Sc,    0x11
        .extCondCode     AS,    0x12
        .extCondCode     ASc,   0x13
        .extCondCode     AZ,    0x18
        .extCondCode     AZc,   0x19
        .extCondCode     AN,    0x1A
        .extCondCode     AP,    0x1B
        .extCondCode     PS,    0x1C
        .extCondCode     PSc,   0x1D
.endif

;;  Dual and Single Operand Instructions for Telephony
.ifdef __Xea
    .ifdef __ARCCOMPACT__
    	.extInstruction	sat16,  0x05, 0x02, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	rnd16,  0x05, 0x03, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	abssw,  0x05, 0x04, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	abss,   0x05, 0x05, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	negsw,  0x05, 0x06, SUFFIX_FLAG, SYNTAX_2OP
    	.extInstruction	negs,   0x05, 0x07, SUFFIX_FLAG, SYNTAX_2OP

        .extInstruction adds,   0x05, 0x06, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction subs,   0x05, 0x07, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    	.extInstruction	divaw,  0x05, 0x08, SUFFIX_COND, SYNTAX_3OP
	.ifdef __Xbarrel_shifter
        .extInstruction	asls,   0x05, 0x0A, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    	.extInstruction	asrs,   0x05, 0x0B, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.endif
        .extInstruction addsdw, 0x05, 0x28, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction subsdw, 0x05, 0x29, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .else
	.ifndef __A4__
        ;; Single Operand Instructions
        .extInstruction sat,    0x03, 0x10, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction sabss,  0x03, 0x11, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction labss,  0x03, 0x12, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction rnd16,  0x03, 0x13, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction snegs,  0x03, 0x14, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction lnegs,  0x03, 0x15, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction snorm,  0x03, 0x16, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
        .extInstruction lnorm,  0x03, 0x17, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP

        .extInstruction adds,   0x1A, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction subs,   0x1B, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction diva,   0x14, 0x00, SUFFIX_COND, SYNTAX_3OP
        .extInstruction asls,   0x16, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extInstruction asrs,   0x17, 0x00, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
	.endif
    .endif

    .extCondCode     Ss,    0x10
    .extCondCode     Sc,    0x11

    ; no BCR check yet possible for Extended Arithmetic.
.endif

;; DSP 3.1 Pack A extensions
.ifdef __Xdsp_packa
    .ifndef __A4__
    ; Dual min/max extension instruction declarations:
    .extInstruction minidl,    0x05, 0x09, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .extInstruction maxidl,    0x05, 0x0f, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .extAuxRegister aux_valm2,      0xa4, r|w
    .extAuxRegister aux_valm1,      0xa5, r|w
    .extAuxRegister aux_count,      0xa6, r|w
    .extAuxRegister aux_countm,     0xa7, r|w

    ; Dual barrel-shifter extension instruction declarations:
    .extInstruction asldw,     0x05, 0x21, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .extInstruction asrdw,     0x05, 0x22, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .extInstruction lsrdw,     0x05, 0x23, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .extInstruction aslsdw,    0x05, 0x24, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .extInstruction asrsdw,    0x05, 0x25, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP

    ; Dual maxabs extension instruction declarations:
    .extInstruction maxabssdw, 0x05, 0x2b, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
    .endif
.endif

;; Dual viterbi butterfly extension
.ifdef __Xdvbf
    .ifdef __ARCCOMPACT__
        .extInstruction vbfdw, 0x05, 0x0A, SUFFIX_FLAG, SYNTAX_2OP
    .else
	.ifndef __A4__
        .extInstruction	dvbf, 0x03, 0x18, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_2OP
	.endif
    .endif
        .ifndef __A4__
	; these are the names for telephony and A5 DSP ver 3.0
        .extAuxRegister aux_dvbf_mode,    0x26,  r|w
        .extAuxRegister aux_dvbf_bm0,     0x27,  r|w
        .extAuxRegister aux_dvbf_bm1,     0x28,  r|w
        .extAuxRegister aux_dvbf_accu,    0x29,  r|w
        .extAuxRegister aux_dvbf_ofst,    0x2A,  r|w
        .extAuxRegister aux_dvbf_intstat, 0x2B,  r|w

	; these are the names for A5 DSP ver 3.1 and later
        .extAuxRegister aux_vbfdw_mode,    0x26,  r|w
        .extAuxRegister aux_vbfdw_bm0,     0x27,  r|w
        .extAuxRegister aux_vbfdw_bm1,     0x28,  r|w
        .extAuxRegister aux_vbfdw_accu,    0x29,  r|w
        .extAuxRegister aux_vbfdw_ofst,    0x2A,  r|w
        .extAuxRegister aux_vbfdw_intstat, 0x2B,  r|w

	; No known reliable BCR check for vbfdw
	.endif
.endif

;; variable polynomial CRC
.ifdef __Xcrc
    .ifdef __ARCCOMPACT__
	.extInstruction crc, 0x05, 0x2c, SUFFIX_COND|SUFFIX_FLAG, SYNTAX_3OP
        .extAuxRegister aux_crc_poly, 0x32,  r|w
        .extAuxRegister aux_crc_mode, 0x33,  r|w
	; No known BCR check for CRC.
    .endif
.endif
