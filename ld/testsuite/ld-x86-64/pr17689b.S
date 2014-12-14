	.text
	.globl	main
	.type	main, @function
main:
	subq	$8, %rsp
	movq	bar_alias(%rip), %rdi
	call	foo@PLT
	xorl	%eax, %eax
	addq	$8, %rsp
	ret
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
