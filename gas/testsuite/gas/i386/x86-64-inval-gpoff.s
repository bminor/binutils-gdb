	.text
_start:
	movl	%fs:foo@GPOFF(%rip), %eax
	movl	%fs:foo@GPOFF(%rax), %eax
	movl	%ds:foo@GPOFF(%rax, %rcx, 2), %eax
	movl	foo@GPOFF(%rip), %eax
	movl	foo@GPOFF(%rax), %eax
