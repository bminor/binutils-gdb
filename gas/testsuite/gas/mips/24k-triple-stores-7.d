#objdump: -dr
#as: -mfix-24k -32
#name: 24K: Triple Store (Extended Range Check)

.*: +file format .*mips.*

Disassembly of section .text:
0+ <.*>:
   0:	a1130004 	sb	s3,4\(t0\)
   4:	00000000 	nop
   8:	ad130008 	sw	s3,8\(t0\)
   c:	a113000f 	sb	s3,15\(t0\)
  10:	0000000d 	break
  14:	a1130003 	sb	s3,3\(t0\)
  18:	ad130008 	sw	s3,8\(t0\)
  1c:	00000000 	nop
  20:	a113000f 	sb	s3,15\(t0\)
  24:	0000000d 	break
  28:	ad13001c 	sw	s3,28\(t0\)
  2c:	ad130008 	sw	s3,8\(t0\)
  30:	a113001f 	sb	s3,31\(t0\)
  34:	0000000d 	break
  38:	a1130005 	sb	s3,5\(t0\)
  3c:	ad130009 	sw	s3,9\(t0\)
  40:	a1130010 	sb	s3,16\(t0\)
  44:	0000000d 	break
  48:	a1130004 	sb	s3,4\(t0\)
  4c:	ad130009 	sw	s3,9\(t0\)
  50:	00000000 	nop
  54:	a1130010 	sb	s3,16\(t0\)
  58:	0000000d 	break
  5c:	a1130006 	sb	s3,6\(t0\)
  60:	a5130008 	sh	s3,8\(t0\)
  64:	a113000f 	sb	s3,15\(t0\)
  68:	0000000d 	break
  6c:	a1130005 	sb	s3,5\(t0\)
  70:	a5130008 	sh	s3,8\(t0\)
  74:	00000000 	nop
  78:	a113000f 	sb	s3,15\(t0\)
  7c:	0000000d 	break
  80:	a513001e 	sh	s3,30\(t0\)
  84:	a5130008 	sh	s3,8\(t0\)
  88:	a113001f 	sb	s3,31\(t0\)
  8c:	0000000d 	break
  90:	a1130007 	sb	s3,7\(t0\)
  94:	a5130009 	sh	s3,9\(t0\)
  98:	a1130010 	sb	s3,16\(t0\)
  9c:	0000000d 	break
  a0:	a1130006 	sb	s3,6\(t0\)
  a4:	a5130009 	sh	s3,9\(t0\)
  a8:	00000000 	nop
  ac:	a1130010 	sb	s3,16\(t0\)
  b0:	0000000d 	break
  b4:	a1130007 	sb	s3,7\(t0\)
  b8:	f5000008 	sdc1	\$f0,8\(t0\)
  bc:	a113000f 	sb	s3,15\(t0\)
  c0:	0000000d 	break
  c4:	a1130007 	sb	s3,7\(t0\)
  c8:	f5000008 	sdc1	\$f0,8\(t0\)
  cc:	00000000 	nop
  d0:	a1130010 	sb	s3,16\(t0\)
  d4:	0000000d 	break
  d8:	a1130010 	sb	s3,16\(t0\)
  dc:	f5000008 	sdc1	\$f0,8\(t0\)
  e0:	a1130017 	sb	s3,23\(t0\)
  e4:	0000000d 	break
  e8:	a1130010 	sb	s3,16\(t0\)
  ec:	f5000008 	sdc1	\$f0,8\(t0\)
  f0:	00000000 	nop
  f4:	a1130018 	sb	s3,24\(t0\)
  f8:	0000000d 	break
  fc:	a1130008 	sb	s3,8\(t0\)
 100:	f5000009 	sdc1	\$f0,9\(t0\)
 104:	a1130010 	sb	s3,16\(t0\)
 108:	0000000d 	break
 10c:	a113fffd 	sb	s3,-3\(t0\)
 110:	f500fffe 	sdc1	\$f0,-2\(t0\)
 114:	00000000 	nop
 118:	a1130006 	sb	s3,6\(t0\)
 11c:	0000000d 	break
	...
