	.text

# All the following should yield "operand type mismatch" (or something yet more
# specific), but _not_ "operand size mismatch".

	bsf	%eax, (%eax)
	bswap	%mm0
	bswapl	%xmm0
	movntdqa %xmm0, (%eax)
	movntdq	(%eax), %xmm0
