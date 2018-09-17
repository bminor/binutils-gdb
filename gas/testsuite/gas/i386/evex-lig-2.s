# Check EVEX non-LIG instructions with with -mevexlig=256

	.allow_index_reg
	.text
_start:
	{evex} vmovd %xmm4,(%ecx)
	{evex} vmovd %xmm4,%ecx
	{evex} vmovd (%ecx),%xmm4
	{evex} vmovd %ecx,%xmm4

	{evex} vmovq %xmm4,(%ecx)
	{evex} vmovq (%ecx),%xmm4

	{evex} vmovq %xmm4,%xmm6
