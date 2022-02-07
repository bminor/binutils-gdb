	.file	"very-basic-test.c"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	adrp	c0, :got:extobj
	ldr	c0, [c0, #:got_lo12:extobj]
	ldr	w0, [c0]
	ret
	.size	_start, .-_start
