	.text
_start:
	addsubps 0x12345678(%rip),%xmm1
	comisd 0x12345678(%rip),%xmm1
	comiss 0x12345678(%rip),%xmm1
	cvtdq2pd 0x12345678(%rip),%xmm1
	cvtpd2dq 0x12345678(%rip),%xmm1
	cvtps2pd 0x12345678(%rip),%xmm1
	cvttps2dq 0x12345678(%rip),%xmm1
	haddps 0x12345678(%rip),%xmm1
	movdqu %xmm1,0x12345678(%rip)
	movdqu 0x12345678(%rip),%xmm1
	movhpd %xmm1,0x12345678(%rip)
	movhpd 0x12345678(%rip),%xmm1
	movhps %xmm1,0x12345678(%rip)
	movhps 0x12345678(%rip),%xmm1
	movlpd %xmm1,0x12345678(%rip)
	movlpd 0x12345678(%rip),%xmm1
	movlps %xmm1,0x12345678(%rip)
	movlps 0x12345678(%rip),%xmm1
	movq %xmm1,0x12345678(%rip)
	movq 0x12345678(%rip),%xmm1
	movshdup 0x12345678(%rip),%xmm1
	movsldup 0x12345678(%rip),%xmm1
	pshufhw $0x90,0x12345678(%rip),%xmm1
	pshuflw $0x90,0x12345678(%rip),%xmm1
	punpcklbw 0x12345678(%rip),%mm1
	punpckldq 0x12345678(%rip),%mm1
	punpcklwd 0x12345678(%rip),%mm1
	punpcklbw 0x12345678(%rip),%xmm1
	punpckldq 0x12345678(%rip),%xmm1
	punpcklwd 0x12345678(%rip),%xmm1
	punpcklqdq 0x12345678(%rip),%xmm1
	ucomisd 0x12345678(%rip),%xmm1
	ucomiss 0x12345678(%rip),%xmm1

	cmpeqsd (%rax),%xmm0
	cmpeqss (%rax),%xmm0
	cvtpi2pd (%rax),%xmm0
	cvtpi2ps (%rax),%xmm0
	cvtps2pi (%rax),%mm0
	cvtsd2si (%rax),%eax
	cvtsd2siq (%rax),%rax
	cvtsd2ss (%rax),%xmm0
	cvtss2sd (%rax),%xmm0
	cvtss2siq (%rax),%rax
	cvtss2si (%rax),%eax
	divsd (%rax),%xmm0
	divss (%rax),%xmm0
	maxsd (%rax),%xmm0
	maxss (%rax),%xmm0
	minss (%rax),%xmm0
	minss (%rax),%xmm0
	movntsd %xmm0,(%rax)
	movntss %xmm0,(%rax)
	movsd (%rax),%xmm0
	movsd %xmm0,(%rax)
	movss (%rax),%xmm0
	movss %xmm0,(%rax)
	mulsd (%rax),%xmm0
	mulss (%rax),%xmm0
	rcpss (%rax),%xmm0
	roundsd $0,(%rax),%xmm0
	roundss $0,(%rax),%xmm0
	rsqrtss (%rax),%xmm0
	sqrtsd (%rax),%xmm0
	sqrtss (%rax),%xmm0
	subsd (%rax),%xmm0
	subss (%rax),%xmm0

	pmovsxbw (%rax),%xmm0
	pmovsxbd (%rax),%xmm0
	pmovsxbq (%rax),%xmm0
	pmovsxwd (%rax),%xmm0
	pmovsxwq (%rax),%xmm0
	pmovsxdq (%rax),%xmm0
	pmovzxbw (%rax),%xmm0
	pmovzxbd (%rax),%xmm0
	pmovzxbq (%rax),%xmm0
	pmovzxwd (%rax),%xmm0
	pmovzxwq (%rax),%xmm0
	pmovzxdq (%rax),%xmm0
	insertps $0x0,(%rax),%xmm0

	.intel_syntax noprefix
	cvtss2si eax,DWORD PTR [rax]
	cvtss2si rax,DWORD PTR [rax]
	cvtsd2si eax,QWORD PTR [rax]
	cvtsd2si rax,QWORD PTR [rax]
