	.section .text
sh4_nofpu:
	ldc r1,sgr
	ldtlb
