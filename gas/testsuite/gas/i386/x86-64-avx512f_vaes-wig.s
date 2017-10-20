# Check 64bit AVX512F,VAES WIG instructions

	.allow_index_reg
	.text
_start:
	vaesdec	%zmm28, %zmm29, %zmm30	 # AVX512F,VAES
	vaesdec	0x123(%rax,%r14,8), %zmm29, %zmm30	 # AVX512F,VAES
	vaesdeclast	%zmm28, %zmm29, %zmm30	 # AVX512F,VAES
	vaesdeclast	0x123(%rax,%r14,8), %zmm29, %zmm30	 # AVX512F,VAES
	vaesenc	%zmm28, %zmm29, %zmm30	 # AVX512F,VAES
	vaesenc	0x123(%rax,%r14,8), %zmm29, %zmm30	 # AVX512F,VAES
	vaesenclast	%zmm28, %zmm29, %zmm30	 # AVX512F,VAES
	vaesenclast	0x123(%rax,%r14,8), %zmm29, %zmm30	 # AVX512F,VAES

	.intel_syntax noprefix
	vaesdec	zmm30, zmm29, zmm28	 # AVX512F,VAES
	vaesdec	zmm30, zmm29, ZMMWORD PTR [rax+r14*8+0x1234]	 # AVX512F,VAES
	vaesdeclast	zmm30, zmm29, zmm28	 # AVX512F,VAES
	vaesdeclast	zmm30, zmm29, ZMMWORD PTR [rax+r14*8+0x1234]	 # AVX512F,VAES
	vaesenc	zmm30, zmm29, zmm28	 # AVX512F,VAES
	vaesenc	zmm30, zmm29, ZMMWORD PTR [rax+r14*8+0x1234]	 # AVX512F,VAES
	vaesenclast	zmm30, zmm29, zmm28	 # AVX512F,VAES
	vaesenclast	zmm30, zmm29, ZMMWORD PTR [rax+r14*8+0x1234]	 # AVX512F,VAES
