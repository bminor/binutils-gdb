	.ifdef C64MODE
	SP_ .req csp
	VAREG .req c24
	ALTSP .req sp
	ALTVAREG .req x12
	.else
	SP_ .req sp
	VAREG .req x11
	ALTSP .req csp
	ALTVAREG .req c21
	.endif

// Base Register
	.macro morello_cas cs, ct, xnsp
	  .irp op, casa casal, cas, casl
	    \op    \cs, \ct, [\xnsp]
	  .endr
	.endm
morello_cas c4, c2, VAREG
morello_cas c4, c2, SP_

	.macro morello_swp cs, ct, xnsp
	  .irp op, swpa, swpal, swp, swpl
	    \op    \cs, \ct, [\xnsp]
	  .endr
	.endm
morello_swp c4, c2, VAREG
morello_swp c4, c2, SP_

	.macro morello_ldst_base ct, xnsp
	  .irp op, ldapr, ldar, ldaxr, ldxr, stlr
	    \op    \ct, [\xnsp]
	  .endr
	.endm
morello_ldst_base c2, VAREG
morello_ldst_base c2, SP_

	.macro morello_ldst_basex xt, xnsp
	  .irp op, ldct, stct
	    \op    \xt, [\xnsp]
	  .endr
	.endm
morello_ldst_basex x2, VAREG
morello_ldst_basex x2, SP_

	.macro morello_ldp_base ct1, ct2, xnsp
	  .irp op, ldaxp, ldxp
	    \op    \ct1, \ct2, [\xnsp]
	  .endr
	.endm
morello_ldp_base c2, c3, VAREG
morello_ldp_base c2, c5, SP_

       .macro morello_str_base ct, xnsp
	  .irp op, stlxr, stxr
	    \op    w2, \ct, [\xnsp]
	  .endr
	.endm
morello_str_base c1, VAREG
morello_str_base c29, SP_

	.macro morello_stp_base ct1, ct2, xnsp
	  .irp op, stlxp, stxp
	    \op    w2, \ct1, \ct2, [\xnsp]
	  .endr
	.endm
morello_stp_base c4, c5, VAREG
morello_stp_base c4, c29, SP_

// Base + immediate offset

	.macro morello_uimm ct, xnsp
	  .irp op, ldr, str
	    \op    \ct, [\xnsp, #65520]
	    \op    \ct, [\xnsp, #32]
	    \op    \ct, [\xnsp, #0]
	  .endr
	.endm
morello_uimm c4, VAREG
morello_uimm c4, SP_

	.macro morello_simm ct, xnsp
	  .irp op, ldur, stur
	    \op    \ct, [\xnsp, #255]
	    \op    \ct, [\xnsp, #-255]
	    \op    \ct, [\xnsp, #0]
	    \op    \ct, [\xnsp, #16]
	  .endr
	.endm
morello_simm c4, VAREG
morello_simm c4, SP_

	.macro morello_simm_scale ct, xnsp
	  .irp op, ldtr, sttr
	    \op    \ct, [\xnsp, #4080]
	    \op    \ct, [\xnsp, #-4096]
	    \op    \ct, [\xnsp, #0]
	    \op    \ct, [\xnsp, #16]
	  .endr
	.endm
morello_simm_scale c4, VAREG
morello_simm_scale c4, SP_

	.macro morello_simm_pair ct, ct2, xnsp
	  .irp op, ldp, stp, ldnp, stnp
	    \op    \ct, \ct2, [\xnsp, #1008]
	    \op    \ct, \ct2, [\xnsp, #-1024]
	    \op    \ct, \ct2, [\xnsp, #16]
	    \op    \ct, \ct2, [\xnsp, #-32]
	  .endr
	.endm
morello_simm_pair c4, c5, VAREG
morello_simm_pair c4, c6, SP_

// Indexed

	.macro morello_index ct, xnsp
	  .irp op, ldr, str
	    \op    \ct, [\xnsp, #32]!
	    \op    \ct, [\xnsp], #32
	    \op    \ct, [\xnsp, #-4096]!
	    \op    \ct, [\xnsp], #-4096
	    \op    \ct, [\xnsp, #4080]!
	    \op    \ct, [\xnsp], #4080
	  .endr
	.endm
morello_index c4, VAREG
morello_index c4, SP_

	.macro morello_index_pair ct, ct2, xnsp
	  .irp op, ldp, stp
	    \op    \ct, \ct2, [\xnsp, #32]!
	    \op    \ct, \ct2, [\xnsp], #32
	    \op    \ct, \ct2, [\xnsp, #-1024]!
	    \op    \ct, \ct2, [\xnsp], #-1024
	    \op    \ct, \ct2, [\xnsp, #1008]!
	    \op    \ct, \ct2, [\xnsp], #1008
	  .endr
	.endm
morello_index_pair c3, c4, VAREG
morello_index_pair c4, c5, SP_

// Register offset.

	.macro morello_regoff ct, xnsp, off
	  .irp op, ldr, str
	    \op    \ct, [\xnsp, x\off]
	    \op    \ct, [\xnsp, x\off, lsl #0]
	    \op    \ct, [\xnsp, w\off, uxtw #4]
	    \op    \ct, [\xnsp, w\off, sxtw #4]
	    \op    \ct, [\xnsp, w\off, sxtw]
	  .endr
	.endm
morello_regoff c4, VAREG, 3
morello_regoff c4, SP_, 3

// Branch and Load, Prefetch, etc.

	.macro morello_branch_load cnsp
	  .irp op, blr, br
	    \op    [\cnsp, #-1024]
	    \op    [\cnsp, #1008]
	    \op    [\cnsp, #16]
	    \op    [\cnsp]
	  .endr
	.endm
morello_branch_load csp
morello_branch_load c2

	.macro morello_branch_load2 cnsp
	  .irp op, ldpblr, ldpbr
	    \op    c2, [\cnsp]
	  .endr
	.endm
morello_branch_load2 csp
morello_branch_load2 c2

	.macro morello_prfm xnsp
	  .irp op, prfum
	    \op    PLDL1KEEP, [\xnsp, #255]
	    \op    PSTL2STRM, [\xnsp, #-255]
	    \op    PLDL2STRM, [\xnsp, #0]
	    \op    PSTL1KEEP, [\xnsp, #16]
	  .endr
	.endm
morello_prfm VAREG
morello_prfm SP_

// Cache maintenance

	.macro morello_dc ct
	  .irp op, ivac, isw, csw, cisw, zva, cvac, cvau, cvap, civac
	    dc \op, \ct
	  .endr
	.endm
morello_dc VAREG

	.macro morello_ic ct
	  .irp op, ivau
	    ic \op, \ct
	  .endr
	.endm
morello_ic VAREG

// Alternate base loads and stores.

	.macro morello_ldst_alt_base ct, cnsp
	  .irp op,ldar, stlr
	    \op    \ct, [\cnsp]
	  .endr
	.endm
morello_ldst_alt_base c2, ALTVAREG
morello_ldst_alt_base c2, ALTSP

	.macro morello_ldst_alt_basew wt, cnsp
	  .irp op, ldarb, ldar, stlrb, stlr
	    \op    \wt, [\cnsp]
	  .endr
	.endm
morello_ldst_alt_basew w2, ALTVAREG
morello_ldst_alt_basew w2, ALTSP

	.macro morello_alt_uimm rt, cnsp, max
	  .irp op, ldr, str
	    \op    \rt, [\cnsp, \max]
	    \op    \rt, [\cnsp, #32]
	    \op    \rt, [\cnsp, #0]
	  .endr
	.endm
morello_alt_uimm c4, ALTVAREG, #8176
morello_alt_uimm c4, ALTSP, #8176
morello_alt_uimm x4, ALTVAREG, #4088
morello_alt_uimm x4, ALTSP, #4088
morello_alt_uimm w4, ALTVAREG, #2044
morello_alt_uimm w4, ALTSP, #2044

	.macro morello_alt_imm cnsp
	  .irp op, ldur, stur
	    .irp rt, c4, x4, w4, b4, h4, s4, d4, q4
	      \op    \rt, [\cnsp, #255]
	      \op    \rt, [\cnsp, #32]
	      \op    \rt, [\cnsp, #-256]
	    .endr
	  .endr
	.endm
morello_alt_imm ALTVAREG
morello_alt_imm ALTSP

	.macro morello_alt_uimmw wt, cnsp
	  .irp op, ldrb, strb
	    \op    \wt, [\cnsp, #511]
	    \op    \wt, [\cnsp, #32]
	    \op    \wt, [\cnsp, #0]
	  .endr
	.endm
morello_alt_uimmw w4, ALTVAREG
morello_alt_uimmw w4, ALTSP

// Substitute LDR/STR with negative immediate with LDUR/STUR.
	.macro morello_uimm_sub xnsp
	  .irp op, ldr, str
	    \op    c4, [\xnsp, #-16]
	    \op    c4, [\xnsp, #-256]
	  .endr
	.endm
morello_uimm_sub VAREG
morello_uimm_sub SP_

	.macro morello_alt_uimm_sub xnsp
	  .irp op, ldr, str
	    .irp reg, c4, x4, w4
	      \op    \reg, [\xnsp, #-16]
	      \op    \reg, [\xnsp, #-256]
	    .endr
	  .endr
	.endm
morello_alt_uimm_sub ALTVAREG
morello_alt_uimm_sub ALTSP

	.macro morello_alt_narrow_simm9_inner op, size, wt, xnsp
	\op\size    \wt, [\xnsp]
	\op\size    \wt, [\xnsp, #-1]
	\op\size    \wt, [\xnsp, #-16]
	\op\size    \wt, [\xnsp, #-256]
	\op\size    \wt, [\xnsp, #1]
	\op\size    \wt, [\xnsp, #16]
	\op\size    \wt, [\xnsp, #255]
	.endm

	.macro morello_alt_narrow_simm9 xnsp
	.irp op, ldr, ldrs, str, ldur, ldurs, stur
	  morello_alt_narrow_simm9_inner \op, b, w4, \xnsp
	  morello_alt_narrow_simm9_inner \op, h, w4, \xnsp
	.endr
	.irp size, b, h, w
	  morello_alt_narrow_simm9_inner ldrs, \size, x4, \xnsp
	  morello_alt_narrow_simm9_inner ldurs, \size, x4, \xnsp
	.endr
	.endm

morello_alt_narrow_simm9 ALTVAREG
morello_alt_narrow_simm9 ALTSP

	.macro morello_alt_zero_basic, op, reg
	  \op \reg, [ALTVAREG]
	  \op \reg, [ALTSP]
	.endm

	.macro morello_alt_zero_general, op, reg
	  \op \reg, [ALTVAREG]
	  \op \reg, [ALTVAREG, #16]
	  \op \reg, [ALTSP, x10]
	.endm

	.macro morello_alt_zero_unscaled, op, reg
	  \op \reg, [ALTVAREG]
	  \op \reg, [ALTVAREG, #16]
	  \op \reg, [ALTSP, #1]
	.endm

	.irp op, ldar, stlr, ldarb, stlrb
	  morello_alt_zero_basic \op, wzr
	.endr

	.irp op, ldr, str
	  morello_alt_zero_general \op, wzr
	  morello_alt_zero_general \op, xzr
	.endr

	.irp op, ldur, stur
	  morello_alt_zero_unscaled \op, wzr
	  morello_alt_zero_unscaled \op, xzr
	.endr

	.irp size, b, h
	  .irp op, ldr\size, ldrs\size, str\size
	    morello_alt_zero_general \op, wzr
	  .endr
	  morello_alt_zero_general ldrs\size, xzr
	  .irp op, ldur\size, ldurs\size, stur\size
	    morello_alt_zero_unscaled \op, wzr
	  .endr
	  morello_alt_zero_unscaled ldurs\size, xzr
	.endr
