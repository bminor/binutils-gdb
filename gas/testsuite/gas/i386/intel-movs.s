	.text
	.intel_syntax noprefix

movs:
	movsb
	movsb	es:[edi], [esi]
	movsb	es:[edi], fs:[esi]
	movsb	[edi], [esi]
	movsb	byte ptr es:[edi], [esi]
	movsb	es:[edi], byte ptr [esi]
	movsb	byte ptr es:[edi], byte ptr [esi]
	movs	byte ptr es:[edi], [esi]
	movs	es:[edi], byte ptr [esi]
	movs	byte ptr es:[edi], byte ptr [esi]

	movsw
	movsw	es:[edi], [esi]
	movsw	es:[edi], fs:[esi]
	movsw	[edi], [esi]
	movsw	word ptr es:[edi], [esi]
	movsw	es:[edi], word ptr [esi]
	movsw	word ptr es:[edi], word ptr [esi]
	movs	word ptr es:[edi], [esi]
	movs	es:[edi], word ptr [esi]
	movs	word ptr es:[edi], word ptr [esi]

	movsd
	movsd	es:[edi], [esi]
	movsd	es:[edi], fs:[esi]
	movsd	[edi], [esi]
	movsd	dword ptr es:[edi], [esi]
	movsd	es:[edi], dword ptr [esi]
	movsd	dword ptr es:[edi], dword ptr [esi]
	movs	dword ptr es:[edi], [esi]
	movs	es:[edi], dword ptr [esi]
	movs	dword ptr es:[edi], dword ptr [esi]

.ifdef x86_64
	movsq
	movsq	es:[rdi], [rsi]
	movsq	es:[rdi], fs:[rsi]
	movsq	[rdi], [rsi]
	movsq	qword ptr es:[rdi], [rsi]
	movsq	es:[rdi], qword ptr [rsi]
	movsq	qword ptr es:[rdi], qword ptr [rsi]
	movs	qword ptr es:[rdi], [rsi]
	movs	es:[rdi], qword ptr [rsi]
	movs	qword ptr es:[rdi], qword ptr [rsi]
.endif
