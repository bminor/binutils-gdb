# Check EVEX non-LIG instructions with with -mevexlig=256

	.allow_index_reg
	.text
_start:
	{evex} vmovd %xmm4,(%rcx)
	{evex} vmovd %xmm4,%ecx
	{evex} vmovd (%rcx),%xmm4
	{evex} vmovd %ecx,%xmm4

	{evex} vmovq %xmm4,(%rcx)
	{evex} vmovq %xmm4,%rcx
	{evex} vmovq (%rcx),%xmm4
	{evex} vmovq %rcx,%xmm4
	{evex} vmovq %xmm4,%xmm6
