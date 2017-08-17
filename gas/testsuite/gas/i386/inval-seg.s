	.text
# All the following should be illegal
	movl	%ds,(%eax)
	movl	(%eax),%ds

	.intel_syntax noprefix
	mov	eax, DWORD PTR fs:foobar:16
	mov	eax, DWORD PTR fs:foobar:barfoo:16
	mov	eax, DWORD PTR fs:ds:16
	mov	eax, DWORD PTR fs:ds:cs:16
