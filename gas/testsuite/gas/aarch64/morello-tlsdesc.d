#source: morello-tlsdesc.s
#as: -march=morello+c64
#objdump: -dr

.*:     file format .*


Disassembly of section .text:

.*<bar>:
.*:	90800000 	adrp	c0, 0 <foo>
			.*: R_MORELLO_TLSDESC_ADR_PAGE20	foo
.*:	c2400001 	ldr	c1, \[c0\]
			.*: R_MORELLO_TLSDESC_LD128_LO12	foo
.*:	c2c23020 	blr	c1
			.*: R_MORELLO_TLSDESC_CALL	foo
