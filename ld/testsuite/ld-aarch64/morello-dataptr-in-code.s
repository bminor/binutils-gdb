	.arch morello+crc+c64
	.text
	.p2align 4,,11
	.global	_start
	.type	_start, %function
_start:
	.cfi_startproc purecap
	adrp	c0, var
	add	c0, c0, #:lo12:var
	ldr	w0, [c0]
	ret
	.cfi_endproc
	.size	_start, .-_start
