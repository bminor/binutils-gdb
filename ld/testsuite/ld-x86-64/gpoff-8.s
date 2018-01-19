	.text
	.globl _start
_start:
	movl	%gs:foo@GPOFF, %eax
	movq   __gp@GOTPCREL(%rip), %rax

	.data
	.globl foo
foo:
	.long 0x12345678
