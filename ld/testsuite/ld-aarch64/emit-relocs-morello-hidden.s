	.text
	.align	2
	.global	foo
	.type	foo, %function
foo:
	adrp	c0, :got:hidden_func
	ldr	c0, [c0, #:got_lo12:hidden_func]
	ret
	.size	foo, .-foo
	.align	2
	.global	hidden_func
	.hidden	hidden_func
	.type	hidden_func, %function
hidden_func:
	sub	csp, csp, #16
	str	w0, [csp, 12]
	ldr	w0, [csp, 12]
	add	csp, csp, 16
	ret
	.size	hidden_func, .-hidden_func
