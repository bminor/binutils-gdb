	.arch morello+crc+c64
	.file	"got.c"
	.text
	.align	2
	.global	get_foo
	.type	get_foo, %function
get_foo:
	adrp	c0, :got:foo
	ldr	c0, [c0, #:got_lo12:foo]
	ret
	.size	get_foo, .-get_foo
	.align	2
	.global	get_bar
	.type	get_bar, %function
get_bar:
	adrp	c0, :got:bar
	ldr	c0, [c0, #:got_lo12:bar]
	ret
	.size	get_bar, .-get_bar
	.align	2
	.global	get_baz
	.type	get_baz, %function
get_baz:
	adrp	c0, :got:baz
	ldr	c0, [c0, #:got_lo12:baz]
	ret
	.size	get_baz, .-get_baz
	.align	2
	.global	_start
	.type	_start, %function
_start:
	ret
	.size	_start, .-_start
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
	.ident	"GCC: (unknown) 11.0.0 20200826 (experimental)"
