	.p2align 4
	.globl	foo
	.type	foo, @function
foo:
	.cfi_startproc
	sub	$0x20,%rsp
	.cfi_adjust_cfa_offset 0x20
	popq	%rcx
	.cfi_register 16, 2
	.cfi_def_cfa_offset 0
	jmp	*%rcx
	.cfi_endproc
	.size	foo, .-foo
