.*:     file format .*

Disassembly of section .text:

00001000 <_start>:
    1000:	000000eb 	bl	1008 <__bar_veneer>
    1004:	00000000 	andeq	r0, r0, r0

00001008 <__bar_veneer>:
    1008:	04f01fe5 	ldr	pc, \[pc, #-4\]	; 100c <__bar_veneer\+0x4>
    100c:	02001020 	.word	0x02001020
Disassembly of section .foo:

02001020 <bar>:
 2001020:	1eff2fe1 	bx	lr
