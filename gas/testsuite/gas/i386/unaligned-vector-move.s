# Encode aligned vector move as unaligned vector move.

	.text
_start:
	movaps %xmm1, %xmm2
	movapd %xmm1, %xmm2
	movdqa %xmm1, %xmm2
	vmovaps %xmm1, %xmm2
	vmovapd %xmm1, %xmm2
	vmovdqa %xmm1, %xmm2
	vmovaps %xmm1, %xmm2
	vmovapd %xmm1, %xmm2{%k1}
	vmovaps %xmm1, %xmm2{%k1}
	vmovdqa32 %xmm1, %xmm2{%k1}
	vmovdqa64 %xmm1, %xmm2{%k1}
