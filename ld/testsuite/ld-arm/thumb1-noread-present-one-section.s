	.text
	.section .text.fetchonly,"axy",%progbits
	.arch armv6s-m
	.syntax unified
	.global	_start
	.thumb_func
	.type	_start, %function
_start:
	bx lr
