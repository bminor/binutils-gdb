// Want to check that having a CAPINIT relocation on an IFUNC does not require
// pointer equality (this would mean that a GOT relocation against the symbol
// would end up using a GOT entry which refers to the PLT, but without
// requiring pointer equality we end up using the PLTGOT entry that is already
// around for the use of the PLT).
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
	bl	foo
	adrp	c0, :got:foo
	ldr	c0, [c0, #:got_lo12:foo]
	ret
	.size	_start, .-_start

	.data
	.align	4
	.chericap foo
