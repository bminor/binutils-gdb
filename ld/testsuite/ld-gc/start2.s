.globl _start
_start:
	.weak	__start__foo
	.dc.a	__start__foo
	.section	_foo,"aw",%progbits
foo:
	.long	1
