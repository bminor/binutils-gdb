	.text
	.globl get_foo
get_foo:
	movl	%fs:foo@GPOFF, %eax
	ret

	.globl get_foo_gpoff
get_foo_gpoff:
	leal	foo@GPOFF, %eax
	ret

	.data
	.globl foo_gpoff
foo_gpoff:
	.long foo@GPOFF

	.data
	.globl foo
foo:
	.long 0x12345678
