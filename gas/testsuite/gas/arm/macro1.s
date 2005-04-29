	@ Test that macro expansions are properly scrubbed.
	.macro popret regs
	ldmia sp!, {\regs, pc}
	.endm
	.text
	popret "r4, r5"
