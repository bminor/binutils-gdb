# Check 64bit instructions with optimized encoding

	.allow_index_reg
	.text
_start:
	testq	$0x7f, %rax
	testl	$0x7f, %eax
	testw	$0x7f, %ax
	testb	$0x7f, %al
	test	$0x7f, %rbx
	test	$0x7f, %ebx
	test	$0x7f, %bx
	test	$0x7f, %bl
	test	$0x7f, %rdi
	test	$0x7f, %edi
	test	$0x7f, %di
	test	$0x7f, %dil
	test	$0x7f, %r9
	test	$0x7f, %r9d
	test	$0x7f, %r9w
	test	$0x7f, %r9b

	vandnpd	%zmm1, %zmm1, %zmm5

	vmovdqa32	%xmm1, %xmm2
	vmovdqa64	%xmm1, %xmm2
	vmovdqu8	%xmm1, %xmm2
	vmovdqu16	%xmm1, %xmm2
	vmovdqu32	%xmm1, %xmm2
	vmovdqu64	%xmm1, %xmm2

	vmovdqa32	%xmm11, %xmm12
	vmovdqa64	%xmm11, %xmm12
	vmovdqu8	%xmm11, %xmm12
	vmovdqu16	%xmm11, %xmm12
	vmovdqu32	%xmm11, %xmm12
	vmovdqu64	%xmm11, %xmm12

	vmovdqa32	127(%rax), %xmm2
	vmovdqa64	127(%rax), %xmm2
	vmovdqu8	127(%rax), %xmm2
	vmovdqu16	127(%rax), %xmm2
	vmovdqu32	127(%rax), %xmm2
	vmovdqu64	127(%rax), %xmm2

	vmovdqa32	%xmm1, 128(%rax)
	vmovdqa64	%xmm1, 128(%rax)
	vmovdqu8	%xmm1, 128(%rax)
	vmovdqu16	%xmm1, 128(%rax)
	vmovdqu32	%xmm1, 128(%rax)
	vmovdqu64	%xmm1, 128(%rax)

	vmovdqa32	%ymm1, %ymm2
	vmovdqa64	%ymm1, %ymm2
	vmovdqu8	%ymm1, %ymm2
	vmovdqu16	%ymm1, %ymm2
	vmovdqu32	%ymm1, %ymm2
	vmovdqu64	%ymm1, %ymm2

	vmovdqa32	%ymm11, %ymm12
	vmovdqa64	%ymm11, %ymm12
	vmovdqu8	%ymm11, %ymm12
	vmovdqu16	%ymm11, %ymm12
	vmovdqu32	%ymm11, %ymm12
	vmovdqu64	%ymm11, %ymm12

	vmovdqa32	127(%rax), %ymm2
	vmovdqa64	127(%rax), %ymm2
	vmovdqu8	127(%rax), %ymm2
	vmovdqu16	127(%rax), %ymm2
	vmovdqu32	127(%rax), %ymm2
	vmovdqu64	127(%rax), %ymm2

	vmovdqa32	%ymm1, 128(%rax)
	vmovdqa64	%ymm1, 128(%rax)
	vmovdqu8	%ymm1, 128(%rax)
	vmovdqu16	%ymm1, 128(%rax)
	vmovdqu32	%ymm1, 128(%rax)
	vmovdqu64	%ymm1, 128(%rax)

	vmovdqa32	%xmm21, %xmm2
	vmovdqa64	%xmm21, %xmm2
	vmovdqu8	%xmm21, %xmm2
	vmovdqu16	%xmm21, %xmm2
	vmovdqu32	%xmm21, %xmm2
	vmovdqu64	%xmm21, %xmm2

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

	vmovdqa32	(%rax), %ymm2{%k1}
	vmovdqa64	(%rax), %ymm2{%k1}
	vmovdqu8	(%rax), %xmm2{%k1}
	vmovdqu16	(%rax), %xmm2{%k1}
	vmovdqu32	(%rax), %xmm2{%k1}
	vmovdqu64	(%rax), %xmm2{%k1}

	vmovdqa32	%ymm1, (%rax){%k1}
	vmovdqa64	%ymm1, (%rax){%k1}
	vmovdqu8	%xmm1, (%rax){%k1}
	vmovdqu16	%xmm1, (%rax){%k1}
	vmovdqu32	%xmm1, (%rax){%k1}
	vmovdqu64	%xmm1, (%rax){%k1}

	vmovdqa32	%xmm1, %xmm2{%k1}{z}
	vmovdqa64	%xmm1, %xmm2{%k1}{z}
	vmovdqu8	%xmm1, %xmm2{%k1}{z}
	vmovdqu16	%xmm1, %xmm2{%k1}{z}
	vmovdqu32	%xmm1, %xmm2{%k1}{z}
	vmovdqu64	%xmm1, %xmm2{%k1}{z}
