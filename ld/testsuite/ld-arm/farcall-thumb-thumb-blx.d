.*:     file format .*

Disassembly of section .text:

00001000 <_start-0x8>:
    1000:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1004 <_start-0x4>
    1004:	02001015 	.word	0x02001015

00001008 <_start>:
    1008:	f7ff effa 	blx	1000 <_start-0x8>
Disassembly of section .foo:

02001014 <bar>:
 2001014:	4770      	bx	lr
