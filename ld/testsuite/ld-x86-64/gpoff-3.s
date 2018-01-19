	.text
	.globl _start
_start:
	movl	%gs:foo@GPOFF, %eax
