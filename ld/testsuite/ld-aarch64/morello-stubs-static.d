#source: morello-stubs.s
#as:
#ld: -static -T morello-stubs.ld
#objdump: -D -j .text_low -j .text_high -j .got -j .plt

.*:     file format .*


Disassembly of section .text_low:

00000000000000b0 <foo>:
  b0:	9400000c 	bl	e0 <__baz_a64c64_veneer>
  b4:	c2c1d041 	mov	c1, c2
  b8:	d65f03c0 	ret

00000000000000bc <baz2>:
  bc:	14000009 	b	e0 <__baz_a64c64_veneer>
  c0:	c2c253c0 	ret	c30
  c4:	00000000 	.*
  c8:	1400000a 	b	f0 <__baz_a64c64_veneer\+0x10>
  cc:	d503201f 	nop

00000000000000d0 <__baz_veneer>:
  d0:	90878010 	adrp	c16, f000000 <baz3>
  d4:	02002610 	add	c16, c16, #0x9
  d8:	c2c21200 	br	c16
  dc:	00000000 	.*

00000000000000e0 <__baz_a64c64_veneer>:
  e0:	c2c273e0 	bx	#4
  e4:	90878010 	adrp	c16, f000000 <baz3>
  e8:	02002610 	add	c16, c16, #0x9
  ec:	c2c21200 	br	c16

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
 f000018:	90f88010 	adrp	c16, 100000000 <___veneer\+0xf0ffffd8>
 f00001c:	0202c210 	add	c16, c16, #0xb0
 f000020:	c2c21200 	br	c16
 f000024:	00000000 	.*

000000000f000028 <___veneer>:
 f000028:	90f88010 	adrp	x16, 0 <foo-0xb0>
 f00002c:	9102c210 	add	x16, x16, #0xb0
 f000030:	d61f0200 	br	x16
	...
