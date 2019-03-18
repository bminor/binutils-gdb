# Check instructions with optimized encoding

	.allow_index_reg
	.text
_start:
	testl	$0x7f, %eax
	testw	$0x7f, %ax
	testb	$0x7f, %al
	test	$0x7f, %ebx
	test	$0x7f, %bx
	test	$0x7f, %bl
	test	$0x7f, %edi
	test	$0x7f, %di

	vandnpd	%zmm1, %zmm1, %zmm5

	vmovdqa32	%xmm1, %xmm2
	vmovdqa64	%xmm1, %xmm2
	vmovdqu8	%xmm1, %xmm2
	vmovdqu16	%xmm1, %xmm2
	vmovdqu32	%xmm1, %xmm2
	vmovdqu64	%xmm1, %xmm2

	vmovdqa32	127(%eax), %xmm2
	vmovdqa64	127(%eax), %xmm2
	vmovdqu8	127(%eax), %xmm2
	vmovdqu16	127(%eax), %xmm2
	vmovdqu32	127(%eax), %xmm2
	vmovdqu64	127(%eax), %xmm2

	vmovdqa32	%xmm1, 128(%eax)
	vmovdqa64	%xmm1, 128(%eax)
	vmovdqu8	%xmm1, 128(%eax)
	vmovdqu16	%xmm1, 128(%eax)
	vmovdqu32	%xmm1, 128(%eax)
	vmovdqu64	%xmm1, 128(%eax)

	vmovdqa32	%ymm1, %ymm2
	vmovdqa64	%ymm1, %ymm2
	vmovdqu8	%ymm1, %ymm2
	vmovdqu16	%ymm1, %ymm2
	vmovdqu32	%ymm1, %ymm2
	vmovdqu64	%ymm1, %ymm2

	vmovdqa32	127(%eax), %ymm2
	vmovdqa64	127(%eax), %ymm2
	vmovdqu8	127(%eax), %ymm2
	vmovdqu16	127(%eax), %ymm2
	vmovdqu32	127(%eax), %ymm2
	vmovdqu64	127(%eax), %ymm2

	vmovdqa32	%ymm1, 128(%eax)
	vmovdqa64	%ymm1, 128(%eax)
	vmovdqu8	%ymm1, 128(%eax)
	vmovdqu16	%ymm1, 128(%eax)
	vmovdqu32	%ymm1, 128(%eax)
	vmovdqu64	%ymm1, 128(%eax)

	vmovdqa32	%zmm1, %zmm2
	vmovdqa64	%zmm1, %zmm2
	vmovdqu8	%zmm1, %zmm2
	vmovdqu16	%zmm1, %zmm2
	vmovdqu32	%zmm1, %zmm2
	vmovdqu64	%zmm1, %zmm2

	{evex} vmovdqa32	%ymm1, %ymm2
	{evex} vmovdqa64	%ymm1, %ymm2
	{evex} vmovdqu8		%xmm1, %xmm2
	{evex} vmovdqu16	%xmm1, %xmm2
	{evex} vmovdqu32	%xmm1, %xmm2
	{evex} vmovdqu64	%xmm1, %xmm2

	vmovdqa32	%ymm1, %ymm2{%k1}
	vmovdqa64	%ymm1, %ymm2{%k1}
	vmovdqu8	%xmm1, %xmm2{%k1}
	vmovdqu16	%xmm1, %xmm2{%k1}
	vmovdqu32	%xmm1, %xmm2{%k1}
	vmovdqu64	%xmm1, %xmm2{%k1}

	vmovdqa32	(%eax), %ymm2{%k1}
	vmovdqa64	(%eax), %ymm2{%k1}
	vmovdqu8	(%eax), %xmm2{%k1}
	vmovdqu16	(%eax), %xmm2{%k1}
	vmovdqu32	(%eax), %xmm2{%k1}
	vmovdqu64	(%eax), %xmm2{%k1}

	vmovdqa32	%ymm1, (%eax){%k1}
	vmovdqa64	%ymm1, (%eax){%k1}
	vmovdqu8	%xmm1, (%eax){%k1}
	vmovdqu16	%xmm1, (%eax){%k1}
	vmovdqu32	%xmm1, (%eax){%k1}
	vmovdqu64	%xmm1, (%eax){%k1}

	vmovdqa32	%xmm1, %xmm2{%k1}{z}
	vmovdqa64	%xmm1, %xmm2{%k1}{z}
	vmovdqu8	%xmm1, %xmm2{%k1}{z}
	vmovdqu16	%xmm1, %xmm2{%k1}{z}
	vmovdqu32	%xmm1, %xmm2{%k1}{z}
	vmovdqu64	%xmm1, %xmm2{%k1}{z}
