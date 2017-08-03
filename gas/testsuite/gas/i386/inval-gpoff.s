	.text
_start:
	movl	%fs:foo@GPOFF(%eax), %eax
	movl	%ds:foo@GPOFF(%eax, %ecx, 2), %eax
	movl	foo@GPOFF(%eax), %eax
