	.arch morello+crc+c64
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
.LFB0:
	.cfi_startproc purecap
	adrp	c0, :got:var
	ldr	c0, [c0, #:got_lo12:var]
	str	wzr, [c0]
	ret
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
