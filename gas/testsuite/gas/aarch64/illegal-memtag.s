func:
	# ADDG/SUBG : Fail uimm6
	addg x1, x2, #0x3ef, #0x6
	subg x1, x2, #0x400, #0x3
	subg x1, x2, -16, #0x3

	# ADDG/SUBG : Fail uimm4
	addg x1, x2, #0x3f0, #0x10
	subg x1, x2, #0x3f0, -4

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
