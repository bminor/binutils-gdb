#as: -march=morello
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

.* <.text>:
.*:	c2c1d26b 	mov	c11, c19
.*:	c2c1d26b 	mov	c11, c19
.*:	c2c1d3eb 	mov	c11, csp
.*:	c2c1d3eb 	mov	c11, csp
.*:	c2c1d17f 	mov	csp, c11
.*:	c2c1d17f 	mov	csp, c11
.*:	c2c1d3ff 	mov	csp, csp
.*:	c2c1d3ff 	mov	csp, csp
.*:	aa1f03e0 	mov	x0, xzr
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
