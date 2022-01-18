	.arch morello+crc+c64
	.file	"very-basic-test.c"
	.text
	.global	retval
	.data
	.align	2
	.type	retval, %object
	.size	retval, 4
retval:
	.word	1
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	adrp	c0, :got:retval
	ldr	c0, [c0, #:got_lo12:retval]
	ldr	w0, [c0]
	ret
	.size	_start, .-_start
