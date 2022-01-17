	.arch morello+crc+c64
	.file	"t.c"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
.LFB0:
	.cfi_startproc
	stp	c29, c30, [csp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -16
	mov	c29, csp
	mrs	c2, ctpidr_el0
	nop
	adrp	c0, :tlsdesc:x
	ldr	c1, [c0, #:tlsdesc_lo12:x]
	add	c0, c0, :tlsdesc_lo12:x
	.tlsdesccall	x
	blr	c1
	scbnds	c0, c0, x1
	mov	w1, 1
	str	w1, [c0]
	nop
	ldp	c29, c30, [csp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
	.weak	x
	.ident	"GCC: (GNU) 11.0.0 20200826 (experimental)"
	.section	.note.GNU-stack,"",@progbits
