# Testcase for a variety of ld st instructions
	.text
	.align  2
	.global foo
	.type   foo, %function
foo:
# ldstpair_indexed
	stp     wzr, wzr, [sp, 52]!
	stp     x0, x1, [sp, 32]!
	stp     d8, d9, [sp, -64]!
# Following is skipped from ginsn generation
	stp     q0, q1, [sp, 64]
# ldstpair_off
	stp     wzr, wzr, [sp, 52]
	ldp     q31, q30, [x0]
	ldp     d8,  d9,  [sp], 64
# ldst_imm9
	ldrb    w7, [sp, 255]!
	ldr     q29, [sp, 44]!
	ret
	.size   foo, .-foo
