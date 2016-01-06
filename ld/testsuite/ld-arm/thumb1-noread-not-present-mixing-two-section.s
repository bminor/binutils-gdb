	.text
	.section .text.noread
	.arch armv6s-m
	.syntax unified
	.global	_start
	.thumb_func
	.type	_start, %function
_start:
	bx lr

	.text
	.arch armv6s-m
	.syntax unified
	.global	foo
	.thumb_func
	.type	foo, %function
foo:
	bx lr
