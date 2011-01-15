	.text
# All the following should be illegal for x32
	movabs xxx,%rax
	movabs foo,%rax

	.data
xxx:
	.byte 0
