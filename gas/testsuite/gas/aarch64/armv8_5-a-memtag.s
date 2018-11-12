func:
	# OP x[0,30], x[0,30], x[0,30]
	.macro expand_3_reg op
	\op x0, x0, x0
	\op x27, x0, x0
	\op x0, x27, x0
	\op x0, x0, x27
	\op x27, x27, x27
	.endm

	# OP x[0,30], x[0,30], #[0,30], #[0,14]
	.macro expand_2_reg op
	\op x0, x0, #0, #0
	\op x27, x0, #0, #0
	\op x0, x27, #0, #0
	\op x27, x27, #0, #0
	.endm

	# IRG
	expand_3_reg irg
	irg sp, x0
	irg x0, sp

	# GMI
	expand_3_reg gmi
	gmi x0, sp, x0
	gmi xzr, x0, x0

	# ADDG
	expand_2_reg addg
	addg x0, sp, #0x3f0, #0xf
	addg sp, x0, #0x2a0, #0xf

	# SUBG
	expand_2_reg subg
	subg x0, sp, #0x3f0, #0xf
	subg sp, x0, #0x3f0, #0x5
