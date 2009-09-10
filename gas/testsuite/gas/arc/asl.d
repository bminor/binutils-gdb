#as: -EL -mARC600
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <.text>:
   0:	2f 20 40 00             	asl        r0,r1
   4:	2f 23 00 37             	asl        fp,sp
   8:	6f 20 00 00             	asl        r0,0
   c:	2f 21 80 0f ff ff ff ff 	asl        r1,-1
  14:	2f 26 80 70             	asl        0,r2
  18:	2f 24 80 0f 00 00 ff 00 	asl        r4,255
  20:	2f 26 80 0f ff ff 00 ff 	asl        r6,-256
  28:	2f 20 80 1f 00 00 00 01 	asl        r8,0x100
  30:	2f 21 80 1f ff ff ff fe 	asl        r9,0xfffffeff
  38:	2f 23 80 1f 42 42 42 42 	asl        r11,0x42424242
  40:	2f 20 80 0f 00 00 00 00 	asl        r0,0
			44: R_ARC_32_ME	foo
  48:	2f 20 40 00             	asl        r0,r1
  4c:	2f 23 00 01             	asl        r3,r4
  50:	2f 26 c0 01             	asl        r6,r7
  54:	2f 21 80 12             	asl        r9,r10
  58:	2f 24 40 13             	asl        r12,r13
  5c:	2f 27 00 14             	asl        r15,r16
  60:	2f 22 c0 24             	asl        r18,r19
  64:	2f 25 80 25             	asl        r21,r22
  68:	2f 20 40 36             	asl        r24,r25
  6c:	2f 23 00 37             	asl        fp,sp
  70:	2f 26 c0 37             	asl        ilink2,blink
  74:	2f 26 80 7f 00 00 00 00 	asl        0,0
			78: R_ARC_32_ME	r34
  7c:	2f 26 80 7f 00 00 00 00 	asl        0,0
			80: R_ARC_32_ME	r37
  84:	2f 26 80 7f 00 00 00 00 	asl        0,0
			88: R_ARC_32_ME	r40
  8c:	2f 26 80 7f 00 00 00 00 	asl        0,0
			90: R_ARC_32_ME	r43
  94:	2f 26 80 7f 00 00 00 00 	asl        0,0
			98: R_ARC_32_ME	r46
  9c:	2f 26 80 7f 00 00 00 00 	asl        0,0
			a0: R_ARC_32_ME	r49
  a4:	2f 26 80 7f 00 00 00 00 	asl        0,0
			a8: R_ARC_32_ME	r50
  ac:	2f 26 80 7f 00 00 00 00 	asl        0,0
			b0: R_ARC_32_ME	r55
  b4:	2f 26 80 7f 00 00 00 00 	asl        0,0
			b8: R_ARC_32_ME	r58
  bc:	2f 20 40 80             	asl.f      r0,r1
  c0:	6f 22 40 80             	asl.f      r2,1
  c4:	2f 26 00 f1             	asl.f      0,r4
  c8:	2f 25 80 8f 00 00 00 02 	asl.f      r5,0x200
