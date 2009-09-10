#as: -EL -mARC700
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <.text>:
   0:	00 21 80 00             	add        r0,r1,r2
   4:	00 23 1a 37             	add        gp,fp,sp
   8:	00 26 dd 37             	add        ilink1,ilink2,blink
   c:	40 21 00 00             	add        r0,r1,0
  10:	00 26 80 70 00 00 00 00 	add        r0,0,r2
  18:	00 21 be 00             	add        0,r1,r2
  1c:	00 21 80 0f ff ff ff ff 	add        r0,r1,-1
  24:	00 26 80 70 ff ff ff ff 	add        r0,-1,r2
  2c:	00 21 80 0f 00 00 ff 00 	add        r0,r1,255
  34:	00 26 80 70 00 00 ff 00 	add        r0,255,r2
  3c:	00 21 80 0f ff ff 00 ff 	add        r0,r1,-256
  44:	00 26 80 70 ff ff 00 ff 	add        r0,-256,r2
  4c:	00 21 80 0f 00 00 00 01 	add        r0,r1,0x100
  54:	00 26 80 70 ff ff ff fe 	add        r0,0xfffffeff,r2
  5c:	00 21 80 0f 00 00 00 00 	add        r0,r1,0
			60: R_ARC_32_ME	foo
  64:	00 21 80 00             	add        r0,r1,r2
  68:	00 24 43 01             	add        r3,r4,r5
  6c:	00 27 06 02             	add        r6,r7,r8
  70:	00 22 c9 12             	add        r9,r10,r11
  74:	00 25 8c 13             	add        r12,r13,r14
  78:	00 20 4f 24             	add        r15,r16,r17
  7c:	00 23 12 25             	add        r18,r19,r20
  80:	00 26 d5 25             	add        r21,r22,r23
  84:	00 21 98 36             	add        r24,r25,gp
  88:	00 24 5b 37             	add        fp,sp,ilink1
  8c:	00 27 9e 3f 00 00 00 00 	add        ilink2,blink,0
			90: R_ARC_32_ME	r32
  94:	00 21 80 80             	add.f      r0,r1,r2
  98:	40 21 40 80             	add.f      r0,r1,1
  9c:	00 26 80 f0 00 00 01 00 	add.f      r0,1,r2
  a4:	00 21 be 80             	add.f      0,r1,r2
  a8:	00 21 80 8f 00 00 00 02 	add.f      r0,r1,0x200
  b0:	00 26 80 f0 00 00 00 02 	add.f      r0,0x200,r2
  b8:	00 21 80 80             	add.f      r0,r1,r2
  bc:	40 21 00 80             	add.f      r0,r1,0
  c0:	00 26 80 f0 00 00 00 00 	add.f      r0,0,r2
  c8:	00 21 be 80             	add.f      0,r1,r2
  cc:	00 21 80 8f 00 00 00 02 	add.f      r0,r1,0x200
  d4:	00 26 80 f0 00 00 00 02 	add.f      r0,0x200,r2
