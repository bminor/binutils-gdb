#objdump: -dr

.*: +file format .*arc.*


Disassembly of section .text:

[0-9a-f]+ <.text>:
   0:	392d 0080           	myinsn	r0,r1,r2
   4:	3b2d 371a           	myinsn	gp,fp,sp
   8:	3e2d 37dd           	myinsn	ilink,r30,blink
   c:	396d 0000           	myinsn	r0,r1,0
  10:	3e2d 7080 0000 0000 	myinsn	r0,0,r2
  18:	392d 00be           	myinsn	0,r1,r2
  1c:	392d 0f80 ffff ffff 	myinsn	r0,r1,0xffffffff
  24:	3e2d 7080 ffff ffff 	myinsn	r0,0xffffffff,r2
  2c:	392d 0f80 0000 00ff 	myinsn	r0,r1,0xff
  34:	3e2d 7080 0000 00ff 	myinsn	r0,0xff,r2
  3c:	392d 0f80 ffff ff00 	myinsn	r0,r1,0xffffff00
  44:	3e2d 7080 ffff ff00 	myinsn	r0,0xffffff00,r2
  4c:	392d 0f80 0000 0100 	myinsn	r0,r1,0x100
  54:	3e2d 7080 ffff feff 	myinsn	r0,0xfffffeff,r2
  5c:	3e2d 7f80 0000 0100 	myinsn	r0,0x100,0x100
  64:	392d 0f80 0000 0000 	myinsn	r0,r1,0
			68: R_ARC_32_ME	foo
  6c:	38ed 0080           	myinsn	r0,r0,r2
  70:	3bed 0140           	myinsn	r3,r3,r5
  74:	3eed 0201           	myinsn.eq	r6,r6,r8
  78:	39ed 12c1           	myinsn.eq	r9,r9,r11
  7c:	3ced 1382           	myinsn.ne	r12,r12,r14
  80:	3fed 1442           	myinsn.ne	r15,r15,r17
  84:	3aed 2503           	myinsn.p	r18,r18,r20
  88:	3ded 25c3           	myinsn.p	r21,r21,r23
  8c:	38ed 3684           	myinsn.n	r24,r24,gp
  90:	3bed 3744           	myinsn.n	fp,fp,ilink
  94:	3eed 37c5           	myinsn.c	r30,r30,blink
  98:	3bed 00c5           	myinsn.c	r3,r3,r3
  9c:	3bed 0205           	myinsn.c	r3,r3,r8
  a0:	3bed 0106           	myinsn.nc	r3,r3,r4
  a4:	3ced 0106           	myinsn.nc	r4,r4,r4
  a8:	3ced 01c6           	myinsn.nc	r4,r4,r7
  ac:	3ced 0147           	myinsn.v	r4,r4,r5
  b0:	3ded 0147           	myinsn.v	r5,r5,r5
  b4:	3ded 0148           	myinsn.nv	r5,r5,r5
  b8:	3ded 0148           	myinsn.nv	r5,r5,r5
  bc:	3eed 0009           	myinsn.gt	r6,r6,r0
  c0:	38ed 002a           	myinsn.ge	r0,r0,0
  c4:	39ed 006b           	myinsn.lt	r1,r1,0x1
  c8:	3bed 00ed           	myinsn.hi	r3,r3,0x3
  cc:	3ced 012e           	myinsn.ls	r4,r4,0x4
  d0:	3ded 016f           	myinsn.pnz	r5,r5,0x5
  d4:	392d 8080           	myinsn.f	r0,r1,r2
  d8:	396d 8040           	myinsn.f	r0,r1,0x1
  dc:	3e2d f080 0000 0001 	myinsn.f	r0,0x1,r2
  e4:	392d 80be           	myinsn.f	0,r1,r2
  e8:	392d 8f80 0000 0200 	myinsn.f	r0,r1,0x200
  f0:	3e2d f080 0000 0200 	myinsn.f	r0,0x200,r2
  f8:	39ed 8081           	myinsn.eq.f	r1,r1,r2
  fc:	38ed 8022           	myinsn.ne.f	r0,r0,0
 100:	3aed 808b           	myinsn.lt.f	r2,r2,r2
 104:	3eed f0a9 0000 0001 	myinsn.gt.f	0,0x1,0x2
 10c:	3eed ff8c 0000 0200 	myinsn.le.f	0,0x200,0x200
 114:	3eed f0aa 0000 0200 	myinsn.ge.f	0,0x200,0x2
