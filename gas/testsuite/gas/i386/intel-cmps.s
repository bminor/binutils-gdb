	.text
	.intel_syntax noprefix

cmps:
	cmpsb
	cmpsb	[esi], es:[edi]
	cmpsb	fs:[esi], es:[edi]
	cmpsb	[esi], [edi]
	cmpsb	byte ptr [esi], es:[edi]
	cmpsb	[esi], byte ptr es:[edi]
	cmpsb	byte ptr [esi], byte ptr es:[edi]
	cmps	byte ptr [esi], es:[edi]
	cmps	[esi], byte ptr es:[edi]
	cmps	byte ptr [esi], byte ptr es:[edi]

	cmpsw
	cmpsw	[esi], es:[edi]
	cmpsw	fs:[esi], es:[edi]
	cmpsw	[esi], [edi]
	cmpsw	word ptr [esi], es:[edi]
	cmpsw	[esi], word ptr es:[edi]
	cmpsw	word ptr [esi], word ptr es:[edi]
	cmps	word ptr [esi], es:[edi]
	cmps	[esi], word ptr es:[edi]
	cmps	word ptr [esi], word ptr es:[edi]

	cmpsd
	cmpsd	[esi], es:[edi]
	cmpsd	fs:[esi], es:[edi]
	cmpsd	[esi], [edi]
	cmpsd	dword ptr [esi], es:[edi]
	cmpsd	[esi], dword ptr es:[edi]
	cmpsd	dword ptr [esi], dword ptr es:[edi]
	cmps	dword ptr [esi], es:[edi]
	cmps	[esi], dword ptr es:[edi]
	cmps	dword ptr [esi], dword ptr es:[edi]

.ifdef x86_64
	cmpsq
	cmpsq	[rsi], es:[rdi]
	cmpsq	fs:[rsi], es:[rdi]
	cmpsq	[rsi], [rdi]
	cmpsq	qword ptr [rsi], es:[rdi]
	cmpsq	[rsi], qword ptr es:[rdi]
	cmpsq	qword ptr [rsi], qword ptr es:[rdi]
	cmps	qword ptr [rsi], es:[rdi]
	cmps	[rsi], qword ptr es:[rdi]
	cmps	qword ptr [rsi], qword ptr es:[rdi]
.endif
