	.arch morello+crc+c64
	.file	"global.c"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	adrp	c0, :got:ptr1
	ldr	c0, [c0, #:got_lo12:ptr1]
	ldr	c2, [c0]
	adrp	c0, :got:ptr2
	ldr	c0, [c0, #:got_lo12:ptr2]
	ldr	c6, [c0]
	adrp	c0, :got:ptr3
	ldr	c0, [c0, #:got_lo12:ptr3]
	ldr	c4, [c0]
	ldr	w0, [c6]
	ldr	w1, [c4]
	add	w0, w0, w1
	str	w0, [c2]
	ret
	.size	_start, .-_start
	.global	ptr3
	.global	ptr2
	.global	ptr1
	.global	baz
	.global	bar
	.global	foo
	.bss
	.align	2
	.type	baz, %object
baz:
	.zero	4
	.type	bar, %object
bar:
	.zero	4
	.type	foo, %object
foo:
	.zero	4
	.section	.data.rel.local,"aw"
	.align	4
	.type	ptr3, %object
	.size	ptr3, 16
ptr3:
	.capinit	baz
	.xword	0
	.xword	0
	.type	ptr2, %object
	.size	ptr2, 16
ptr2:
	.capinit	bar
	.xword	0
	.xword	0
	.type	ptr1, %object
	.size	ptr1, 16
ptr1:
	.capinit	foo
	.xword	0
	.xword	0
	.ident	"GCC: (unknown) 11.0.0 20200826 (experimental)"
