	# pseudo/aliaese
	zext.h	a0, a1
	rev8	a0, a1
	orc.b	a0, a1
.ifdef __64_bit__
	zext.w	a0, a1
.endif

	# ZBA
	sh1add	a0, a1, a2
	sh2add	a0, a1, a2
	sh3add	a0, a1, a2
.ifdef __64_bit__
	sh1add.uw	a0, a1, a2
	sh2add.uw	a0, a1, a2
	sh3add.uw	a0, a1, a2
	add.uw		a0, a1, a2
	slli.uw		a0, a1, 0
	slli.uw		a0, a1, 63
.endif

	# ZBB
	clz	a0, a1
	ctz	a0, a1
	cpop	a0, a1
	min	a0, a1, a2
	max	a0, a1, a2
	minu	a0, a1, a2
	maxu	a0, a1, a2
	sext.b	a0, a1
	sext.h	a0, a1
	andn	a0, a1, a2
	orn	a0, a1, a2
	xor	a0, a1, a2
	rori	a0, a1, 0
	rori	a0, a1, 31
	ror	a0, a1, a2
	ror	a0, a1, 0
	ror	a0, a1, 31
	rol	a0, a1, a2
.ifdef __64_bit__
	clzw	a0, a1
	ctzw	a0, a1
	cpopw	a0, a1
	rori	a0, a1, 63
	ror	a0, a1, 63
	roriw	a0, a1, 0
	roriw	a0, a1, 31
	rorw	a0, a1, a2
	rorw	a0, a1, 0
	rorw	a0, a1, 31
	rolw	a0, a1, a2
.endif

	# ZBC
	clmul	a0, a1, a2
	clmulh	a0, a1, a2
	clmulr	a0, a1, a2
