# Check 64bit RdRnd new instructions.

	.text
foo:
	rdrnd %bx
	rdrnd %ebx
	rdrnd %rbx
	rdrnd %r8w
	rdrnd %r8d
	rdrnd %r8

	.intel_syntax noprefix
	rdrnd bx
	rdrnd ebx
	rdrnd rbx
	rdrnd r8w
	rdrnd r8d
	rdrnd r8
