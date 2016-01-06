	.text
	.section .text.noread.first
	.arch armv6s-m
	.syntax unified
	.global	_start
	.thumb_func
	.type	_start, %function
_start:
	bx lr

	.text
	.section .text.noread.second
	.arch armv6s-m
	.syntax unified
	.global	foo
	.thumb_func
	.type	foo, %function
foo:
	bx lr
