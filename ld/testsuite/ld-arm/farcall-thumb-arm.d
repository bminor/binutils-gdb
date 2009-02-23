.*:     file format .*

Disassembly of section .text:

00001000 <__bar_from_thumb>:
    1000:	4778      	bx	pc
    1002:	46c0      	nop			\(mov r8, r8\)
    1004:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1008 <__bar_from_thumb\+0x8>
    1008:	02001014 	.word	0x02001014
    100c:	00000000 	.word	0x00000000

00001010 <_start>:
    1010:	f7ff fff6 	bl	1000 <__bar_from_thumb>
Disassembly of section .foo:

02001014 <bar>:
 2001014:	e12fff1e 	bx	lr
