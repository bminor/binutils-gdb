#as: -Av9
#objdump: -dr
#name: sparc64 set64

.*: +file format .*sparc.*

Disassembly of section .text:

0+ <foo>:
   0:	05 00 00 00 	sethi  %hi\(0x0\), %g2
			0: R_SPARC_HI22	.text
   4:	84 10 a0 00 	mov  %g2, %g2	! 0 <foo>
			4: R_SPARC_LO10	.text
   8:	07 1d 95 0c 	sethi  %hi\(0x76543000\), %g3
   c:	86 10 e2 10 	or  %g3, 0x210, %g3	! 76543210 <\*ABS\*\+(0x|)0x76543210>
  10:	88 10 20 00 	clr  %g4
  14:	0b 00 00 3f 	sethi  %hi\(0xfc00\), %g5
  18:	8a 11 63 ff 	or  %g5, 0x3ff, %g5	! ffff <\*ABS\*\+(0x|)ffff>
  1c:	03 00 00 00 	sethi  %hi\(0x0\), %g1
			1c: R_SPARC_HH22	.text
  20:	82 10 60 00 	mov  %g1, %g1	! 0 <foo>
			20: R_SPARC_HM10	.text
  24:	05 00 00 00 	sethi  %hi\(0x0\), %g2
			24: R_SPARC_HI22	.text
  28:	84 10 a0 00 	mov  %g2, %g2	! 0 <foo>
			28: R_SPARC_LO10	.text
  2c:	83 28 70 20 	sllx  %g1, 0x20, %g1
  30:	84 10 80 01 	or  %g2, %g1, %g2
  34:	86 10 3f ff 	mov  -1, %g3
  38:	86 10 20 00 	clr  %g3
  3c:	86 10 20 01 	mov  1, %g3
  40:	86 10 2f ff 	mov  0xfff, %g3
  44:	07 00 00 04 	sethi  %hi\(0x1000\), %g3
  48:	86 10 30 00 	mov  -4096, %g3
  4c:	07 3f ff fb 	sethi  %hi\(0xffffec00\), %g3
  50:	86 10 e3 ff 	or  %g3, 0x3ff, %g3	! ffffefff <\*ABS\*\+(0x|)ffffefff>
  54:	87 38 e0 00 	sra  %g3, 0, %g3
  58:	07 00 00 3f 	sethi  %hi\(0xfc00\), %g3
  5c:	86 10 e3 ff 	or  %g3, 0x3ff, %g3	! ffff <\*ABS\*\+(0x|)ffff>
  60:	07 3f ff c0 	sethi  %hi\(0xffff0000\), %g3
  64:	87 38 e0 00 	sra  %g3, 0, %g3
  68:	09 1f ff ff 	sethi  %hi\(0x7ffffc00\), %g4
  6c:	88 11 23 ff 	or  %g4, 0x3ff, %g4	! 7fffffff <\*ABS\*\+(0x|)7fffffff>
  70:	09 20 00 00 	sethi  %hi\(0x80000000\), %g4
  74:	09 20 00 00 	sethi  %hi\(0x80000000\), %g4
  78:	89 39 20 00 	sra  %g4, 0, %g4
  7c:	82 10 3f ff 	mov  -1, %g1
  80:	09 1f ff ff 	sethi  %hi\(0x7ffffc00\), %g4
  84:	88 11 23 ff 	or  %g4, 0x3ff, %g4	! 7fffffff <\*ABS\*\+(0x|)7fffffff>
  88:	83 28 70 20 	sllx  %g1, 0x20, %g1
  8c:	88 11 00 01 	or  %g4, %g1, %g4
  90:	09 3f ff ff 	sethi  %hi\(0xfffffc00\), %g4
  94:	88 11 23 ff 	or  %g4, 0x3ff, %g4	! ffffffff <\*ABS\*\+(0x|)ffffffff>
  98:	88 10 20 01 	mov  1, %g4
  9c:	89 29 30 20 	sllx  %g4, 0x20, %g4
  a0:	03 1f ff ff 	sethi  %hi\(0x7ffffc00\), %g1
  a4:	82 10 63 ff 	or  %g1, 0x3ff, %g1	! 7fffffff <\*ABS\*\+(0x|)7fffffff>
  a8:	0b 3f ff ff 	sethi  %hi\(0xfffffc00\), %g5
  ac:	8a 11 63 ff 	or  %g5, 0x3ff, %g5	! ffffffff <\*ABS\*\+(0x|)ffffffff>
  b0:	83 28 70 20 	sllx  %g1, 0x20, %g1
  b4:	8a 11 40 01 	or  %g5, %g1, %g5
  b8:	0b 20 00 00 	sethi  %hi\(0x80000000\), %g5
  bc:	8b 29 70 20 	sllx  %g5, 0x20, %g5
  c0:	8a 10 3f ff 	mov  -1, %g5
  c4:	8b 29 70 20 	sllx  %g5, 0x20, %g5
  c8:	0b 20 00 00 	sethi  %hi\(0x80000000\), %g5
  cc:	8b 39 60 00 	sra  %g5, 0, %g5
  d0:	03 3f ff c0 	sethi  %hi\(0xffff0000\), %g1
  d4:	0b 3f ff c0 	sethi  %hi\(0xffff0000\), %g5
  d8:	83 28 70 20 	sllx  %g1, 0x20, %g1
  dc:	8a 11 40 01 	or  %g5, %g1, %g5
  e0:	03 3f ff c0 	sethi  %hi\(0xffff0000\), %g1
  e4:	8a 10 20 01 	mov  1, %g5
  e8:	83 28 70 20 	sllx  %g1, 0x20, %g1
  ec:	8a 11 40 01 	or  %g5, %g1, %g5
  f0:	82 10 20 01 	mov  1, %g1
  f4:	0b 3f ff c0 	sethi  %hi\(0xffff0000\), %g5
  f8:	8a 11 60 01 	or  %g5, 1, %g5	! ffff0001 <\*ABS\*\+(0x|)ffff0001>
  fc:	83 28 70 20 	sllx  %g1, 0x20, %g1
 100:	8a 11 40 01 	or  %g5, %g1, %g5
 104:	82 10 20 01 	mov  1, %g1
 108:	0b 3f ff c0 	sethi  %hi\(0xffff0000\), %g5
 10c:	83 28 70 20 	sllx  %g1, 0x20, %g1
 110:	8a 11 40 01 	or  %g5, %g1, %g5
 114:	82 10 20 01 	mov  1, %g1
 118:	8a 10 20 01 	mov  1, %g5
 11c:	83 28 70 20 	sllx  %g1, 0x20, %g1
 120:	8a 11 40 01 	or  %g5, %g1, %g5
