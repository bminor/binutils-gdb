	.text
	.p2align 4,,11
	.global	_start
	.type	_start, %function
_start:
	.cfi_startproc purecap
	adrp	c0, memcpy
	add	c0, c0, #:lo12:memcpy
	ldr	w0, [c0]
	ret
	.cfi_endproc
	.size	_start, .-_start

