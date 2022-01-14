	.text
	.globl foo
	.type foo, @function
foo:
	.cfi_startproc
	call	func@plt
	movq	func@GOTPCREL(%rip), %rax
	.cfi_endproc
	.section .init_array,"aw",@init_array
	.p2align 3
	.zero 0x4
	.section .fini_array,"aw",@fini_array
	.p2align 20
	.zero 0x100
	.data
	.zero 0x100
