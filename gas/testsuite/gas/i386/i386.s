# i386 instructions
	.text

	fnstsw
	fnstsw	%ax
	fstsw
	fstsw	%ax

	movsx	%al, %si
	movsx	%al, %esi
	movsx	%ax, %esi
	movsx	(%eax), %dx
	movsxb	(%eax), %dx
	movsxb	(%eax), %edx
	movsxw	(%eax), %edx
	movsbl	(%eax), %edx
	movsbw	(%eax), %dx
	movswl	(%eax), %edx

	movzx	%al, %si
	movzx	%al, %esi
	movzx	%ax, %esi
	movzx	(%eax), %dx
	movzxb	(%eax), %dx
	movzxb	(%eax), %edx
	movzxw	(%eax), %edx
	movzb	(%eax), %edx
	movzb	(%eax), %dx
	movzbl	(%eax), %edx
	movzbw	(%eax), %dx
	movzwl	(%eax), %edx

	movnti %eax, (%eax)
	movntil %eax, (%eax)

	.intel_syntax noprefix
	fnstsw
	fnstsw	ax
	fstsw
	fstsw	ax

	movsx	si,al
	movsx	esi,al
	movsx	esi,ax
	movsx	edx,BYTE PTR [eax]
	movsx	dx,BYTE PTR [eax]
	movsx	edx,WORD PTR [eax]

	movzx	si,al
	movzx	esi,al
	movzx	esi,ax
	movzx	edx,BYTE PTR [eax]
	movzx	dx,BYTE PTR [eax]
	movzx	edx,WORD PTR [eax]

	movq	xmm1,QWORD PTR [esp]
	movq	xmm1,[esp]
	movq	QWORD PTR [esp],xmm1
	movq	[esp],xmm1

movsx ax, byte ptr [eax]
movsx eax, byte ptr [eax]
movsx eax, word ptr [eax]
movzx ax, byte ptr [eax]
movzx eax, byte ptr [eax]
movzx eax, word ptr [eax]

movnti dword ptr [eax], eax

	.att_syntax
	lar    %dx,%dx
	lar    %dx,%edx
	lar    %edx,%edx
	lar    (%edx),%dx
	lar    (%edx),%edx
	lsl    %dx,%dx
	lsl    %dx,%edx
	lsl    %edx,%edx
	lsl    (%edx),%dx
	lsl    (%edx),%edx

	.intel_syntax noprefix
	lar    dx,dx
	lar    edx,dx
	lar    edx,edx
	lar    dx,WORD PTR [edx]
	lar    edx,WORD PTR [edx]
	lsl    dx,dx
	lsl    edx,dx
	lsl    edx,edx
	lsl    dx,WORD PTR [edx]
	lsl    edx,WORD PTR [edx]
