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
