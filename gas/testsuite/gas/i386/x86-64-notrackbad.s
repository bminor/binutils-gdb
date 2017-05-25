# Check 64bit unsupported NOTRACK prefix

	.allow_index_reg
	.text
_start:
	notrack call foo
	notrack jmp foo

	notrack call *(%rax)
	notrack jmp *(%rax)
	fs notrack call *%rax
	notrack fs call *%rax
	notrack bnd call *%rax
	notrack data16 call *%rax

	.intel_syntax noprefix
	notrack call [rax]
	notrack jmp [rax]
	fs notrack call rax
	notrack fs call rax
	notrack bnd call rax
	notrack data16 call rax
