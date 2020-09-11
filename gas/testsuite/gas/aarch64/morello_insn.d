#as: -march=morello
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

.* <.text>:
.*:	c2c273e0 	bx	#4
.*:	d65f03c0 	ret
.*:	c2c23280 	blr	c20
.*:	c2c23283 	blrr	c20
.*:	c2c23282 	blrs	c20
.*:	c2c21280 	br	c20
.*:	c2c21283 	brr	c20
.*:	c2c21282 	brs	c20
.*:	c2c25280 	ret	c20
.*:	c2c25283 	retr	c20
.*:	c2c25282 	rets	c20
.*:	c2c213e1 	chksld	csp
.*:	c2c233e1 	chktgd	csp
.*:	c2c1d26b 	mov	c11, c19
.*:	c2c1d26b 	mov	c11, c19
.*:	c2d38561 	chkss	c11, c19
.*:	c2c1926b 	clrtag	c11, c19
.*:	c2c1d3eb 	mov	c11, csp
.*:	c2c1d3eb 	mov	c11, csp
.*:	c2df8561 	chkss	c11, csp
.*:	c2c193eb 	clrtag	c11, csp
.*:	c2c1d17f 	mov	csp, c11
.*:	c2c1d17f 	mov	csp, c11
.*:	c2cb87e1 	chkss	csp, c11
.*:	c2c1917f 	clrtag	csp, c11
.*:	c2c1d3ff 	mov	csp, csp
.*:	c2c1d3ff 	mov	csp, csp
.*:	c2df87e1 	chkss	csp, csp
.*:	c2c193ff 	clrtag	csp, csp
.*:	aa1f03e0 	mov	x0, xzr
.*:	c2c59020 	cvtd	c0, x1
.*:	c2c5d020 	cvtdz	c0, x1
.*:	c2c5b020 	cvtp	c0, x1
.*:	c2c5f020 	cvtpz	c0, x1
.*:	c2c71022 	rrlen	x2, x1
.*:	c2c73022 	rrmask	x2, x1
.*:	c2c151e7 	cfhi	x7, c15
.*:	c2c511e7 	cvtd	x7, c15
.*:	c2c531e7 	cvtp	x7, c15
.*:	c2c153e6 	cfhi	x6, csp
.*:	c2c513e6 	cvtd	x6, csp
.*:	c2c533e6 	cvtp	x6, csp
.*:	c2c011e7 	gcbase	x7, c15
.*:	c2c131e7 	gcflgs	x7, c15
.*:	c2c031e7 	gclen	x7, c15
.*:	c2c111e7 	gclim	x7, c15
.*:	c2c071e7 	gcoff	x7, c15
.*:	c2c0d1e7 	gcperm	x7, c15
.*:	c2c0b1e7 	gcseal	x7, c15
.*:	c2c091e7 	gctag	x7, c15
.*:	c2c0f1e7 	gctype	x7, c15
.*:	c2c051e7 	gcvalue	x7, c15
.*:	c2c013e6 	gcbase	x6, csp
.*:	c2c133e6 	gcflgs	x6, csp
.*:	c2c033e6 	gclen	x6, csp
.*:	c2c113e6 	gclim	x6, csp
.*:	c2c073e6 	gcoff	x6, csp
.*:	c2c0d3e6 	gcperm	x6, csp
.*:	c2c0b3e6 	gcseal	x6, csp
.*:	c2c093e6 	gctag	x6, csp
.*:	c2c0f3e6 	gctype	x6, csp
.*:	c2c053e6 	gcvalue	x6, csp
.*:	c2d4a7e1 	chkeq	csp, c20
.*:	c2d4a661 	chkeq	c19, c20
.*:	023fc135 	add	c21, c9, #0xff0
.*:	023ffd35 	add	c21, c9, #0xfff
.*:	0247f935 	add	c21, c9, #0x1fe, lsl #12
.*:	02000d35 	add	c21, c9, #0x3
.*:	02400135 	add	c21, c9, #0x0, lsl #12
.*:	02bfc135 	sub	c21, c9, #0xff0
.*:	02bffd35 	sub	c21, c9, #0xfff
.*:	02c7f935 	sub	c21, c9, #0x1fe, lsl #12
.*:	02800d35 	sub	c21, c9, #0x3
.*:	02c00135 	sub	c21, c9, #0x0, lsl #12
.*:	023fc13f 	add	csp, c9, #0xff0
.*:	023ffd3f 	add	csp, c9, #0xfff
.*:	0247f93f 	add	csp, c9, #0x1fe, lsl #12
.*:	02000d3f 	add	csp, c9, #0x3
.*:	0240013f 	add	csp, c9, #0x0, lsl #12
.*:	02bfc13f 	sub	csp, c9, #0xff0
.*:	02bffd3f 	sub	csp, c9, #0xfff
.*:	02c7f93f 	sub	csp, c9, #0x1fe, lsl #12
.*:	02800d3f 	sub	csp, c9, #0x3
.*:	02c0013f 	sub	csp, c9, #0x0, lsl #12
.*:	023fc3ff 	add	csp, csp, #0xff0
.*:	023fffff 	add	csp, csp, #0xfff
.*:	0247fbff 	add	csp, csp, #0x1fe, lsl #12
.*:	02000fff 	add	csp, csp, #0x3
.*:	024003ff 	add	csp, csp, #0x0, lsl #12
.*:	02bfc3ff 	sub	csp, csp, #0xff0
.*:	02bfffff 	sub	csp, csp, #0xfff
.*:	02c7fbff 	sub	csp, csp, #0x1fe, lsl #12
.*:	02800fff 	sub	csp, csp, #0x3
.*:	02c003ff 	sub	csp, csp, #0x0, lsl #12
.*:	023fc3f5 	add	c21, csp, #0xff0
.*:	023ffff5 	add	c21, csp, #0xfff
.*:	0247fbf5 	add	c21, csp, #0x1fe, lsl #12
.*:	02000ff5 	add	c21, csp, #0x3
.*:	024003f5 	add	c21, csp, #0x0, lsl #12
.*:	02bfc3f5 	sub	c21, csp, #0xff0
.*:	02bffff5 	sub	c21, csp, #0xfff
.*:	02c7fbf5 	sub	c21, csp, #0x1fe, lsl #12
.*:	02800ff5 	sub	c21, csp, #0x3
.*:	02c003f5 	sub	c21, csp, #0x0, lsl #12
.*:	c2ffe0c7 	bicflgs	c7, c6, #255
.*:	c2e000c7 	bicflgs	c7, c6, #0
.*:	c2ffe0c7 	bicflgs	c7, c6, #255
.*:	c2e200c7 	bicflgs	c7, c6, #16
.*:	c2fff0c7 	eorflgs	c7, c6, #255
.*:	c2e010c7 	eorflgs	c7, c6, #0
.*:	c2fff0c7 	eorflgs	c7, c6, #255
.*:	c2e210c7 	eorflgs	c7, c6, #16
.*:	c2ffe8c7 	orrflgs	c7, c6, #255
.*:	c2e008c7 	orrflgs	c7, c6, #0
.*:	c2ffe8c7 	orrflgs	c7, c6, #255
.*:	c2e208c7 	orrflgs	c7, c6, #16
.*:	c2ffe0df 	bicflgs	csp, c6, #255
.*:	c2e000df 	bicflgs	csp, c6, #0
.*:	c2ffe0df 	bicflgs	csp, c6, #255
.*:	c2e200df 	bicflgs	csp, c6, #16
.*:	c2fff0df 	eorflgs	csp, c6, #255
.*:	c2e010df 	eorflgs	csp, c6, #0
.*:	c2fff0df 	eorflgs	csp, c6, #255
.*:	c2e210df 	eorflgs	csp, c6, #16
.*:	c2ffe8df 	orrflgs	csp, c6, #255
.*:	c2e008df 	orrflgs	csp, c6, #0
.*:	c2ffe8df 	orrflgs	csp, c6, #255
.*:	c2e208df 	orrflgs	csp, c6, #16
.*:	c2ffe3e8 	bicflgs	c8, csp, #255
.*:	c2e003e8 	bicflgs	c8, csp, #0
.*:	c2ffe3e8 	bicflgs	c8, csp, #255
.*:	c2e203e8 	bicflgs	c8, csp, #16
.*:	c2fff3e8 	eorflgs	c8, csp, #255
.*:	c2e013e8 	eorflgs	c8, csp, #0
.*:	c2fff3e8 	eorflgs	c8, csp, #255
.*:	c2e213e8 	eorflgs	c8, csp, #16
.*:	c2ffebe8 	orrflgs	c8, csp, #255
.*:	c2e00be8 	orrflgs	c8, csp, #0
.*:	c2ffebe8 	orrflgs	c8, csp, #255
.*:	c2e20be8 	orrflgs	c8, csp, #16
.*:	c2ffe3ff 	bicflgs	csp, csp, #255
.*:	c2e003ff 	bicflgs	csp, csp, #0
.*:	c2ffe3ff 	bicflgs	csp, csp, #255
.*:	c2e203ff 	bicflgs	csp, csp, #16
.*:	c2fff3ff 	eorflgs	csp, csp, #255
.*:	c2e013ff 	eorflgs	csp, csp, #0
.*:	c2fff3ff 	eorflgs	csp, csp, #255
.*:	c2e213ff 	eorflgs	csp, csp, #16
.*:	c2ffebff 	orrflgs	csp, csp, #255
.*:	c2e00bff 	orrflgs	csp, csp, #0
.*:	c2ffebff 	orrflgs	csp, csp, #255
.*:	c2e20bff 	orrflgs	csp, csp, #16
.*:	c2df9a11 	alignd	c17, c16, #63
.*:	c2cf1a11 	alignd	c17, c16, #30
.*:	c2c01a11 	alignd	c17, c16, #0
.*:	c2d01a11 	alignd	c17, c16, #32
.*:	c2dfda11 	alignu	c17, c16, #63
.*:	c2cf5a11 	alignu	c17, c16, #30
.*:	c2c05a11 	alignu	c17, c16, #0
.*:	c2d05a11 	alignu	c17, c16, #32
.*:	c2dfba11 	scbnds	c17, c16, #0x3f
.*:	c2cf3a11 	scbnds	c17, c16, #0x1e
.*:	c2c03a11 	scbnds	c17, c16, #0x0
.*:	c2d03a11 	scbnds	c17, c16, #0x20
.*:	c2df9a1f 	alignd	csp, c16, #63
.*:	c2cf1a1f 	alignd	csp, c16, #30
.*:	c2c01a1f 	alignd	csp, c16, #0
.*:	c2d01a1f 	alignd	csp, c16, #32
.*:	c2dfda1f 	alignu	csp, c16, #63
.*:	c2cf5a1f 	alignu	csp, c16, #30
.*:	c2c05a1f 	alignu	csp, c16, #0
.*:	c2d05a1f 	alignu	csp, c16, #32
.*:	c2dfba1f 	scbnds	csp, c16, #0x3f
.*:	c2cf3a1f 	scbnds	csp, c16, #0x1e
.*:	c2c03a1f 	scbnds	csp, c16, #0x0
.*:	c2d03a1f 	scbnds	csp, c16, #0x20
.*:	c2df9bf2 	alignd	c18, csp, #63
.*:	c2cf1bf2 	alignd	c18, csp, #30
.*:	c2c01bf2 	alignd	c18, csp, #0
.*:	c2d01bf2 	alignd	c18, csp, #32
.*:	c2dfdbf2 	alignu	c18, csp, #63
.*:	c2cf5bf2 	alignu	c18, csp, #30
.*:	c2c05bf2 	alignu	c18, csp, #0
.*:	c2d05bf2 	alignu	c18, csp, #32
.*:	c2dfbbf2 	scbnds	c18, csp, #0x3f
.*:	c2cf3bf2 	scbnds	c18, csp, #0x1e
.*:	c2c03bf2 	scbnds	c18, csp, #0x0
.*:	c2d03bf2 	scbnds	c18, csp, #0x20
.*:	c2df9bff 	alignd	csp, csp, #63
.*:	c2cf1bff 	alignd	csp, csp, #30
.*:	c2c01bff 	alignd	csp, csp, #0
.*:	c2d01bff 	alignd	csp, csp, #32
.*:	c2dfdbff 	alignu	csp, csp, #63
.*:	c2cf5bff 	alignu	csp, csp, #30
.*:	c2c05bff 	alignu	csp, csp, #0
.*:	c2d05bff 	alignu	csp, csp, #32
.*:	c2dfbbff 	scbnds	csp, csp, #0x3f
.*:	c2cf3bff 	scbnds	csp, csp, #0x1e
.*:	c2c03bff 	scbnds	csp, csp, #0x0
.*:	c2d03bff 	scbnds	csp, csp, #0x20
.*:	c2c692f6 	clrperm	c22, c23, r
.*:	c2c652f6 	clrperm	c22, c23, w
.*:	c2c632f6 	clrperm	c22, c23, x
.*:	c2c6b2f6 	clrperm	c22, c23, rx
.*:	c2c672f6 	clrperm	c22, c23, wx
.*:	c2c6d2f6 	clrperm	c22, c23, rw
.*:	c2c6f2f6 	clrperm	c22, c23, rwx
.*:	c2c612f6 	clrperm	c22, c23, #0
.*:	c2c632f6 	clrperm	c22, c23, x
.*:	c2c652f6 	clrperm	c22, c23, w
.*:	c2c672f6 	clrperm	c22, c23, wx
.*:	c2c692f6 	clrperm	c22, c23, r
.*:	c2c6b2f6 	clrperm	c22, c23, rx
.*:	c2c6d2f6 	clrperm	c22, c23, rw
.*:	c2c6f2f6 	clrperm	c22, c23, rwx
.*:	c2dffa11 	scbnds	c17, c16, #0x3f, lsl #4
.*:	c2cf7a11 	scbnds	c17, c16, #0x1e, lsl #4
.*:	c2c07a11 	scbnds	c17, c16, #0x0, lsl #4
.*:	c2d07a11 	scbnds	c17, c16, #0x20, lsl #4
.*:	c2dffa11 	scbnds	c17, c16, #0x3f, lsl #4
.*:	c2c27a11 	scbnds	c17, c16, #0x4, lsl #4
.*:	c2dffa1f 	scbnds	csp, c16, #0x3f, lsl #4
.*:	c2cf7a1f 	scbnds	csp, c16, #0x1e, lsl #4
.*:	c2c07a1f 	scbnds	csp, c16, #0x0, lsl #4
.*:	c2d07a1f 	scbnds	csp, c16, #0x20, lsl #4
.*:	c2dffa1f 	scbnds	csp, c16, #0x3f, lsl #4
.*:	c2c27a1f 	scbnds	csp, c16, #0x4, lsl #4
.*:	c2dffbf2 	scbnds	c18, csp, #0x3f, lsl #4
.*:	c2cf7bf2 	scbnds	c18, csp, #0x1e, lsl #4
.*:	c2c07bf2 	scbnds	c18, csp, #0x0, lsl #4
.*:	c2d07bf2 	scbnds	c18, csp, #0x20, lsl #4
.*:	c2dffbf2 	scbnds	c18, csp, #0x3f, lsl #4
.*:	c2c27bf2 	scbnds	c18, csp, #0x4, lsl #4
.*:	c2dffbff 	scbnds	csp, csp, #0x3f, lsl #4
.*:	c2cf7bff 	scbnds	csp, csp, #0x1e, lsl #4
.*:	c2c07bff 	scbnds	csp, csp, #0x0, lsl #4
.*:	c2d07bff 	scbnds	csp, csp, #0x20, lsl #4
.*:	c2dffbff 	scbnds	csp, csp, #0x3f, lsl #4
.*:	c2c27bff 	scbnds	csp, csp, #0x4, lsl #4
.*:	c2c33062 	seal	c2, c3, rb
.*:	c2c37062 	seal	c2, c3, lb
.*:	c2c35062 	seal	c2, c3, lpb
.*:	c2d928c7 	bicflgs	c7, c6, x25
.*:	c2d9a8c7 	eorflgs	c7, c6, x25
.*:	c2d968c7 	orrflgs	c7, c6, x25
.*:	c2d9a0c7 	clrperm	c7, c6, x25
.*:	c2d900c7 	scbnds	c7, c6, x25
.*:	c2d920c7 	scbndse	c7, c6, x25
.*:	c2d960c7 	scoff	c7, c6, x25
.*:	c2d9e0c7 	scflgs	c7, c6, x25
.*:	c2d980c7 	sctag	c7, c6, x25
.*:	c2d940c7 	scvalue	c7, c6, x25
.*:	c2d92be7 	bicflgs	c7, csp, x25
.*:	c2d9abe7 	eorflgs	c7, csp, x25
.*:	c2d96be7 	orrflgs	c7, csp, x25
.*:	c2d9a3e7 	clrperm	c7, csp, x25
.*:	c2d903e7 	scbnds	c7, csp, x25
.*:	c2d923e7 	scbndse	c7, csp, x25
.*:	c2d963e7 	scoff	c7, csp, x25
.*:	c2d9e3e7 	scflgs	c7, csp, x25
.*:	c2d983e7 	sctag	c7, csp, x25
.*:	c2d943e7 	scvalue	c7, csp, x25
.*:	c2d928df 	bicflgs	csp, c6, x25
.*:	c2d9a8df 	eorflgs	csp, c6, x25
.*:	c2d968df 	orrflgs	csp, c6, x25
.*:	c2d9a0df 	clrperm	csp, c6, x25
.*:	c2d900df 	scbnds	csp, c6, x25
.*:	c2d920df 	scbndse	csp, c6, x25
.*:	c2d960df 	scoff	csp, c6, x25
.*:	c2d9e0df 	scflgs	csp, c6, x25
.*:	c2d980df 	sctag	csp, c6, x25
.*:	c2d940df 	scvalue	csp, c6, x25
.*:	c2d92bff 	bicflgs	csp, csp, x25
.*:	c2d9abff 	eorflgs	csp, csp, x25
.*:	c2d96bff 	orrflgs	csp, csp, x25
.*:	c2d9a3ff 	clrperm	csp, csp, x25
.*:	c2d903ff 	scbnds	csp, csp, x25
.*:	c2d923ff 	scbndse	csp, csp, x25
.*:	c2d963ff 	scoff	csp, csp, x25
.*:	c2d9e3ff 	scflgs	csp, csp, x25
.*:	c2d983ff 	sctag	csp, csp, x25
.*:	c2d943ff 	scvalue	csp, csp, x25
.*:	c2ee99a4 	subs	x4, c13, c14
.*:	c2c4a440 	blrs	c29, c2, c4
.*:	c2c48440 	brs	c29, c2, c4
.*:	c2c4c440 	rets	c29, c2, c4
.*:	c2cd2482 	cpytype	c2, c4, c13
.*:	c2cd6482 	cpyvalue	c2, c4, c13
.*:	c2cd0882 	seal	c2, c4, c13
.*:	c2cd4882 	unseal	c2, c4, c13
.*:	c2d9eac0 	cthi	c0, c22, x25
.*:	c2d9e89f 	cthi	csp, c4, x25
.*:	c2f91816 	cvt	c22, c0, x25
.*:	c2f95816 	cvtz	c22, c0, x25
.*:	c2f91be4 	cvt	c4, csp, x25
.*:	c2f95be4 	cvtz	c4, csp, x25
.*:	c2d9c016 	cvt	x22, c0, c25
.*:	c2d9c3e4 	cvt	x4, csp, c25
.*:	c2d688ed 	chkssu	c13, c7, c22
.*:	c2d68bed 	chkssu	c13, csp, c22
.*:	c2df88ed 	chkssu	c13, c7, csp
.*:	c2df8bed 	chkssu	c13, csp, csp
.*:	c2cd0482 	build	c2, c4, c13
.*:	c2cd4482 	cseal	c2, c4, c13
.*:	c2cd049f 	build	csp, c4, c13
.*:	c2cd449f 	cseal	csp, c4, c13
.*:	c2cd07e2 	build	c2, csp, c13
.*:	c2cd47e2 	cseal	c2, csp, c13
.*:	c2cd07ff 	build	csp, csp, c13
.*:	c2cd47ff 	cseal	csp, csp, c13
.*:	c2df0482 	build	c2, c4, csp
.*:	c2df4482 	cseal	c2, c4, csp
.*:	c2df049f 	build	csp, c4, csp
.*:	c2df449f 	cseal	csp, c4, csp
.*:	c2df07e2 	build	c2, csp, csp
.*:	c2df47e2 	cseal	c2, csp, csp
.*:	c2df07ff 	build	csp, csp, csp
.*:	c2df47ff 	cseal	csp, csp, csp
.*:	c2a4e131 	add	c17, c9, x4, sxtx
.*:	c2a4f131 	add	c17, c9, x4, sxtx #4
.*:	c2a4d131 	add	c17, c9, w4, sxtw #4
.*:	c2a46131 	add	c17, c9, x4, uxtx
.*:	c2a47131 	add	c17, c9, x4, uxtx #4
.*:	c2a4e13f 	add	csp, c9, x4, sxtx
.*:	c2a4f13f 	add	csp, c9, x4, sxtx #4
.*:	c2a4d13f 	add	csp, c9, w4, sxtw #4
.*:	c2a4613f 	add	csp, c9, x4, uxtx
.*:	c2a4713f 	add	csp, c9, x4, uxtx #4
.*:	c2a4e3f1 	add	c17, csp, x4, sxtx
.*:	c2a4f3f1 	add	c17, csp, x4, sxtx #4
.*:	c2a4d3f1 	add	c17, csp, w4, sxtw #4
.*:	c2a463f1 	add	c17, csp, x4, uxtx
.*:	c2a473f1 	add	c17, csp, x4, uxtx #4
.*:	c2a4e3ff 	add	csp, csp, x4, sxtx
.*:	c2a4f3ff 	add	csp, csp, x4, sxtx #4
.*:	c2a4d3ff 	add	csp, csp, w4, sxtw #4
.*:	c2a463ff 	add	csp, csp, x4, uxtx
.*:	c2a473ff 	add	csp, csp, x4, uxtx #4
.*:	c2c30ced 	csel	c13, c7, c3, eq  // eq = none
.*:	c2c31ced 	csel	c13, c7, c3, ne  // ne = any
.*:	c2c32ced 	csel	c13, c7, c3, cs  // cs = hs, nlast
.*:	c2c32ced 	csel	c13, c7, c3, cs  // cs = hs, nlast
.*:	c2c33ced 	csel	c13, c7, c3, cc  // cc = lo, ul, last
.*:	c2c33ced 	csel	c13, c7, c3, cc  // cc = lo, ul, last
.*:	c2c34ced 	csel	c13, c7, c3, mi  // mi = first
.*:	c2c35ced 	csel	c13, c7, c3, pl  // pl = nfrst
.*:	c2c36ced 	csel	c13, c7, c3, vs
.*:	c2c37ced 	csel	c13, c7, c3, vc
.*:	c2c38ced 	csel	c13, c7, c3, hi  // hi = pmore
.*:	c2c39ced 	csel	c13, c7, c3, ls  // ls = plast
.*:	c2c3aced 	csel	c13, c7, c3, ge  // ge = tcont
.*:	c2c3bced 	csel	c13, c7, c3, lt  // lt = tstop
.*:	c2c3cced 	csel	c13, c7, c3, gt
.*:	c2c3dced 	csel	c13, c7, c3, le
.*:	c2c3eced 	csel	c13, c7, c3, al
.*:	c2c3fced 	csel	c13, c7, c3, nv
