# Check SSE2 instructions in 16-bit mode

	.code16
	.include "sse2.s"
	.att_syntax prefix

	addps (%bx),%xmm5
