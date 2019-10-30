	.text
	.code16
noreg:
	adc	$1, (%bx)
	add	$1, (%bx)
	and	$1, (%bx)
	bt	$1, (%bx)
	btc	$1, (%bx)
	btr	$1, (%bx)
	bts	$1, (%bx)
	call	*(%bx)
	cmp	$1, (%bx)
	cvtsi2sd (%bx), %xmm0
	cvtsi2ss (%bx), %xmm0
	fadd	(%bx)
	fcom	(%bx)
	fcomp	(%bx)
	fdiv	(%bx)
	fdivr	(%bx)
	fiadd	(%bx)
	ficom	(%bx)
	ficomp	(%bx)
	fidiv	(%bx)
	fidivr	(%bx)
	fild	(%bx)
	fimul	(%bx)
	fist	(%bx)
	fistp	(%bx)
	fisttp	(%bx)
	fisub	(%bx)
	fisubr	(%bx)
	fld	(%bx)
	fmul	(%bx)
	fst	(%bx)
	fstp	(%bx)
	fsub	(%bx)
	fsubr	(%bx)
	jmp	*(%bx)
	lgdt	(%bx)
	lidt	(%bx)
	lldt	(%bx)
	lmsw	(%bx)
	ltr	(%bx)
	mov	%es, (%bx)
	mov	(%bx), %es
	nop	(%bx)
	or	$1, (%bx)
	pop	(%bx)
	pop	%es
	ptwrite	(%bx)
	push	(%bx)
	push	%es
	sbb	$1, (%bx)
	sub	$1, (%bx)
	vcvtsi2sd (%bx), %xmm0, %xmm0
	{evex} vcvtsi2sd (%bx), %xmm0, %xmm0
	vcvtsi2ss (%bx), %xmm0, %xmm0
	{evex} vcvtsi2ss (%bx), %xmm0, %xmm0
	vcvtusi2sd (%bx), %xmm0, %xmm0
	vcvtusi2ss (%bx), %xmm0, %xmm0
	xor	$1, (%bx)
