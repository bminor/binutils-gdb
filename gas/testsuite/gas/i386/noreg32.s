	.text
noreg:
	adc	$1, (%eax)
	add	$1, (%eax)
	and	$1, (%eax)
	bt	$1, (%eax)
	btc	$1, (%eax)
	btr	$1, (%eax)
	bts	$1, (%eax)
	call	*(%eax)
	cmp	$1, (%eax)
	cvtsi2sd (%eax), %xmm0
	cvtsi2ss (%eax), %xmm0
	fadd	(%eax)
	fcom	(%eax)
	fcomp	(%eax)
	fdiv	(%eax)
	fdivr	(%eax)
	fiadd	(%eax)
	ficom	(%eax)
	ficomp	(%eax)
	fidiv	(%eax)
	fidivr	(%eax)
	fild	(%eax)
	fimul	(%eax)
	fist	(%eax)
	fistp	(%eax)
	fisttp	(%eax)
	fisub	(%eax)
	fisubr	(%eax)
	fld	(%eax)
	fmul	(%eax)
	fst	(%eax)
	fstp	(%eax)
	fsub	(%eax)
	fsubr	(%eax)
	jmp	*(%eax)
	lgdt	(%eax)
	lidt	(%eax)
	lldt	(%eax)
	lmsw	(%eax)
	ltr	(%eax)
	mov	%es, (%eax)
	mov	(%eax), %es
	nop	(%eax)
	or	$1, (%eax)
	pop	(%eax)
	pop	%es
	ptwrite	(%eax)
	push	(%eax)
	push	%es
	sbb	$1, (%eax)
	sub	$1, (%eax)
	vcvtsi2sd (%eax), %xmm0, %xmm0
	{evex} vcvtsi2sd (%eax), %xmm0, %xmm0
	vcvtsi2ss (%eax), %xmm0, %xmm0
	{evex} vcvtsi2ss (%eax), %xmm0, %xmm0
	vcvtusi2sd (%eax), %xmm0, %xmm0
	vcvtusi2ss (%eax), %xmm0, %xmm0
	xor	$1, (%eax)
