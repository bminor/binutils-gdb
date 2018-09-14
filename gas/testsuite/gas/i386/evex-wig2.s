# Check non-WIG EVEX instructions with -mevexwig=1

	.allow_index_reg
	.text
_start:
	vcvtsi2ss %eax, {rd-sae}, %xmm5, %xmm6
	{evex} vcvtsi2ss %eax, %xmm5, %xmm6
	{evex}  vcvtsi2sd %eax, %xmm5, %xmm6
	vcvtusi2ss %eax, {rd-sae}, %xmm5, %xmm6
	{evex} vcvtusi2ss %eax, %xmm5, %xmm6
	{evex}  vcvtusi2sd %eax, %xmm5, %xmm6
