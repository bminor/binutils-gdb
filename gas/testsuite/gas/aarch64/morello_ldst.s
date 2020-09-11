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
