	.text
_start:
	leal	foo@GPOFF, %eax
	movl	%fs:foo@GPOFF, %eax
