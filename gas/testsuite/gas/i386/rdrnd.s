# Check RdRnd new instructions.

	.text
foo:
	rdrnd %bx
	rdrnd %ebx

	.intel_syntax noprefix
	rdrnd bx
	rdrnd ebx
