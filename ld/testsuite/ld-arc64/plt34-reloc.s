;;; Check if 34bit reloc is correctly solved by the linker.
	.text
	.align 4
	.global __start
__start:
	nop_s
	bl_s	@foo@plt34
	bl_s	@foo@plt34

;;;  Have a symbol beyond 4G boundary.
	.section	".foo.text"
	.align	4
foo:
	add	r0,r0,r0
