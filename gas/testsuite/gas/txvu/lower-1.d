#as:
#objdump: -dr
#name: lower-1

.*: +file format .*

Disassembly of section .text:

0* <foo>:
   0:	ff 03 00 40[ 	]*nop[ 	]*b 1ff8 <foo\+0x1ff8>
   4:	ff 02 00 00 
   8:	ff 03 03 42[ 	]*nop[ 	]*bal vi03,2000 <foo\+0x2000>
   c:	ff 02 00 00 
  10:	bc 03 21 81[ 	]*nop[ 	]*div q,vf00y,vf00x
  14:	ff 02 00 00 
  18:	fd 07 20 80[ 	]*nop[ 	]*eatan p,vf00y
  1c:	ff 02 00 00 
  20:	7c 07 e0 81[ 	]*nop[ 	]*eatanxy p,vf00
  24:	ff 02 00 00 
  28:	7d 07 e0 81[ 	]*nop[ 	]*eatanxz p,vf00
  2c:	ff 02 00 00 
  30:	fe 07 20 80[ 	]*nop[ 	]*eexp p,vf00y
  34:	ff 02 00 00 
  38:	4e 07 e0 81[ 	]*nop[ 	]*eleng p,vf00
  3c:	ff 02 00 00 
  40:	be 07 20 80[ 	]*nop[ 	]*ercpr p,vf00y
  44:	ff 02 00 00 
  48:	3f 07 e0 81[ 	]*nop[ 	]*erleng p,vf00
  4c:	ff 02 00 00 
  50:	3d 07 e0 81[ 	]*nop[ 	]*ersadd p,vf00
  54:	ff 02 00 00 
  58:	bd 07 20 80[ 	]*nop[ 	]*ersqrt p,vf00y
  5c:	ff 02 00 00 
  60:	3c 07 e0 81[ 	]*nop[ 	]*esadd p,vf00
  64:	ff 02 00 00 
  68:	fc 07 20 80[ 	]*nop[ 	]*esin p,vf00y
  6c:	ff 02 00 00 
  70:	bc 07 20 80[ 	]*nop[ 	]*esqrt p,vf00y
  74:	ff 02 00 00 
  78:	7e 07 e0 81[ 	]*nop[ 	]*esum p,vf00
  7c:	ff 02 00 00 
  80:	ff ff ff 24[ 	]*nop[ 	]*fcand vi01,16777215
  84:	ff 02 00 00 
  88:	ff ff ff 20[ 	]*nop[ 	]*fceq vi01,16777215
  8c:	ff 02 00 00 
  90:	00 00 03 38[ 	]*nop[ 	]*fcget vi03
  94:	ff 02 00 00 
  98:	ff ff ff 26[ 	]*nop[ 	]*fcor vi01,16777215
  9c:	ff 02 00 00 
  a0:	ff ff ff 22[ 	]*nop[ 	]*fcset 16777215
  a4:	ff 02 00 00 
  a8:	00 20 03 34[ 	]*nop[ 	]*fmand vi03,vi04
  ac:	ff 02 00 00 
  b0:	00 20 03 30[ 	]*nop[ 	]*fmeq vi03,vi04
  b4:	ff 02 00 00 
  b8:	00 20 03 36[ 	]*nop[ 	]*fmor vi03,vi04
  bc:	ff 02 00 00 
  c0:	ff 07 23 2c[ 	]*nop[ 	]*fsand vi03,4095
  c4:	ff 02 00 00 
  c8:	ff 07 23 28[ 	]*nop[ 	]*fseq vi03,4095
  cc:	ff 02 00 00 
  d0:	ff 07 23 2e[ 	]*nop[ 	]*fsor vi03,4095
  d4:	ff 02 00 00 
  d8:	c0 07 20 2a[ 	]*nop[ 	]*fsset 4032
  dc:	ff 02 00 00 
  e0:	70 21 03 80[ 	]*nop[ 	]*iadd vi05,vi04,vi03
  e4:	ff 02 00 00 
  e8:	f2 23 03 80[ 	]*nop[ 	]*iaddi vi03,vi04,15
  ec:	ff 02 00 00 
  f0:	32 24 03 80[ 	]*nop[ 	]*iaddi vi03,vi04,-16
  f4:	ff 02 00 00 
  f8:	ff 27 e3 11[ 	]*nop[ 	]*iaddiu vi03,vi04,32767
  fc:	ff 02 00 00 
 100:	74 21 03 80[ 	]*nop[ 	]*iand vi05,vi04,vi03
 104:	ff 02 00 00 
 108:	ff 23 03 50[ 	]*nop[ 	]*ibeq vi03,vi04,2100 <foo\+0x2100>
 10c:	ff 02 00 00 
 110:	ff 23 00 5e[ 	]*nop[ 	]*ibgez vi04,2108 <foo\+0x2108>
 114:	ff 02 00 00 
 118:	ff 23 00 5a[ 	]*nop[ 	]*ibgtz vi04,2110 <foo\+0x2110>
 11c:	ff 02 00 00 
 120:	ff 23 00 5c[ 	]*nop[ 	]*iblez vi04,2118 <foo\+0x2118>
 124:	ff 02 00 00 
 128:	ff 23 00 5c[ 	]*nop[ 	]*iblez vi04,2120 <foo\+0x2120>
 12c:	ff 02 00 00 
 130:	ff 23 03 52[ 	]*nop[ 	]*ibne vi03,vi04,2128 <foo\+0x2128>
 134:	ff 02 00 00 
 138:	ff 23 23 08[ 	]*nop[ 	]*ilw.w vi03,1023\(vi04\)w
 13c:	ff 02 00 00 
 140:	fe 23 23 80[ 	]*nop[ 	]*ilwr.w vi03,\(vi04\)w
 144:	ff 02 00 00 
 148:	74 21 03 80[ 	]*nop[ 	]*ior vi05,vi04,vi03
 14c:	ff 02 00 00 
 150:	71 21 03 80[ 	]*nop[ 	]*isub vi05,vi04,vi03
 154:	ff 02 00 00 
 158:	ff 27 e3 13[ 	]*nop[ 	]*isubiu vi03,vi04,32767
 15c:	ff 02 00 00 
 160:	ff 23 23 0a[ 	]*nop[ 	]*isw.w vi03,1023\(vi04\)w
 164:	ff 02 00 00 
 168:	ff 23 23 80[ 	]*nop[ 	]*iswr.w vi03,\(vi04\)w
 16c:	ff 02 00 00 
 170:	00 20 03 4a[ 	]*nop[ 	]*jalr vi03,vi04
 174:	ff 02 00 00 
 178:	00 20 00 48[ 	]*nop[ 	]*jr vi04
 17c:	ff 02 00 00 
 180:	ff 23 c1 01[ 	]*nop[ 	]*lq.xyz vf01xyz,1023\(vi04\)
 184:	ff 02 00 00 
 188:	7e 23 c1 81[ 	]*nop[ 	]*lqd.xyz vf01xyz,\(--vi04\)
 18c:	ff 02 00 00 
 190:	7c 23 c1 81[ 	]*nop[ 	]*lqi.xyz vf01xyz,\(vi04\+\+\)
 194:	ff 02 00 00 
 198:	fc 23 01 81[ 	]*nop[ 	]*mfir.x vf01x,vi04
 19c:	ff 02 00 00 
 1a0:	7c 06 21 80[ 	]*nop[ 	]*mfp.w vf01w,p
 1a4:	ff 02 00 00 
 1a8:	3c 03 c1 81[ 	]*nop[ 	]*move.xyz vf01xyz,vf00xyz
 1ac:	ff 02 00 00 
 1b0:	3d 03 c1 81[ 	]*nop[ 	]*mr32.xyz vf01xyz,vf00xyz
 1b4:	ff 02 00 00 
 1b8:	fd 03 03 81[ 	]*nop[ 	]*mtir.x vi03,vf00x
 1bc:	ff 02 00 00 
 1c0:	3d 04 c1 81[ 	]*nop[ 	]*rget.xyz vf01xyz,r
 1c4:	ff 02 00 00 
 1c8:	3e 04 20 80[ 	]*nop[ 	]*rinit r,vf00y
 1cc:	ff 02 00 00 
 1d0:	3c 04 c1 81[ 	]*nop[ 	]*rnext.xyz vf01xyz,r
 1d4:	ff 02 00 00 
 1d8:	be 03 21 81[ 	]*nop[ 	]*rsqrt q,vf00y,vf00x
 1dc:	ff 02 00 00 
 1e0:	3f 04 20 80[ 	]*nop[ 	]*rxor r,vf00y
 1e4:	ff 02 00 00 
 1e8:	ff 1b c0 03[ 	]*nop[ 	]*sq.xyz vf00xyz,1023\(vi03\)
 1ec:	ff 02 00 00 
 1f0:	7f 1b c0 81[ 	]*nop[ 	]*sqd.xyz vf00xyz,\(--vi03\)
 1f4:	ff 02 00 00 
 1f8:	7d 1b c0 81[ 	]*nop[ 	]*sqi.xyz vf00xyz,\(vi03\+\+\)
 1fc:	ff 02 00 00 
 200:	bd 03 01 81[ 	]*nop[ 	]*sqrt q,vf00x
 204:	ff 02 00 00 
 208:	bf 07 00 80[ 	]*nop[ 	]*waitp
 20c:	ff 02 00 00 
 210:	bf 03 00 80[ 	]*nop[ 	]*waitq
 214:	ff 02 00 00 
 218:	fc 26 00 80[ 	]*nop[ 	]*xgkick vi04
 21c:	ff 02 00 00 
 220:	bd 06 03 80[ 	]*nop[ 	]*xitop vi03
 224:	ff 02 00 00 
 228:	bc 06 03 80[ 	]*nop[ 	]*xtop vi03
 22c:	ff 02 00 00 
