	.text
noreg:
	adc	$1, (%rax)
	add	$1, (%rax)
	and	$1, (%rax)
	bt	$1, (%rax)
	btc	$1, (%rax)
	btr	$1, (%rax)
	bts	$1, (%rax)
	call	*(%rax)
	cmp	$1, (%rax)
	cvtsi2sd (%rax), %xmm0
	cvtsi2ss (%rax), %xmm0
	fadd	(%rax)
	fcom	(%rax)
	fcomp	(%rax)
	fdiv	(%rax)
	fdivr	(%rax)
	fiadd	(%rax)
	ficom	(%rax)
	ficomp	(%rax)
	fidiv	(%rax)
	fidivr	(%rax)
	fild	(%rax)
	fimul	(%rax)
	fist	(%rax)
	fistp	(%rax)
	fisttp	(%rax)
	fisub	(%rax)
	fisubr	(%rax)
	fld	(%rax)
	fmul	(%rax)
	fst	(%rax)
	fstp	(%rax)
	fsub	(%rax)
	fsubr	(%rax)
	jmp	*(%rax)
	lgdt	(%rax)
	lidt	(%rax)
	lldt	(%rax)
	lmsw	(%rax)
	ltr	(%rax)
	mov	%es, (%rax)
	mov	(%rax), %es
	nop	(%rax)
	or	$1, (%rax)
	pop	(%rax)
	pop	%fs
	ptwrite	(%rax)
	push	(%rax)
	push	%fs
	sbb	$1, (%rax)
	sub	$1, (%rax)
	vcvtsi2sd (%rax), %xmm0, %xmm0
	vcvtsi2sd (%rax), %xmm0, %xmm31
	vcvtsi2ss (%rax), %xmm0, %xmm0
	vcvtsi2ss (%rax), %xmm0, %xmm31
	vcvtusi2sd (%rax), %xmm0, %xmm0
	vcvtusi2ss (%rax), %xmm0, %xmm0
	xor	$1, (%rax)
