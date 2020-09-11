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
