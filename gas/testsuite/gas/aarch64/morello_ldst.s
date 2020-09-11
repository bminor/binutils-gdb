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
