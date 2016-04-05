#as: -mcpu=nps400
#objdump: -dr

.*: +file format .*arc.*

Disassembly of section .text:

[0-9a-f]+ <.*>:
   0:	4821 1485           	movb	r0,r0,r1,0x4,0x5,0x6
   4:	4881 1485           	movb	r0,r0,r12,0x4,0x5,0x6
   8:	4f81 1485           	movb	r15,r15,r12,0x4,0x5,0x6
   c:	4821 9485           	movb.cl	r0,r1,0x4,0x5,0x6
  10:	48c1 9485           	movb.cl	r0,r14,0x4,0x5,0x6
  14:	4d21 9485           	movb.cl	r13,r1,0x4,0x5,0x6
  18:	4808 04d2           	movh	r0,r0,0x4d2
  1c:	4868 ffff           	movh	r3,r3,0xffff
  20:	4818 04d2           	movh.cl	r0,0x4d2
  24:	4878 ffff           	movh.cl	r3,0xffff
