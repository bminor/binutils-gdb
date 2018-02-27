# Check instructions with optimized encoding

	.allow_index_reg
	.text
_start:
	vandnpd %zmm1, %zmm1, %zmm5{%k7}
	vandnpd %ymm1, %ymm1, %ymm5{z}{%k7}
	vandnpd %zmm1, %zmm1, %zmm5
	vandnpd %ymm1, %ymm1, %ymm5

	vandnps %zmm1, %zmm1, %zmm5{%k7}
	vandnps %ymm1, %ymm1, %ymm5{z}{%k7}
	vandnps %zmm1, %zmm1, %zmm5
	vandnps %ymm1, %ymm1, %ymm5

	vpandn %ymm1, %ymm1, %ymm5

	vpandnd %zmm1, %zmm1, %zmm5{%k7}
	vpandnd %ymm1, %ymm1, %ymm5{z}{%k7}
	vpandnd %zmm1, %zmm1, %zmm5
	vpandnd %ymm1, %ymm1, %ymm5

	vpandnq %zmm1, %zmm1, %zmm5{%k7}
	vpandnq %ymm1, %ymm1, %ymm5{z}{%k7}
	vpandnq %zmm1, %zmm1, %zmm5
	vpandnq %ymm1, %ymm1, %ymm5

	vxorpd %zmm1, %zmm1, %zmm5{%k7}
	vxorpd %ymm1, %ymm1, %ymm5{z}{%k7}
	vxorpd %zmm1, %zmm1, %zmm5
	vxorpd %ymm1, %ymm1, %ymm5

	vxorps %zmm1, %zmm1, %zmm5{%k7}
	vxorps %ymm1, %ymm1, %ymm5{z}{%k7}
	vxorps %zmm1, %zmm1, %zmm5
	vxorps %ymm1, %ymm1, %ymm5

	vpxor %ymm1, %ymm1, %ymm5

	vpxord %zmm1, %zmm1, %zmm5{%k7}
	vpxord %ymm1, %ymm1, %ymm5{z}{%k7}
	vpxord %zmm1, %zmm1, %zmm5
	vpxord %ymm1, %ymm1, %ymm5

	vpxorq %zmm1, %zmm1, %zmm5{%k7}
	vpxorq %ymm1, %ymm1, %ymm5{z}{%k7}
	vpxorq %zmm1, %zmm1, %zmm5
	vpxorq %ymm1, %ymm1, %ymm5
