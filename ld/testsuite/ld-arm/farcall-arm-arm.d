.*:     file format .*

Disassembly of section .text:

00001000 <_start-0x8>:
    1000:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1004 <_start-0x4>
    1004:	02001020 	.word	0x02001020

00001008 <_start>:
    1008:	ebfffffc 	bl	1000 <_start-0x8>
Disassembly of section .foo:

02001020 <bar>:
 2001020:	e12fff1e 	bx	lr
