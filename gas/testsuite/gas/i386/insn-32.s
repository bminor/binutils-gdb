	.text
insn:
	# nop
	.insn 0x90

	# pause
	.insn 0xf390
	.insn repe 0x90

	# fldz
	.insn 0xd9ee

	# setssbsy
	.insn 0xf30f01e8

	# vzeroall
	.insn VEX.256.0F.WIG 0x77
	.insn {vex3} VEX.L1 0x0f77
