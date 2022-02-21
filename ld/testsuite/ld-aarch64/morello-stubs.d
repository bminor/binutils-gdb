#source: morello-stubs.s
#as:
#ld: -shared -T morello-stubs.ld
#objdump: -DR -j .text_low -j .text_high -j .got.plt -j .plt

.*:     file format .*


Disassembly of section .text_low:

.* <foo>:
.*:	94000008 	bl	[0-9a-f]+ <__baz_veneer>
.*:	c2c1d041 	mov	c1, c2
.*:	d65f03c0 	ret

.* <baz2>:
.*:	14000005 	b	[0-9a-f]+ <__baz_veneer>
.*:	c2c253c0 	ret	c30
.*:	00000000 	.*
.*:	14000008 	b	[0-9a-f]+ <__baz_veneer\+0x18>
.*:	d503201f 	nop

.* <__baz_veneer>:
.*:	900..... 	adrp	x16, [0-9a-f]+ <baz3>
.*:	910...10 	add	x16, x16, #0x[0-9a-f]+
.*:	d61f0200 	br	x16
	...

Disassembly of section .text_high:

.* <baz3>:
.*:	1400000a 	b	[0-9a-f]+ <___veneer>
.*:	d65f03c0 	ret

.* <baz>:
.*:	14000004 	b	[0-9a-f]+ <__foo_c64a64_veneer>
.*:	c2c253c0 	ret	c30
.*:	1400000c 	b	[0-9a-f]+ <.plt>
.*:	d503201f 	nop

.* <__foo_c64a64_veneer>:
.*:	90f..... 	adrp	c16, [0-9a-f]+ <.*>
.*:	020...10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21200 	br	c16
.*:	00000000 	.*

.* <___veneer>:
.*:	90f..... 	adrp	x16, .* <.*>
.*:	910...10 	add	x16, x16, #0x[0-9a-f]+
.*:	d61f0200 	br	x16
	...

Disassembly of section .plt:

.* <.plt>:
.*:	62bf7bf0 	stp	c16, c30, \[csp, #-32\]!
.*:	90800010 	adrp	c16, [0-9a-f]+ <baz3>
.*:	c24...11 	ldr	c17, \[c16, #[0-9]+\]
.*:	020...10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	90800010 	adrp	c16, [0-9a-f]+ <baz3>
.*:	c24...11 	ldr	c17, \[c16, #[0-9]+\]
.*:	020...10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17

Disassembly of section .got.plt:

.* <.got.plt>:
	...
.*:	0f000041 	.*
			.*: R_MORELLO_JUMP_SLOT	baz
	...
