	.text
_start:
	leal	foo@GPOFF, %eax
	movl	%gs:foo@GPOFF, %eax
