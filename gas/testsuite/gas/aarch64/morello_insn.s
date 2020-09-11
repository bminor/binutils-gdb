// Zero/one operands

bx #4
ret

// Single operand (n)

	.macro morello_jump cn
	  .irp op, blr, blrr, blrs, br, brr, brs, ret, retr, rets
	    \op    \cn
	  .endr
	.endm
morello_jump c20

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

	.macro morello_cspcspi8 cdsp, cnsp
	  .irp op, bicflgs
		\op   \cdsp, \cnsp, #0xff
		\op   \cdsp, \cnsp, #0
		\op   \cdsp, \cnsp, #0xff
		\op   \cdsp, \cnsp, #0x10
	  .endr
	.endm
morello_cspcspi8 c7, c6
morello_cspcspi8 csp, c6
morello_cspcspi8 c8, csp
morello_cspcspi8 csp, csp

// Three operands (dnm)

	.macro morello_cspcspx cdsp, cnsp, xm
	  .irp op, bicflgs
	    \op   \cdsp, \cnsp, \xm
	  .endr
	.endm
morello_cspcspx c7, c6, x25
morello_cspcspx c7, csp, x25
morello_cspcspx csp, c6, x25
morello_cspcspx csp, csp, x25

	.macro morello_jump_sealed cn, cm
	  .irp op, blrs, brs, rets
	    \op   c29, \cn, \cm
	  .endr
	.endm
morello_jump_sealed c2, c4

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
