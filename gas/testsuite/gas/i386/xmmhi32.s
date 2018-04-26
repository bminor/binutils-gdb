	.text
	.intel_syntax noprefix
	.code32
xmm:
	vaddps	xmm0, xmm1, xmm8
	vaddps	ymm0, ymm1, ymm8
	vaddps	zmm0, zmm1, zmm8
	vaddps	zmm0, zmm1, zmm16
	vaddps	zmm0, zmm1, zmm24

	vmovdqa	xmm0, xmm8
	vmovdqa	ymm0, ymm8
	vmovdqa	xmm8, xmm0
	vmovdqa	ymm8, ymm0

	.arch .noavx512f
	vaddps	xmm0, xmm1, xmm8
	vaddps	ymm0, ymm1, ymm8
	vmovdqa	xmm0, zmm0
	vmovdqa	xmm0, k0

	.arch .noavx
	addps	xmm0, xmm8
	addps	xmm0, ymm0
	addps	xmm0, ymm8
	addps	xmm0, zmm0
	addps	xmm0, k0

	.arch .nosse
	mov	eax, xmm0
	mov	eax, ymm0
	mov	eax, zmm0
	mov	eax, k0
