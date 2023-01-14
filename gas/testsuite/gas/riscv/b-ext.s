target:
	clz	a0, a0
	ctz	a0, a0
	cpop	a0, a0
	min	a0, a1, a2
	minu	a0, a1, a2
	max	a0, a1, a2
	maxu	a0, a1, a2
	sext.b	a0, a0
	sext.h	a0, a0
	zext.h	a0, a0
	andn	a0, a1, a2
	orn	a0, a1, a2
	xnor	a0, a1, a2
	rol	a0, a1, a2
	ror	a0, a1, a2
	ror	a0, a1, 2
	rori	a0, a1, 2
	rev8	a0, a0
	orc.b	a0, a0
	sh1add	a0, a1, a2
	sh2add	a0, a1, a2
	sh3add	a0, a1, a2
	clmul	a0, a1, a2
	clmulh	a0, a1, a2
	clmulr	a0, a1, a2
