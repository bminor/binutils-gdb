# Check 64bit NOTRACK prefix

	.allow_index_reg
	.text
_start:
	notrack call *%rax
	notrack call *%r8
	notrack jmp *%rax
	notrack jmp *%r8

	bnd notrack call *%rax
	bnd notrack call *%r8
	bnd notrack jmp *%rax
	bnd notrack jmp *%r8

	.intel_syntax noprefix
	notrack call rax
	notrack call r8
	notrack jmp rax
	notrack jmp r8

	bnd notrack call rax
	bnd notrack call r8
	bnd notrack jmp rax
	bnd notrack jmp r8

	.byte 0x3e
	.byte 0xf2
	.byte 0xff
	.byte 0xd0

	.byte 0x3e
	.byte 0x66
	.byte 0xff
	.byte 0xd0

	.byte 0x66
	.byte 0x3e
	.byte 0xff
	.byte 0xd0
