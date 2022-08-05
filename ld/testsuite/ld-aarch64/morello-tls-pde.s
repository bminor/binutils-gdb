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
	# A local exec load of a local symbol.
	# Should stay the same.
	mrs	c0, ctpidr_el0
	movz	x1, #:tprel_g1:w
	movk	x1, #:tprel_g0_nc:w
	add	c0, c0, x1
	movz	x1, #:size_g1:w
	movk	x1, #:size_g0_nc:w
	scbnds	c0, c0, x1
	ldr	w0, [c0]
	ret
load_w_ie:
	# An initial exec load of a local variable, should be relaxed to a
	# local-exec access.
	adrp	c0, :gottprel:w
	add	c0, c0, :gottprel_lo12:w
	ldp	x2, x1, [c0]
	mrs	c0, ctpidr_el0
	add	c0, c0, x2
	scbnds	c0, c0, x1
	ldr	w0, [c0]
	ret
load_w1:
	# An initial exec load of some external variable, should stay as
	# initial exec.
	adrp	c0, :gottprel:w1
	add	c0, c0, :gottprel_lo12:w1
	ldp	x2, x1, [c0]
	mrs	c0, ctpidr_el0
	add	c0, c0, x2
	scbnds	c0, c0, x1
	ldr	w0, [c0]
	ret
load_w2:
	# A global dynamic load of some external variable.
	# Should be relaxed to an initial-exec load, since this is the
	# executable and hence we know that this binaries dependencies will be
	# available at load time.
	mrs	c2, ctpidr_el0
	adrp	c0, :tlsdesc:w2
	ldr	c1, [c0, #:tlsdesc_lo12:w2]
	add	c0, c0, :tlsdesc_lo12:w2
	nop
	.tlsdesccall	w2
	blr	c1
	ldr	w0, [c0]
	ret

	.size	_start, .-_start
