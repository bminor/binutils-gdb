	.arch morello+crc+c64
	.file	"very-basic-test.c"
	.text
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
	adrp	c0, .LC0
	add	c0, c0, :lo12:.LC0
	ldr	c0, [c0]
	ldr	w0, [c0]
	ret
	.size	_start, .-_start
	.section	.data.rel.ro.local,"aw"
	.align	4
	.type	.LC0, %object
	.size	.LC0, 16
.LC0:
	.capinit	retval
	.xword	0
	.xword	0
