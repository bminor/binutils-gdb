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

	.macro morello_csp cnsp
	  .irp op, chksld, chktgd
	    \op    \cnsp
	  .endr
	.endm
morello_csp csp

// Two operands (dn).

	.macro morello_cspcsp cdsp, cnsp
	  .irp op, cpy, mov, chkss
	    \op    \cdsp, \cnsp
	  .endr
	.endm
morello_cspcsp c11, c19
morello_cspcsp c11, csp
morello_cspcsp csp, c11
morello_cspcsp csp, csp

mov	c0, czr

	.macro morello_cx cd, xn
	  .irp op, cvtd, cvtdz, cvtp, cvtpz
	    \op    \cd, \xn
	  .endr
	.endm
morello_cx c0, x1

	.macro morello_xx xd, xn
	  .irp op, rrlen, rrmask
	    \op    \xd, \xn
	  .endr
	.endm
morello_xx x2, x1

	.macro morello_xcsp xd, cnsp
	  .irp op, cfhi, cvtd, cvtp
	    \op    \xd, \cnsp
	  .endr
	.endm
morello_xcsp x7, c15
morello_xcsp x6, csp

	.macro morello_gc_xc xd, cnsp
	  .irp op, gcbase, gcflgs, gclen, gclim, gcoff, gcperm, gcseal, gctag, gctype, gcvalue
	    \op    \xd, \cnsp
	  .endr
	.endm
morello_gc_xc x7, c15
morello_gc_xc x6, csp

chkeq csp, c20
chkeq c19, c20

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
	  .irp op, bicflgs, eorflgs, orrflgs
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

	.macro morello_cspcspi6 cdsp, cnsp
	  .irp op, alignd, alignu
	    \op   \cdsp, \cnsp, #0x3f
	    \op   \cdsp, \cnsp, #0x1e
	    \op   \cdsp, \cnsp, #0
	    \op   \cdsp, \cnsp, #0x20
	  .endr
	.endm
morello_cspcspi6 c17, c16
morello_cspcspi6 csp, c16
morello_cspcspi6 c18, csp
morello_cspcspi6 csp, csp

// Three operands (dnm)

	.macro morello_cspcspx cdsp, cnsp, xm
	  .irp op, bicflgs, eorflgs, orrflgs
	    \op   \cdsp, \cnsp, \xm
	  .endr
	.endm
morello_cspcspx c7, c6, x25
morello_cspcspx c7, csp, x25
morello_cspcspx csp, c6, x25
morello_cspcspx csp, csp, x25

subs x4, c13, c14

	.macro morello_jump_sealed cn, cm
	  .irp op, blrs, brs, rets
	    \op   c29, \cn, \cm
	  .endr
	.endm
morello_jump_sealed c2, c4

	.macro morello_ccc cd, cn, cm
	  .irp op, cpytype, cpyvalue
	    \op    \cd, \cn, \cm
	  .endr
	.endm
morello_ccc c2, c4, c13

	.macro morello_cspcx cdsp, cn, xm
	  .irp op, cthi
	    \op   \cdsp, \cn, \xm
	  .endr
	.endm
morello_cspcx c0, c22, x25
morello_cspcx csp, c4, x25

	.macro morello_ccspx cd, cnsp, xm
	  .irp op, cvt, cvtz
	    \op   \cd, \cnsp, \xm
	  .endr
	.endm
morello_ccspx c22, c0, x25
morello_ccspx c4, csp, x25

	.macro morello_xcspc xd, cnsp, cm
	  .irp op, cvt
	    \op    \xd, \cnsp, \cm
	  .endr
	.endm
morello_xcspc x22, c0, c25
morello_xcspc x4, csp, c25

	.macro morello_ccspcsp cd, cnsp, cmsp
	  .irp op, chkssu
	    \op    \cd, \cnsp, \cmsp
	  .endr
	.endm
morello_ccspcsp c13, c7, c22
morello_ccspcsp c13, csp, c22
morello_ccspcsp c13, c7, csp
morello_ccspcsp c13, csp, csp

	.macro morello_cspcspcsp cdsp, cnsp, cmsp
	  .irp op, build, cseal
	    \op    \cdsp, \cnsp, \cmsp
	  .endr
	.endm
morello_cspcspcsp c2, c4, c13
morello_cspcspcsp csp, c4, c13
morello_cspcspcsp c2, csp, c13
morello_cspcspcsp csp, csp, c13
morello_cspcspcsp c2, c4, csp
morello_cspcspcsp csp, c4, csp
morello_cspcspcsp c2, csp, csp
morello_cspcspcsp csp, csp, csp

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

	.macro morello_csel cd, cn, cm
	  .irp cond, EQ, NE, CS, HS, CC, LO, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL, NV
	    csel   \cd, \cn, \cm, \cond
	  .endr
	.endm
morello_csel c13, c7, c3
