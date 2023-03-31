	.text
insn:
	# nop
	.insn 0x90

	# pause
	.insn 0xf390
	.insn repe 0x90

	# fldz
	.insn 0xd9ee

	# setssbsy
	.insn 0xf30f01e8

	# mov
	.insn 0x8b, %ecx, %eax
	.insn 0x8b, %ax, %cx
	.insn 0x89, %ecx, 4(%eax)
	.insn 0x8b, 0x4444(,%eax), %ecx

	# movzx
	.insn 0x0fb6, %ah, %cx
	.insn 0x0fb7, %eax, %ecx

	# bswap
	.insn 0x0fc8+r, %edx

	# vzeroall
	.insn VEX.256.0F.WIG 0x77
	.insn {vex3} VEX.L1 0x0f77

	# vaddpd
	.insn VEX.66.0F 0x58, %xmm0, %xmm1, %xmm2
	.insn VEX.66 0x0f58, %ymm0, %ymm1, %ymm2

	# vaddss
	.insn VEX.LIG.F3.0F 0x58, %xmm0, %xmm1, %xmm2

	# vfmaddps
	.insn VEX.66.0F3A.W0 0x68, %xmm0, (%ecx), %xmm2, %xmm3
	.insn VEX.66.0F3A.W1 0x68, %xmm0, (%ecx), %xmm2, %xmm3
	.insn VEX.66.0F3A.W1 0x68, (%eax), %xmm1, %xmm2, %xmm3

	# kmovw
	.insn VEX.L0.0F.W0 0x92, %eax, %k1
	.insn VEX.L0.0F.W0 0x93, %k1, %eax

	# vaddps
	.insn EVEX.NP.0F.W0 0x58, {rn-sae}, %zmm0, %zmm1, %zmm2

	# vgather...
	.insn VEX.66.0f38.W0 0x92, %xmm0, (%eax, %xmm1, 2), %xmm3
	.insn EVEX.66.0f38.W1 0x93, (%eax, %xmm1, 2), %xmm3{%k4}
