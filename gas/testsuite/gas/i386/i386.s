# i386 instructions
	.text

	movsx	(%eax), %edx
	movsx	(%eax), %dx
	movsbl	(%eax), %edx
	movsbw	(%eax), %dx
	movsbl	(%eax), %edx
	movswl	(%eax), %edx

	movzx	(%eax), %edx
	movzx	(%eax), %dx
	movzb	(%eax), %edx
	movzb	(%eax), %dx
	movzbl	(%eax), %edx
	movzbw	(%eax), %dx
	movzwl	(%eax), %edx

	.intel_syntax noprefix
	movsx	edx,BYTE PTR [eax]
	movsx	dx,BYTE PTR [eax]
	movsx	edx,WORD PTR [eax]

	movzx	edx,BYTE PTR [eax]
	movzx	dx,BYTE PTR [eax]
	movzx	edx,WORD PTR [eax]

	movq	xmm1,QWORD PTR [esp]
	movq	xmm1,[esp]
	movq	QWORD PTR [esp],xmm1
	movq	[esp],xmm1
