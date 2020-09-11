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

// Three operands (dni)

	.macro morello_addsub_imm cdsp, cnsp
	  .irp op, add, sub
		\op   \cdsp, \cnsp, #0xff0
		\op   \cdsp, \cnsp, #0xfff, lsl #0
		\op   \cdsp, \cnsp, #0x1fe, lsl #12
		\op   \cdsp, \cnsp, #0x3, lsl #0
		\op   \cdsp, \cnsp, #0x0, lsl #12
	  .endr
	.endm
morello_addsub_imm c21, c9
morello_addsub_imm csp, c9
morello_addsub_imm csp, csp
morello_addsub_imm c21, csp

// Four operands (dnmi)

	.macro morello_add_scalar cspd, cspn, rm
	  .irp op, add
	    \op \cspd, \cspn, x\rm, sxtx #0
	    \op \cspd, \cspn, x\rm, sxtx #4
	    \op \cspd, \cspn, w\rm, sxtw #4
	    \op \cspd, \cspn, x\rm
	    \op \cspd, \cspn, x\rm, lsl #4
	  .endr
	.endm
morello_add_scalar c17, c9, 4
morello_add_scalar csp, c9, 4
morello_add_scalar c17, csp, 4
morello_add_scalar csp, csp, 4
