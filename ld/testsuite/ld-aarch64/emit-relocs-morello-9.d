#source: emit-relocs-morello-9.s
#as: -march=morello+c64
#ld: -static -Ttext-segment 0x0
#objdump: -d -j .data -j .text

.*:     file format .*


Disassembly of section \.text:

0000000000000000 <_start>:
   0:	f2800020 	movk	x0, #0x1
   4:	f2800020 	movk	x0, #0x1
   8:	30ffffc0 	adr	c0, 1 <_start\+0x1>
   c:	30ffffa0 	adr	c0, 1 <_start\+0x1>
  10:	02000400 	add	c0, c0, #0x1
  14:	02000400 	add	c0, c0, #0x1
  18:	d2800020 	mov	x0, #0x1                   	// #1
  1c:	d2800020 	mov	x0, #0x1                   	// #1
  20:	f2800020 	movk	x0, #0x1
  24:	f2800020 	movk	x0, #0x1

Disassembly of section \.data:

.* <val>:
   .*:	00000001 	.word	0x00000001
   .*:	00000001 	.word	0x00000001
   .*:	00000001 	.word	0x00000001
   .*:	00000000 	.word	0x00000000
   .*:	00000001 	.word	0x00000001
   .*:	00000001 	.word	0x00000001
   .*:	00000001 	.word	0x00000001
   .*:	00000000 	.word	0x00000000
