	.section	.tbss,"awT",@nobits
	.align	2
	.type	w, %object
	.size	w, 4
w:
	.zero	4
	.align	2
	.global exec_sym
	.type	exec_sym, %object
	.size	exec_sym, 4
exec_sym:
	.zero	4

	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	ret
load_w:
	# A local exec load of a symbol defined in a different shared library
	# should be an error.
	mrs	c0, ctpidr_el0
	movz	x1, #:tprel_g1:w1
	movk	x1, #:tprel_g0_nc:w1
	add	c0, c0, x1
	movz	x1, #:size_g1:w1
	movk	x1, #:size_g0_nc:w1
	scbnds	c0, c0, x1
	ldr	w0, [c0]
	ret
	.size	_start, .-_start

