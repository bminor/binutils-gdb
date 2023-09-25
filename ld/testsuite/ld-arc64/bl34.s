	.text
	.align 4
	.global __start
__start:
	nop_s
	bl_s	@foo@s32
	bl_s	@foo@s32

;;;  Have a symbol beyond 4G boundary.
	.section	".foo.text"
	.align	4
foo:
	add	r0,r0,r0
