# Testcase for a variety of arith instructions
	.text
	.align  2
	.global foo
	.type   foo, %function
foo:
	add     sp, sp, 32
	adds    x29, sp, 2134
	sub     sp, sp, 32
	subs    x29, sp, 2134
	add     x29, sp, x2
	sub     x29, sp, x3
	add     sp, x29, x3
	sub     sp, x29, x2
	ret
	.size   foo, .-foo
