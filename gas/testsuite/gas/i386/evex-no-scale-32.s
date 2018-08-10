	.allow_index_reg
	.text
disp:
	vmovaps	64(,%eax), %zmm0
	vmovaps	64(,%eiz), %zmm0
	vmovaps	64, %zmm0
	addr16 vmovaps 64, %zmm0
