#as: -march=morello+c64
#objdump: -dr
#source: morello_insn.s

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
