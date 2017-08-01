	.text
# All the following should be illegal
	movq	%ds,(%rax)
	movl	%ds,(%rax)
	movq	(%rax),%ds
	movl	(%rax),%ds

	.intel_syntax noprefix
	mov	eax, DWORD PTR fs:foobar:16
	mov	eax, DWORD PTR fs:foobar:barfoo:16
	mov	eax, DWORD PTR fs:ds:16
	mov	eax, DWORD PTR fs:ds:cs:16
