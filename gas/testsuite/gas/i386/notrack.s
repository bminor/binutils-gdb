# Check 32bit NOTRACK prefix

	.allow_index_reg
	.text
_start:
	notrack call *%eax
	notrack call *%ax
	notrack jmp *%eax
	notrack jmp *%ax

	bnd notrack call *%eax
	bnd notrack call *%ax
	bnd notrack jmp *%eax
	bnd notrack jmp *%ax

	.intel_syntax noprefix
	notrack call eax
	notrack call ax
	notrack jmp eax
	notrack jmp ax

	bnd notrack call eax
	bnd notrack call ax
	bnd notrack jmp eax
	bnd notrack jmp ax

	.byte 0x3e
	.byte 0xf2
	.byte 0xff
	.byte 0xd0

	.byte 0x3e
	.byte 0x66
	.byte 0xff
	.byte 0xd0
