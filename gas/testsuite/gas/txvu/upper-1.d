#as:
#objdump: -dr
#name: upper-1

.*: +file format .*

Disassembly of section .text:

0* <foo>:
   0:	3c 03 00 80[ 	]*abs.xyz vf01xyz,vf00xyz[ 	]*nop
   4:	fd 01 c1 01 
   8:	3c 03 00 80[ 	]*add.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
   c:	a8 00 c1 01 
  10:	3c 03 00 80[ 	]*addi.xyz vf02xyz,vf00xyz,i[ 	]*nop
  14:	a2 00 c0 01 
  18:	3c 03 00 80[ 	]*addq.xyz vf02xyz,vf00xyz,q[ 	]*nop
  1c:	a0 00 c0 01 
  20:	3c 03 00 80[ 	]*addx.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
  24:	80 00 c1 01 
  28:	3c 03 00 80[ 	]*adda.xyz accxyz,vf00xyz,vf01xyz[ 	]*nop
  2c:	bc 02 c1 01 
  30:	3c 03 00 80[ 	]*addai.xyz accxyz,vf00xyz,i[ 	]*nop
  34:	3e 02 c0 01 
  38:	3c 03 00 80[ 	]*addaq.xyz accxyz,vf00xyz,q[ 	]*nop
  3c:	3c 02 c0 01 
  40:	3c 03 00 80[ 	]*addax.xyz accxyz,vf00xyz,vf01x[ 	]*nop
  44:	3c 00 c1 01 
  48:	3c 03 00 80[ 	]*clip.xyzw vf00xyzw[ 	]*nop
  4c:	ff 01 e0 01 
  50:	3c 03 00 80[ 	]*ftoi0.xyz vf01xyz,vf00xyz[ 	]*nop
  54:	7c 01 c1 01 
  58:	3c 03 00 80[ 	]*ftoi4.xyz vf01xyz,vf00xyz[ 	]*nop
  5c:	7d 01 c1 01 
  60:	3c 03 00 80[ 	]*ftoi12.xyz vf01xyz,vf00xyz[ 	]*nop
  64:	7e 01 c1 01 
  68:	3c 03 00 80[ 	]*ftoi15.xyz vf01xyz,vf00xyz[ 	]*nop
  6c:	7f 01 c1 01 
  70:	3c 03 00 80[ 	]*itof0.xyz vf01xyz,vf00xyz[ 	]*nop
  74:	3c 01 c1 01 
  78:	3c 03 00 80[ 	]*itof4.xyz vf01xyz,vf00xyz[ 	]*nop
  7c:	3d 01 c1 01 
  80:	3c 03 00 80[ 	]*itof12.xyz vf01xyz,vf00xyz[ 	]*nop
  84:	3e 01 c1 01 
  88:	3c 03 00 80[ 	]*itof15.xyz vf01xyz,vf00xyz[ 	]*nop
  8c:	3f 01 c1 01 
  90:	3c 03 00 80[ 	]*madd.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
  94:	a9 00 c1 01 
  98:	3c 03 00 80[ 	]*maddi.xyz vf02xyz,vf00xyz,i[ 	]*nop
  9c:	a3 00 c0 01 
  a0:	3c 03 00 80[ 	]*maddq.xyz vf02xyz,vf00xyz,q[ 	]*nop
  a4:	a1 00 c0 01 
  a8:	3c 03 00 80[ 	]*maddx.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
  ac:	88 00 c1 01 
  b0:	3c 03 00 80[ 	]*madda.xyz accxyz,vf00xyz,vf01xyz[ 	]*nop
  b4:	bd 02 c1 01 
  b8:	3c 03 00 80[ 	]*maddai.xyz accxyz,vf00xyz,i[ 	]*nop
  bc:	3f 02 c0 01 
  c0:	3c 03 00 80[ 	]*maddaq.xyz accxyz,vf00xyz,q[ 	]*nop
  c4:	3d 02 c0 01 
  c8:	3c 03 00 80[ 	]*maddax.xyz accxyz,vf00xyz,vf01x[ 	]*nop
  cc:	bc 00 c1 01 
  d0:	3c 03 00 80[ 	]*max.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
  d4:	ab 00 c1 01 
  d8:	3c 03 00 80[ 	]*maxi.xyz vf02xyz,vf00xyz,i[ 	]*nop
  dc:	ad 00 c0 01 
  e0:	3c 03 00 80[ 	]*maxx.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
  e4:	90 00 c1 01 
  e8:	3c 03 00 80[ 	]*mini.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
  ec:	af 00 c1 01 
  f0:	3c 03 00 80[ 	]*minii.xyz vf02xyz,vf00xyz,i[ 	]*nop
  f4:	9f 00 c0 01 
  f8:	3c 03 00 80[ 	]*minix.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
  fc:	94 00 c1 01 
 100:	3c 03 00 80[ 	]*msub.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
 104:	ad 00 c1 01 
 108:	3c 03 00 80[ 	]*msubi.xyz vf02xyz,vf00xyz,i[ 	]*nop
 10c:	a7 00 c0 01 
 110:	3c 03 00 80[ 	]*msubq.xyz vf02xyz,vf00xyz,q[ 	]*nop
 114:	a5 00 c0 01 
 118:	3c 03 00 80[ 	]*msubx.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
 11c:	8c 00 c1 01 
 120:	3c 03 00 80[ 	]*msuba.xyz accxyz,vf00xyz,vf01xyz[ 	]*nop
 124:	fd 02 c1 01 
 128:	3c 03 00 80[ 	]*msubai.xyz accxyz,vf00xyz,i[ 	]*nop
 12c:	7f 02 c0 01 
 130:	3c 03 00 80[ 	]*msubaq.xyz accxyz,vf00xyz,q[ 	]*nop
 134:	7d 02 c0 01 
 138:	3c 03 00 80[ 	]*msubax.xyz accxyz,vf00xyz,vf01x[ 	]*nop
 13c:	fc 00 c1 01 
 140:	3c 03 00 80[ 	]*mul.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
 144:	aa 00 c1 01 
 148:	3c 03 00 80[ 	]*muli.xyz vf02xyz,vf00xyz,i[ 	]*nop
 14c:	9e 00 c0 01 
 150:	3c 03 00 80[ 	]*mulq.xyz vf02xyz,vf00xyz,q[ 	]*nop
 154:	9c 00 c0 01 
 158:	3c 03 00 80[ 	]*mulx.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
 15c:	98 00 c1 01 
 160:	3c 03 00 80[ 	]*mula.xyz accxyz,vf00xyz,vf01xyz[ 	]*nop
 164:	be 02 c1 01 
 168:	3c 03 00 80[ 	]*mulai.xyz accxyz,vf00xyz,i[ 	]*nop
 16c:	fe 01 c0 01 
 170:	3c 03 00 80[ 	]*mulaq.xyz accxyz,vf00xyz,q[ 	]*nop
 174:	fc 01 c0 01 
 178:	3c 03 00 80[ 	]*mulax.xyz accxyz,vf00xyz,vf01x[ 	]*nop
 17c:	bc 01 c1 01 
 180:	3c 03 00 80[ 	]*nop[ 	]*nop
 184:	ff 02 00 00 
 188:	3c 03 00 80[ 	]*opmula.xyz accxyz,vf00xyz,vf01xyz[ 	]*nop
 18c:	fe 02 c1 01 
 190:	3c 03 00 80[ 	]*opmsub.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
 194:	ae 00 c1 01 
 198:	3c 03 00 80[ 	]*sub.xyz vf02xyz,vf00xyz,vf01xyz[ 	]*nop
 19c:	ac 00 c1 01 
 1a0:	3c 03 00 80[ 	]*subi.xyz vf02xyz,vf00xyz,i[ 	]*nop
 1a4:	a6 00 c0 01 
 1a8:	3c 03 00 80[ 	]*subq.xyz vf02xyz,vf00xyz,q[ 	]*nop
 1ac:	a4 00 c0 01 
 1b0:	3c 03 00 80[ 	]*subx.xyz vf02xyz,vf00xyz,vf01x[ 	]*nop
 1b4:	84 00 c1 01 
 1b8:	3c 03 00 80[ 	]*suba.xyz accxyz,vf00xyz,vf01xyz[ 	]*nop
 1bc:	fc 02 c1 01 
 1c0:	3c 03 00 80[ 	]*subai.xyz accxyz,vf00xyz,i[ 	]*nop
 1c4:	7e 02 c0 01 
 1c8:	3c 03 00 80[ 	]*subaq.xyz accxyz,vf00xyz,q[ 	]*nop
 1cc:	7c 02 c0 01 
 1d0:	3c 03 00 80[ 	]*subax.xyz accxyz,vf00xyz,vf01x[ 	]*nop
 1d4:	7c 00 c1 01 
