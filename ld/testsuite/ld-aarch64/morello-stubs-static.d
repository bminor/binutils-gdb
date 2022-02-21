#source: morello-stubs.s
#as:
#ld: -static -T morello-stubs.ld
#objdump: -D -j .text_low -j .text_high -j .got -j .plt

.*:     file format .*


Disassembly of section .text_low:

0000000000010000 <foo>:
   10000:	9400000c 	bl	10030 <__baz_a64c64_veneer>
   10004:	c2c1d041 	mov	c1, c2
   10008:	d65f03c0 	ret

000000000001000c <baz2>:
   1000c:	14000009 	b	10030 <__baz_a64c64_veneer>
   10010:	c2c253c0 	ret	c30
   10014:	00000000 	.*
   10018:	1400000a 	b	10040 <__baz_a64c64_veneer\+0x10>
   1001c:	d503201f 	nop

0000000000010020 <__baz_veneer>:
   10020:	90877f90 	adrp	c16, f000000 <baz3>
   10024:	02002610 	add	c16, c16, #0x9
   10028:	c2c21200 	br	c16
   1002c:	00000000 	.*

0000000000010030 <__baz_a64c64_veneer>:
   10030:	c2c273e0 	bx	#4
   10034:	90877f90 	adrp	c16, f000000 <baz3>
   10038:	02002610 	add	c16, c16, #0x9
   1003c:	c2c21200 	br	c16

Disassembly of section .text_high:

000000000f000000 <baz3>:
 f000000:	1400000a 	b	f000028 <___veneer>
 f000004:	d65f03c0 	ret

000000000f000008 <baz>:
 f000008:	14000004 	b	f000018 <__foo_c64a64_veneer>
 f00000c:	c2c253c0 	ret	c30
 f000010:	1400000c 	b	f000040 <___veneer\+0x18>
 f000014:	d503201f 	nop

000000000f000018 <__foo_c64a64_veneer>:
 f000018:	90f88090 	adrp	c16, 100010000 <___veneer\+0xf100ffd8>
 f00001c:	02000210 	add	c16, c16, #0x0
 f000020:	c2c21200 	br	c16
 f000024:	00000000 	.*

000000000f000028 <___veneer>:
 f000028:	90f88090 	adrp	x16, 10000 <foo>
 f00002c:	91000210 	add	x16, x16, #0x0
 f000030:	d61f0200 	br	x16
	...
