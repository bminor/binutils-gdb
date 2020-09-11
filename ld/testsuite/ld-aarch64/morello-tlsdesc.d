#source: morello-tlsdesc.s
#as: -march=morello+c64
#ld: -shared
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

.*<_start@plt>:
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17

.*<extf@plt>:
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17

.*<var1@plt>:
.*:	62bf8fe2 	stp	c2, c3, \[csp, #-16\]!
.*:	908...82 	adrp	c2, .*
.*:	908...83 	adrp	c3, .*
.*:	c2....42 	ldr	c2, \[c2, #[0-9]+\]

.*<var2@plt>:
.*:	02....63 	add	c3, c3, #0x[0-9a-f]+
.*:	c2c21040 	br	c2
.*:	d503201f 	nop
.*:	d503201f 	nop

Disassembly of section .text:

.*<f1>:
.*:	97fffff4 	bl	.* <extf@plt>
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d503201f 	nop
.*:	908...80 	adrp	c0, .*
.*:	c2....01 	ldr	c1, \[c0, #[0-9]+\]
.*:	02....00 	add	c0, c0, #0x[0-9a-f]+
.*:	c2c23020 	blr	c1
.*:	c2c10000 	scbnds	c0, c0, x1

.*<f2>:
.*:	97ffffe8 	bl	.* <_start@plt>
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d503201f 	nop
.*:	908...80 	adrp	c0, .*
.*:	c2....01 	ldr	c1, \[c0, #[0-9]+\]
.*:	02....00 	add	c0, c0, #0x[0-9a-f]+
.*:	c2c23020 	blr	c1
.*:	c2c10000 	scbnds	c0, c0, x1

.*<_start>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d503201f 	nop
.*:	908...80 	adrp	c0, .*
.*:	c2....01 	ldr	c1, \[c0, #[0-9]+\]
.*:	02....00 	add	c0, c0, #0x[0-9a-f]+
.*:	c2c23020 	blr	c1
.*:	c2c10000 	scbnds	c0, c0, x1

Disassembly of section .got.plt:

.*:
	...
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_JUMP_SLOT	_start
	...
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_JUMP_SLOT	extf
	...
			.*: R_MORELLO_TLSDESC	\*ABS\*\+0x4
			.*: R_MORELLO_TLSDESC	var1
			.*: R_MORELLO_TLSDESC	var2
