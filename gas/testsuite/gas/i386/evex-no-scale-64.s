	.allow_index_reg
	.text
disp:
	vmovaps	-1024(%rip), %zmm0
	vmovaps	64(,%rax), %zmm0
	vmovaps	64(,%riz), %zmm0
	vmovaps	64(,%eax), %zmm0
	vmovaps	64(,%eiz), %zmm0
	vmovaps	64, %zmm0
