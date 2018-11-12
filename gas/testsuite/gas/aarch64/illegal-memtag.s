func:
	# ADDG/SUBG : Fail uimm6
	addg x1, x2, #0x3ef, #0x6
	subg x1, x2, #0x400, #0x3
	subg x1, x2, -16, #0x3

	# ADDG/SUBG : Fail uimm4
	addg x1, x2, #0x3f0, #0x10
	subg x1, x2, #0x3f0, -4

	# STG/STZG/ST2G/LDG : Fail imm
	stg [x1, #15]
	stzg [x1, #-4097]!
	st2g [x1], #4096
	ldg x1, [x2, #33]
	ldg x1, [x2, #4112]

	# STGP : Fail imm
	stgp x1, x2, [x3, #1009]
	stgp x1, x2, [x3, #33]
	stgp x1, x2, [x3, #-1025]

	# LDGV : Warn for Xt == Xn
	# STGV : Sould not warn for above
	ldgv x1, [x1]!
	stgv x1, [x1]!
	# Error for no writeback
	ldgv x1, [x2]
	stgv x1, [x2]

	# Illegal SP/XZR registers
	irg xzr, x2, x3
	irg x1, xzr, x3
	irg x1, x2, sp
	gmi x1, x2, sp
	gmi sp, x2, x3
	gmi x1, xzr, x3
	addg xzr, x2, #0, #0
	subg x1, xzr, #0, #0
	subp sp, x1, x2
	subp x1, xzr, x2
	subp x1, x2, xzr
	subps sp, x1, x2
	subps x1, xzr, x2
	subps x1, x2, xzr
	cmpp xzr, x2
	cmpp x2, xzr
	stg [xzr, #0]
	st2g [xzr, #0]!
	stzg [xzr], #0
	stz2g [xzr, #0]
	stgp sp, x2, [x3]
	stgp x1, sp, [x3]
	stgp x0, x0, [xzr]
	ldg sp, [x0, #16]
	ldg x0, [xzr, #16]
	ldgv sp, [x1]!
	ldgv x0, [xzr]!
	stgv sp, [x1]!
	stgv x0, [xzr]!
