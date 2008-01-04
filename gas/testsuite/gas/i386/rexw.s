# Instructions where REX.W is ignored.

	.text
foo:
	extractps $0x0,%xmm0,%rcx
	movmskpd %xmm2,%rcx
	movmskps %xmm2,%rcx
	pextrb $0x0,%xmm0,%rcx
	pextrw $0x0,%mm0,%rcx
	pextrw $0x0,%xmm0,%rcx
	pinsrb $0x0,%rcx,%xmm0
	pinsrw $0x0,%rcx,%xmm0
	pinsrw $0x0,%rcx,%mm0
	pmovmskb %mm5,%rax
	pmovmskb %xmm5,%rax

	.intel_syntax noprefix
	extractps rcx,xmm0,0x0
	movmskpd rcx,xmm2
	movmskps rcx,xmm2
	pextrb rcx,xmm0,0x0
	pextrw rcx,mm0,0x0
	pextrw rcx,xmm0,0x0
	pinsrb xmm0,rcx,0x0
	pinsrw mm0,rcx,0x0
	pinsrw xmm0,rcx,0x0
	pmovmskb rcx,mm5
	pmovmskb rcx,xmm5
