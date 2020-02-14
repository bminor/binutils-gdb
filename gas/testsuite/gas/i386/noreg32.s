	.text
noreg:
	adc	$1, (%eax)
	adc	$0x89, (%eax)
	adc	$0x1234, (%eax)
	adc	$0x12345678, (%eax)
	add	$1, (%eax)
	add	$0x89, (%eax)
	add	$0x1234, (%eax)
	add	$0x12345678, (%eax)
	and	$1, (%eax)
	and	$0x89, (%eax)
	and	$0x1234, (%eax)
	and	$0x12345678, (%eax)
	bt	$1, (%eax)
	btc	$1, (%eax)
	btr	$1, (%eax)
	bts	$1, (%eax)
	call	*(%eax)
	cmp	$1, (%eax)
	cmp	$0x89, (%eax)
	cmp	$0x1234, (%eax)
	cmp	$0x12345678, (%eax)
	cmps
	cmps	%es:(%edi), (%esi)
	crc32	(%eax), %eax
	cvtsi2sd (%eax), %xmm0
	cvtsi2ss (%eax), %xmm0
	dec	(%eax)
	div	(%eax)
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
	idiv	(%eax)
	imul	(%eax)
	in	$0
	in	%dx
	inc	(%eax)
	ins
	ins	%dx, %es:(%edi)
	jmp	*(%eax)
	lgdt	(%eax)
	lidt	(%eax)
	lldt	(%eax)
	lmsw	(%eax)
	lods
	lods	(%esi)
	ltr	(%eax)
	mov	$0x12, (%eax)
	mov	$0x1234, (%eax)
	mov	$0x12345678, (%eax)
	mov	%es, (%eax)
	mov	(%eax), %es
	movs
	movs	(%esi), %es:(%edi)
	movsx	(%eax), %ax
	movsx	(%eax), %eax
	movzx	(%eax), %ax
	movzx	(%eax), %eax
	mul	(%eax)
	neg	(%eax)
	nop	(%eax)
	not	(%eax)
	or	$1, (%eax)
	or	$0x89, (%eax)
	or	$0x1234, (%eax)
	or	$0x12345678, (%eax)
	out	$0
	out	%dx
	outs
	outs	(%esi), %dx
	pop	(%eax)
	pop	%es
	ptwrite	(%eax)
	push	(%eax)
	push	%es
	rcl	$1, (%eax)
	rcl	$2, (%eax)
	rcl	%cl, (%eax)
	rcl	(%eax)
	rcr	$1, (%eax)
	rcr	$2, (%eax)
	rcr	%cl, (%eax)
	rcr	(%eax)
	rol	$1, (%eax)
	rol	$2, (%eax)
	rol	%cl, (%eax)
	rol	(%eax)
	ror	$1, (%eax)
	ror	$2, (%eax)
	ror	%cl, (%eax)
	ror	(%eax)
	sbb	$1, (%eax)
	sbb	$0x89, (%eax)
	sbb	$0x1234, (%eax)
	sbb	$0x12345678, (%eax)
	scas
	scas	%es:(%edi)
	sal	$1, (%eax)
	sal	$2, (%eax)
	sal	%cl, (%eax)
	sal	(%eax)
	sar	$1, (%eax)
	sar	$2, (%eax)
	sar	%cl, (%eax)
	sar	(%eax)
	shl	$1, (%eax)
	shl	$2, (%eax)
	shl	%cl, (%eax)
	shl	(%eax)
	shr	$1, (%eax)
	shr	$2, (%eax)
	shr	%cl, (%eax)
	shr	(%eax)
	stos
	stos	%es:(%edi)
	sub	$1, (%eax)
	sub	$0x89, (%eax)
	sub	$0x1234, (%eax)
	sub	$0x12345678, (%eax)
	test	$0x89, (%eax)
	test	$0x1234, (%eax)
	test	$0x12345678, (%eax)
	vcvtsi2sd (%eax), %xmm0, %xmm0
	{evex} vcvtsi2sd (%eax), %xmm0, %xmm0
	vcvtsi2ss (%eax), %xmm0, %xmm0
	{evex} vcvtsi2ss (%eax), %xmm0, %xmm0
	vcvtusi2sd (%eax), %xmm0, %xmm0
	vcvtusi2ss (%eax), %xmm0, %xmm0
	xor	$1, (%eax)
	xor	$0x89, (%eax)
	xor	$0x1234, (%eax)
	xor	$0x12345678, (%eax)
