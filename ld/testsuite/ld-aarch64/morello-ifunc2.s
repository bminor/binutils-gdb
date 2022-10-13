// Want to check that having a CAPINIT relocation on an IFUNC stops the garbage
// collection of the IFUNC's PLT (so that the CAPINIT relocation has something
// to point to).
#APP
	.type foo, %gnu_indirect_function
#NO_APP
	.align	2
	.type	foo_1, %function
foo_1:
	mov	w0, 1
	ret
	.size	foo_1, .-foo_1
	.align	2
	.type	foo_2, %function
foo_2:
	mov	w0, 2
	ret
	.size	foo_2, .-foo_2
	.align	2

	.global	foo
	.type	foo, %function
foo:
	mov	x1, 42
	tst	x0, x1
	bne	.L5
	adrp	c0, foo_1
	add	c0, c0, :lo12:foo_1
.L4:
	ret
.L5:
	adrp	c0, foo_1
	add	c0, c0, :lo12:foo_1
	b	.L4
	.size	foo, .-foo
	.align	2

	.global	_start
	.type	_start, %function
_start:
	ret
	.size	_start, .-_start

	.data
	.align	4
	.chericap foo
