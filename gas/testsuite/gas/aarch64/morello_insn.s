// Two operands (dn).

	.macro morello_cspcsp cdsp, cnsp
	  .irp op, cpy, mov
	    \op    \cdsp, \cnsp
	  .endr
	.endm
morello_cspcsp c11, c19
morello_cspcsp c11, csp
morello_cspcsp csp, c11
morello_cspcsp csp, csp

mov	c0, czr
