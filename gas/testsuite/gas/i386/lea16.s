	.code16

	.macro addr16 mnem:req opnds:vararg
	addr32 \mnem \opnds
	.endm

	.include "lea.s"
