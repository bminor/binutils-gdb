	.text
# All the following should be illegal for x32
	movabs xxx,%rax
	movabs foo,%rax
	movabsq xxx,%rax
	movabsq foo,%rax

	.data
xxx:
	.quad foo
