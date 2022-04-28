	.text
	.global	x
	.bss
	.align	2
	.type	x, %object
	.size	x, 4
x:
	.zero	4
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
.LFB0:
	.cfi_startproc purecap
	adrp	c0, :got:x
	ldr	c0, [c0, #:got_lo12:x]
	ldr	w0, [c0]
	ret
	.cfi_endproc
.LFE0:
	.size	_start, .-_start

