#as: -march=morello+c64 --defsym C64MODE=1
#objdump: -dr
#source: morello_ldst.s

.*:     file format .*


Disassembly of section \.text:

0000000000000000 <.text>:
.*:	a2e47f02 	casa	c4, c2, \[c24\]
.*:	a2e4ff02 	casal	c4, c2, \[c24\]
.*:	a2a47f02 	cas	c4, c2, \[c24\]
.*:	a2a4ff02 	casl	c4, c2, \[c24\]
.*:	a2e47fe2 	casa	c4, c2, \[csp\]
.*:	a2e4ffe2 	casal	c4, c2, \[csp\]
.*:	a2a47fe2 	cas	c4, c2, \[csp\]
.*:	a2a4ffe2 	casl	c4, c2, \[csp\]
.*:	a2a48302 	swpa	c4, c2, \[c24\]
.*:	a2e48302 	swpal	c4, c2, \[c24\]
.*:	a2248302 	swp	c4, c2, \[c24\]
.*:	a2648302 	swpl	c4, c2, \[c24\]
.*:	a2a483e2 	swpa	c4, c2, \[csp\]
.*:	a2e483e2 	swpal	c4, c2, \[csp\]
.*:	a22483e2 	swp	c4, c2, \[csp\]
.*:	a26483e2 	swpl	c4, c2, \[csp\]
.*:	a23fc302 	ldapr	c2, \[c24\]
.*:	425fff02 	ldar	c2, \[c24\]
.*:	225fff02 	ldaxr	c2, \[c24\]
.*:	225f7f02 	ldxr	c2, \[c24\]
.*:	421fff02 	stlr	c2, \[c24\]
.*:	a23fc3e2 	ldapr	c2, \[csp\]
.*:	425fffe2 	ldar	c2, \[csp\]
.*:	225fffe2 	ldaxr	c2, \[csp\]
.*:	225f7fe2 	ldxr	c2, \[csp\]
.*:	421fffe2 	stlr	c2, \[csp\]
.*:	c2c4b302 	ldct	x2, \[c24\]
.*:	c2c49302 	stct	x2, \[c24\]
.*:	c2c4b3e2 	ldct	x2, \[csp\]
.*:	c2c493e2 	stct	x2, \[csp\]
.*:	227f8f02 	ldaxp	c2, c3, \[c24\]
.*:	227f0f02 	ldxp	c2, c3, \[c24\]
.*:	227f97e2 	ldaxp	c2, c5, \[csp\]
.*:	227f17e2 	ldxp	c2, c5, \[csp\]
.*:	2202ff01 	stlxr	w2, c1, \[c24\]
.*:	22027f01 	stxr	w2, c1, \[c24\]
.*:	2202fffd 	stlxr	w2, c29, \[csp\]
.*:	22027ffd 	stxr	w2, c29, \[csp\]
.*:	22229704 	stlxp	w2, c4, c5, \[c24\]
.*:	22221704 	stxp	w2, c4, c5, \[c24\]
.*:	2222f7e4 	stlxp	w2, c4, c29, \[csp\]
.*:	222277e4 	stxp	w2, c4, c29, \[csp\]
.*:	c27fff04 	ldr	c4, \[c24, #65520\]
.*:	c2400b04 	ldr	c4, \[c24, #32\]
.*:	c2400304 	ldr	c4, \[c24\]
.*:	c23fff04 	str	c4, \[c24, #65520\]
.*:	c2000b04 	str	c4, \[c24, #32\]
.*:	c2000304 	str	c4, \[c24\]
.*:	c27fffe4 	ldr	c4, \[csp, #65520\]
.*:	c2400be4 	ldr	c4, \[csp, #32\]
.*:	c24003e4 	ldr	c4, \[csp\]
.*:	c23fffe4 	str	c4, \[csp, #65520\]
.*:	c2000be4 	str	c4, \[csp, #32\]
.*:	c20003e4 	str	c4, \[csp\]
.*:	a24ff304 	ldur	c4, \[c24, #255\]
.*:	a2501304 	ldur	c4, \[c24, #-255\]
.*:	a2400304 	ldur	c4, \[c24\]
.*:	a2410304 	ldur	c4, \[c24, #16\]
.*:	a24ffb04 	ldtr	c4, \[c24, #255\]
.*:	a2501b04 	ldtr	c4, \[c24, #-255\]
.*:	a2400b04 	ldtr	c4, \[c24\]
.*:	a2410b04 	ldtr	c4, \[c24, #16\]
.*:	a20ff304 	stur	c4, \[c24, #255\]
.*:	a2101304 	stur	c4, \[c24, #-255\]
.*:	a2000304 	stur	c4, \[c24\]
.*:	a2010304 	stur	c4, \[c24, #16\]
.*:	a20ffb04 	sttr	c4, \[c24, #255\]
.*:	a2101b04 	sttr	c4, \[c24, #-255\]
.*:	a2000b04 	sttr	c4, \[c24\]
.*:	a2010b04 	sttr	c4, \[c24, #16\]
.*:	a24ff3e4 	ldur	c4, \[csp, #255\]
.*:	a25013e4 	ldur	c4, \[csp, #-255\]
.*:	a24003e4 	ldur	c4, \[csp\]
.*:	a24103e4 	ldur	c4, \[csp, #16\]
.*:	a24ffbe4 	ldtr	c4, \[csp, #255\]
.*:	a2501be4 	ldtr	c4, \[csp, #-255\]
.*:	a2400be4 	ldtr	c4, \[csp\]
.*:	a2410be4 	ldtr	c4, \[csp, #16\]
.*:	a20ff3e4 	stur	c4, \[csp, #255\]
.*:	a21013e4 	stur	c4, \[csp, #-255\]
.*:	a20003e4 	stur	c4, \[csp\]
.*:	a20103e4 	stur	c4, \[csp, #16\]
.*:	a20ffbe4 	sttr	c4, \[csp, #255\]
.*:	a2101be4 	sttr	c4, \[csp, #-255\]
.*:	a2000be4 	sttr	c4, \[csp\]
.*:	a2010be4 	sttr	c4, \[csp, #16\]
.*:	42df9704 	ldp	c4, c5, \[c24, #1008\]
.*:	42e01704 	ldp	c4, c5, \[c24, #-1024\]
.*:	42c09704 	ldp	c4, c5, \[c24, #16\]
.*:	42ff1704 	ldp	c4, c5, \[c24, #-32\]
.*:	429f9704 	stp	c4, c5, \[c24, #1008\]
.*:	42a01704 	stp	c4, c5, \[c24, #-1024\]
.*:	42809704 	stp	c4, c5, \[c24, #16\]
.*:	42bf1704 	stp	c4, c5, \[c24, #-32\]
.*:	625f9704 	ldnp	c4, c5, \[c24, #1008\]
.*:	62601704 	ldnp	c4, c5, \[c24, #-1024\]
.*:	62409704 	ldnp	c4, c5, \[c24, #16\]
.*:	627f1704 	ldnp	c4, c5, \[c24, #-32\]
.*:	621f9704 	stnp	c4, c5, \[c24, #1008\]
.*:	62201704 	stnp	c4, c5, \[c24, #-1024\]
.*:	62009704 	stnp	c4, c5, \[c24, #16\]
.*:	623f1704 	stnp	c4, c5, \[c24, #-32\]
.*:	42df9be4 	ldp	c4, c6, \[csp, #1008\]
.*:	42e01be4 	ldp	c4, c6, \[csp, #-1024\]
.*:	42c09be4 	ldp	c4, c6, \[csp, #16\]
.*:	42ff1be4 	ldp	c4, c6, \[csp, #-32\]
.*:	429f9be4 	stp	c4, c6, \[csp, #1008\]
.*:	42a01be4 	stp	c4, c6, \[csp, #-1024\]
.*:	42809be4 	stp	c4, c6, \[csp, #16\]
.*:	42bf1be4 	stp	c4, c6, \[csp, #-32\]
.*:	625f9be4 	ldnp	c4, c6, \[csp, #1008\]
.*:	62601be4 	ldnp	c4, c6, \[csp, #-1024\]
.*:	62409be4 	ldnp	c4, c6, \[csp, #16\]
.*:	627f1be4 	ldnp	c4, c6, \[csp, #-32\]
.*:	621f9be4 	stnp	c4, c6, \[csp, #1008\]
.*:	62201be4 	stnp	c4, c6, \[csp, #-1024\]
.*:	62009be4 	stnp	c4, c6, \[csp, #16\]
.*:	623f1be4 	stnp	c4, c6, \[csp, #-32\]
.*:	a2402f04 	ldr	c4, \[c24, #32\]!
.*:	a2402704 	ldr	c4, \[c24\], #32
.*:	a2500f04 	ldr	c4, \[c24, #-4096\]!
.*:	a2500704 	ldr	c4, \[c24\], #-4096
.*:	a24fff04 	ldr	c4, \[c24, #4080\]!
.*:	a24ff704 	ldr	c4, \[c24\], #4080
.*:	a2002f04 	str	c4, \[c24, #32\]!
.*:	a2002704 	str	c4, \[c24\], #32
.*:	a2100f04 	str	c4, \[c24, #-4096\]!
.*:	a2100704 	str	c4, \[c24\], #-4096
.*:	a20fff04 	str	c4, \[c24, #4080\]!
.*:	a20ff704 	str	c4, \[c24\], #4080
.*:	a2402fe4 	ldr	c4, \[csp, #32\]!
.*:	a24027e4 	ldr	c4, \[csp\], #32
.*:	a2500fe4 	ldr	c4, \[csp, #-4096\]!
.*:	a25007e4 	ldr	c4, \[csp\], #-4096
.*:	a24fffe4 	ldr	c4, \[csp, #4080\]!
.*:	a24ff7e4 	ldr	c4, \[csp\], #4080
.*:	a2002fe4 	str	c4, \[csp, #32\]!
.*:	a20027e4 	str	c4, \[csp\], #32
.*:	a2100fe4 	str	c4, \[csp, #-4096\]!
.*:	a21007e4 	str	c4, \[csp\], #-4096
.*:	a20fffe4 	str	c4, \[csp, #4080\]!
.*:	a20ff7e4 	str	c4, \[csp\], #4080
.*:	62c11303 	ldp	c3, c4, \[c24, #32\]!
.*:	22c11303 	ldp	c3, c4, \[c24\], #32
.*:	62e01303 	ldp	c3, c4, \[c24, #-1024\]!
.*:	22e01303 	ldp	c3, c4, \[c24\], #-1024
.*:	62df9303 	ldp	c3, c4, \[c24, #1008\]!
.*:	22df9303 	ldp	c3, c4, \[c24\], #1008
.*:	62811303 	stp	c3, c4, \[c24, #32\]!
.*:	22811303 	stp	c3, c4, \[c24\], #32
.*:	62a01303 	stp	c3, c4, \[c24, #-1024\]!
.*:	22a01303 	stp	c3, c4, \[c24\], #-1024
.*:	629f9303 	stp	c3, c4, \[c24, #1008\]!
.*:	229f9303 	stp	c3, c4, \[c24\], #1008
.*:	62c117e4 	ldp	c4, c5, \[csp, #32\]!
.*:	22c117e4 	ldp	c4, c5, \[csp\], #32
.*:	62e017e4 	ldp	c4, c5, \[csp, #-1024\]!
.*:	22e017e4 	ldp	c4, c5, \[csp\], #-1024
.*:	62df97e4 	ldp	c4, c5, \[csp, #1008\]!
.*:	22df97e4 	ldp	c4, c5, \[csp\], #1008
.*:	628117e4 	stp	c4, c5, \[csp, #32\]!
.*:	228117e4 	stp	c4, c5, \[csp\], #32
.*:	62a017e4 	stp	c4, c5, \[csp, #-1024\]!
.*:	22a017e4 	stp	c4, c5, \[csp\], #-1024
.*:	629f97e4 	stp	c4, c5, \[csp, #1008\]!
.*:	229f97e4 	stp	c4, c5, \[csp\], #1008
.*:	a2636b04 	ldr	c4, \[c24, x3\]
.*:	a2636b04 	ldr	c4, \[c24, x3\]
.*:	a2635b04 	ldr	c4, \[c24, w3, uxtw #4\]
.*:	a263db04 	ldr	c4, \[c24, w3, sxtw #4\]
.*:	a263cb04 	ldr	c4, \[c24, w3, sxtw\]
.*:	a2236b04 	str	c4, \[c24, x3\]
.*:	a2236b04 	str	c4, \[c24, x3\]
.*:	a2235b04 	str	c4, \[c24, w3, uxtw #4\]
.*:	a223db04 	str	c4, \[c24, w3, sxtw #4\]
.*:	a223cb04 	str	c4, \[c24, w3, sxtw\]
.*:	a2636be4 	ldr	c4, \[csp, x3\]
.*:	a2636be4 	ldr	c4, \[csp, x3\]
.*:	a2635be4 	ldr	c4, \[csp, w3, uxtw #4\]
.*:	a263dbe4 	ldr	c4, \[csp, w3, sxtw #4\]
.*:	a263cbe4 	ldr	c4, \[csp, w3, sxtw\]
.*:	a2236be4 	str	c4, \[csp, x3\]
.*:	a2236be4 	str	c4, \[csp, x3\]
.*:	a2235be4 	str	c4, \[csp, w3, uxtw #4\]
.*:	a223dbe4 	str	c4, \[csp, w3, sxtw #4\]
.*:	a223cbe4 	str	c4, \[csp, w3, sxtw\]
.*:	c2d813e1 	blr	\[csp, #-1024\]
.*:	c2d7f3e1 	blr	\[csp, #1008\]
.*:	c2d033e1 	blr	\[csp, #16\]
.*:	c2d013e1 	blr	\[csp\]
.*:	c2d813e0 	br	\[csp, #-1024\]
.*:	c2d7f3e0 	br	\[csp, #1008\]
.*:	c2d033e0 	br	\[csp, #16\]
.*:	c2d013e0 	br	\[csp\]
.*:	c2d81041 	blr	\[c2, #-1024\]
.*:	c2d7f041 	blr	\[c2, #1008\]
.*:	c2d03041 	blr	\[c2, #16\]
.*:	c2d01041 	blr	\[c2\]
.*:	c2d81040 	br	\[c2, #-1024\]
.*:	c2d7f040 	br	\[c2, #1008\]
.*:	c2d03040 	br	\[c2, #16\]
.*:	c2d01040 	br	\[c2\]
.*:	c2c433e2 	ldpblr	c2, \[csp\]
.*:	c2c413e2 	ldpbr	c2, \[csp\]
.*:	c2c43042 	ldpblr	c2, \[c2\]
.*:	c2c41042 	ldpbr	c2, \[c2\]
.*:	425f7d82 	ldar	c2, \[x12\]
.*:	421f7d82 	stlr	c2, \[x12\]
.*:	425f7fe2 	ldar	c2, \[sp\]
.*:	421f7fe2 	stlr	c2, \[sp\]
.*:	427f7d82 	ldarb	w2, \[x12\]
.*:	427ffd82 	ldar	w2, \[x12\]
.*:	423f7d82 	stlrb	w2, \[x12\]
.*:	423ffd82 	stlr	w2, \[x12\]
.*:	427f7fe2 	ldarb	w2, \[sp\]
.*:	427fffe2 	ldar	w2, \[sp\]
.*:	423f7fe2 	stlrb	w2, \[sp\]
.*:	423fffe2 	stlr	w2, \[sp\]
