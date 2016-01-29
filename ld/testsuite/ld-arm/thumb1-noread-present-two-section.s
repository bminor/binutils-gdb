	.text
	.section .text.fetchonly.first,"axy",%progbits
	.arch armv6s-m
	.syntax unified
	.global	_start
	.thumb_func
	.type	_start, %function
_start:
	bx lr

	.text
	.section .text.fetchonly.second,"axy",%progbits
	.arch armv6s-m
	.syntax unified
	.global	foo
	.thumb_func
	.type	foo, %function
foo:
	bx lr
