	.text
	.globl _start
_start:
	movl	%fs:foo@GPOFF, %eax

	.data
	.globl bar_gpoff
bar_gpoff:
	.long bar@GPOFF

foo:
	.long 0x12345678

	.section .rodata,"a",@progbits
bar:
	.long 0x12345678
