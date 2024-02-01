# Check illegal 64bit APX_F instructions
	.text
	addq $0xe0, %fs:0, %rdx
	xorq $0xe0, foo(%eax,%edx), %rdx
