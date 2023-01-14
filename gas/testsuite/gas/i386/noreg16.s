	.text
	.code16
noreg:
	adc	$1, (%bx)
	adc	$0x89, (%bx)
	adc	$0x1234, (%bx)
	add	$1, (%bx)
	add	$0x89, (%bx)
	add	$0x1234, (%bx)
	and	$1, (%bx)
	and	$0x89, (%bx)
	and	$0x1234, (%bx)
	bt	$1, (%bx)
	btc	$1, (%bx)
	btr	$1, (%bx)
	bts	$1, (%bx)
	call	*(%bx)
	cmp	$1, (%bx)
	cmp	$0x89, (%bx)
	cmp	$0x1234, (%bx)
	cmps
	cmps	%es:(%edi), (%esi)
	crc32	(%bx), %eax
	cvtsi2sd (%bx), %xmm0
	cvtsi2ss (%bx), %xmm0
	dec	(%bx)
	div	(%bx)
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
	idiv	(%bx)
	imul	(%bx)
	in	$0
	in	%dx
	inc	(%bx)
	ins
	ins	%dx, %es:(%edi)
	jmp	*(%bx)
	lgdt	(%bx)
	lidt	(%bx)
	lldt	(%bx)
	lmsw	(%bx)
	lods
	lods	(%esi)
	ltr	(%bx)
	mov	$0x12, (%bx)
	mov	$0x1234, (%bx)
	mov	%es, (%bx)
	mov	(%bx), %es
	movs
	movs	(%esi), %es:(%edi)
	movsx	(%bx), %ax
	movsx	(%bx), %eax
	movzx	(%bx), %ax
	movzx	(%bx), %eax
	mul	(%bx)
	neg	(%bx)
	nop	(%bx)
	not	(%bx)
	or	$1, (%bx)
	or	$0x89, (%bx)
	or	$0x1234, (%bx)
	out	$0
	out	%dx
	outs
	outs	(%esi), %dx
	pop	(%bx)
	pop	%es
	ptwrite	(%bx)
	push	(%bx)
	push	%es
	rcl	$1, (%bx)
	rcl	$2, (%bx)
	rcl	%cl, (%bx)
	rcl	(%bx)
	rcr	$1, (%bx)
	rcr	$2, (%bx)
	rcr	%cl, (%bx)
	rcr	(%bx)
	rol	$1, (%bx)
	rol	$2, (%bx)
	rol	%cl, (%bx)
	rol	(%bx)
	ror	$1, (%bx)
	ror	$2, (%bx)
	ror	%cl, (%bx)
	ror	(%bx)
	sbb	$1, (%bx)
	sbb	$0x89, (%bx)
	sbb	$0x1234, (%bx)
	scas
	scas	%es:(%edi)
	sal	$1, (%bx)
	sal	$2, (%bx)
	sal	%cl, (%bx)
	sal	(%bx)
	sar	$1, (%bx)
	sar	$2, (%bx)
	sar	%cl, (%bx)
	sar	(%bx)
	shl	$1, (%bx)
	shl	$2, (%bx)
	shl	%cl, (%bx)
	shl	(%bx)
	shr	$1, (%bx)
	shr	$2, (%bx)
	shr	%cl, (%bx)
	shr	(%bx)
	stos
	stos	%es:(%edi)
	sub	$1, (%bx)
	sub	$0x89, (%bx)
	sub	$0x1234, (%bx)
	test	$0x89, (%bx)
	test	$0x1234, (%bx)
	vcvtsi2sd (%bx), %xmm0, %xmm0
	{evex} vcvtsi2sd (%bx), %xmm0, %xmm0
	vcvtsi2ss (%bx), %xmm0, %xmm0
	{evex} vcvtsi2ss (%bx), %xmm0, %xmm0
	vcvtusi2sd (%bx), %xmm0, %xmm0
	vcvtusi2ss (%bx), %xmm0, %xmm0
	xor	$1, (%bx)
	xor	$0x89, (%bx)
	xor	$0x1234, (%bx)
