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
