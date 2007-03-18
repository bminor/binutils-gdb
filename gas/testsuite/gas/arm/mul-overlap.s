	.text
	.align	2
	.global	foo
	.type	foo, %function
foo:
	mul	r0, r0, r0
	mla	r0, r0, r1, r2
	mls	r0, r0, r1, r2
	bx	lr
