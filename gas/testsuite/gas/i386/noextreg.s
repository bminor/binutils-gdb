	.intel_syntax noprefix
	.text
ix86:
	vfmaddps xmm0, xmm0, [eax], xmm0
	.byte	0xc4, 0xe3, 0x79, 0x68, 0x00, 0x0f	# vfmaddps xmm0, xmm0, [eax], xmm0

	ret

	.type ix86, @function
	.size ix86, . - ix86
