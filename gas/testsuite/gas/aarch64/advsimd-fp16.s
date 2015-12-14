/* simdhp.s Test file for AArch64 half-precision floating-point
   vector instructions.  */

	/* Vector three-same.  */

	.macro three_same, op
	\op	v1.2d, v2.2d, v3.2d
	\op	v1.2s, v2.2s, v3.2s
	\op	v1.4s, v2.4s, v3.4s
	\op	v0.4h, v0.4h, v0.4h
	\op	v1.4h, v2.4h, v3.4h
	\op	v0.8h, v0.8h, v0.8h
	\op	v1.8h, v2.8h, v3.8h
	.endm

	.text

	three_same fmaxnm
	three_same fmaxnmp
	three_same fminnm
	three_same fminnmp
	three_same fmla
	three_same fmls
	three_same fadd
	three_same faddp
	three_same fsub
	three_same fmulx
	three_same fmul
	three_same fcmeq
	three_same fcmge
	three_same fcmgt
	three_same facge
	three_same facgt
	three_same fmax
	three_same fmaxp
	three_same fmin
	three_same fminp
	three_same frecps
	three_same fdiv
	three_same frsqrts

	/* Scalar three-same.  */

	.macro sthree_same, op
	\op	d0, d1, d2
	\op	s0, s1, s2
	\op	h0, h1, h2
	\op	h0, h0, h0
	.endm

	sthree_same fabd
	sthree_same fmulx
	sthree_same fcmeq
	sthree_same fcmgt
	sthree_same fcmge
	sthree_same facge
	sthree_same facgt
	sthree_same frecps
	sthree_same frsqrts
