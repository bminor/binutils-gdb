;;; Check if 34bit reloc is correctly solved by the linker.
	.text
	.align 4
	.global __start
__start:
	nop_s
	bl	@foo@plt
	bl_s	@foo@plt34

