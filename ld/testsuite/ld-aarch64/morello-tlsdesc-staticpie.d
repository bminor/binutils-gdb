#source: morello-tlsdesc.s
#as: -march=morello+c64 --defsym STATIC=1
#ld: -pie
#objdump: -DR -j .got.plt -j .text -j .plt

.*:     file format .*


Disassembly of section .plt:

.*<.plt>:
.*:	62bf7bf0 	stp	c16, c30, \[csp, #-32\]!
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	62bf8fe2 	stp	c2, c3, \[csp, #-16\]!
.*:	908...82 	adrp	c2, .*
.*:	908...83 	adrp	c3, .*
.*:	c2....42 	ldr	c2, \[c2, #[0-9]+\]
.*:	02....63 	add	c3, c3, #0x[0-9a-f]+
.*:	c2c21040 	br	c2
.*:	d503201f 	nop
.*:	d503201f 	nop

Disassembly of section .text:

.*<f2>:
.*:	94000008 	bl	.* <_start>
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d503201f 	nop
.*:	908...80 	adrp	c0, .*
.*:	c2....01 	ldr	c1, \[c0, #[0-9]+\]
.*:	02....00 	add	c0, c0, #0x[0-9a-f]+
.*:	c2c23020 	blr	c1
.*:	c2c10000 	scbnds	c0, c0, x1

.*<_start>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d2a00001 	movz	x1, #0x0, lsl #16
.*:	d2a00000 	movz	x0, #0x0, lsl #16
.*:	f2800480 	movk	x0, #0x24
.*:	f2800281 	movk	x1, #0x14
.*:	c2....40 	add	c0, c2, x0, uxtx
.*:	c2c10000 	scbnds	c0, c0, x1

Disassembly of section .got.plt:

.*:
	...
			.*: R_MORELLO_TLSDESC	\*ABS\*
