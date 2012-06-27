	.text
	.globl _start
_start:
	.space 16

	.data
	.globl foo
foo:
	.weak __ehdr_start
	.dc.a __ehdr_start
