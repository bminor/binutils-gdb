#objdump: -dr
#name: TIc80 signed and unsigned add instructions

.*: +file format .*tic80.*

Disassembly of section .text:

00000000 <.text>:
   0:	0a 00 fb 62 	add       r10,r11,r12
   4:	ff 3f ac 20 	add       0x3fff,r2,r4
   8:	00 40 2c 21 	add       0xffffc000,r4,r4
   c:	00 10 7b 31 	add       0x4000,r5,r6
  10:	00 40 00 00 
  14:	00 10 fb 41 	add       0xffffbfff,r7,r8
  18:	ff bf ff ff 
  1c:	00 10 bb 5a 	add       0x7fffffff,r10,r11
  20:	ff ff ff 7f 
  24:	00 10 3b 6b 	add       0x80000000,r12,r13
  28:	00 00 00 80 
  2c:	0a 20 fb 62 	addu      r10,r11,r12
  30:	ff bf ac 20 	addu      0x3fff,r2,r4
  34:	00 c0 2c 21 	addu      0xffffc000,r4,r4
  38:	00 30 7b 31 	addu      0x4000,r5,r6
  3c:	00 40 00 00 
  40:	00 30 fb 41 	addu      0xffffbfff,r7,r8
  44:	ff bf ff ff 
  48:	00 30 bb 5a 	addu      0x7fffffff,r10,r11
  4c:	ff ff ff 7f 
  50:	00 30 3b 6b 	addu      0x80000000,r12,r13
  54:	00 00 00 80 
