	.text
	.globl _start
_start:
	movl	%fs:foo@GPOFF, %eax
	movl   __gp@GOT(%ebx), %eax

	.data
	.globl foo
foo:
	.long 0x12345678
