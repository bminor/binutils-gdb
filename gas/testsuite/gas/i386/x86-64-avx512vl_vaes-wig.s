# Check 64bit AVX512VL,VAES WIG instructions

	.allow_index_reg
	.text
_start:
	vaesdec	%xmm28, %xmm29, %xmm30	 # AVX512VL,VAES
	vaesdec	0x123(%rax,%r14,8), %xmm29, %xmm30	 # AVX512VL,VAES
	vaesdec	%ymm28, %ymm29, %ymm30	 # AVX512VL,VAES
	vaesdec	0x123(%rax,%r14,8), %ymm29, %ymm30	 # AVX512VL,VAES
	vaesdeclast	%xmm28, %xmm29, %xmm30	 # AVX512VL,VAES
	vaesdeclast	0x123(%rax,%r14,8), %xmm29, %xmm30	 # AVX512VL,VAES
	vaesdeclast	%ymm28, %ymm29, %ymm30	 # AVX512VL,VAES
	vaesdeclast	0x123(%rax,%r14,8), %ymm29, %ymm30	 # AVX512VL,VAES
	vaesenc	%xmm28, %xmm29, %xmm30	 # AVX512VL,VAES
	vaesenc	0x123(%rax,%r14,8), %xmm29, %xmm30	 # AVX512VL,VAES
	vaesenc	%ymm28, %ymm29, %ymm30	 # AVX512VL,VAES
	vaesenc	0x123(%rax,%r14,8), %ymm29, %ymm30	 # AVX512VL,VAES
	vaesenclast	%xmm28, %xmm29, %xmm30	 # AVX512VL,VAES
	vaesenclast	0x123(%rax,%r14,8), %xmm29, %xmm30	 # AVX512VL,VAES
	vaesenclast	%ymm28, %ymm29, %ymm30	 # AVX512VL,VAES
	vaesenclast	0x123(%rax,%r14,8), %ymm29, %ymm30	 # AVX512VL,VAES

	.intel_syntax noprefix
	vaesdec	xmm30, xmm29, xmm28	 # AVX512VL,VAES
	vaesdec	xmm30, xmm29, XMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesdec	ymm30, ymm29, ymm28	 # AVX512VL,VAES
	vaesdec	ymm30, ymm29, YMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesdeclast	xmm30, xmm29, xmm28	 # AVX512VL,VAES
	vaesdeclast	xmm30, xmm29, XMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesdeclast	ymm30, ymm29, ymm28	 # AVX512VL,VAES
	vaesdeclast	ymm30, ymm29, YMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesenc	xmm30, xmm29, xmm28	 # AVX512VL,VAES
	vaesenc	xmm30, xmm29, XMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesenc	ymm30, ymm29, ymm28	 # AVX512VL,VAES
	vaesenc	ymm30, ymm29, YMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesenclast	xmm30, xmm29, xmm28	 # AVX512VL,VAES
	vaesenclast	xmm30, xmm29, XMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
	vaesenclast	ymm30, ymm29, ymm28	 # AVX512VL,VAES
	vaesenclast	ymm30, ymm29, YMMWORD PTR [rax+r14*8+0x1234]	 # AVX512VL,VAES
