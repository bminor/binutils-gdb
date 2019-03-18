# Check 64bit instructions with optimized encoding

	.allow_index_reg
	.text
_start:
	{nooptimize} testl $0x7f, %eax

	{nooptimize} vmovdqa32	%ymm1, %ymm2
	{nooptimize} vmovdqa64	%ymm1, %ymm2
	{nooptimize} vmovdqu8	%xmm1, %xmm2
	{nooptimize} vmovdqu16	%xmm1, %xmm2
	{nooptimize} vmovdqu32	%xmm1, %xmm2
	{nooptimize} vmovdqu64	%xmm1, %xmm2
