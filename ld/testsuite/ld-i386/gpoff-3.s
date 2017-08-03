	.text
	.globl _start
_start:
	movl	%fs:foo@GPOFF, %eax
