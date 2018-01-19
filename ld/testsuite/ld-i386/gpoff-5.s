	.text
	.globl _start
_start:
	movl	%fs:foo@GPOFF, %eax

	.data
	.globl bar_gpoff
bar_gpoff:
	.long bar@GPOFF

	.globl foo
foo:
	.long 0x12345678

	.section .rodata,"a",@progbits
	.globl bar
bar:
	.long 0x12345678
