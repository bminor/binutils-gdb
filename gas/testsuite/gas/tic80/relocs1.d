#objdump: -dr
#name: TIc80 simple relocs, global/local funcs & branches

.*: +file format .*tic80.*

Disassembly of section .text:

00000000 <_sfunc>:
   0:	f0 ff 6c 08 	addu      -16,r1,r1
   4:	0c 00 59 f8 	st        12\(r1\),r31
   8:	00 00 59 10 	st        0\(r1\),r2
   c:	00 90 38 f8 	jsr       0 <_sfunc>\(r0\),r31
  10:	00 00 00 00 
			10: 32	_xfunc
  14:	00 00 51 10 	ld        0\(r1\),r2
  18:	0c 00 51 f8 	ld        12\(r1\),r31
  1c:	1f 80 38 00 	jsr       r31\(r0\),r0
  20:	10 80 6c 08 	addu      16,r1,r1

00000024 <_gfunc>:
  24:	f0 ff 6c 08 	addu      -16,r1,r1
  28:	0c 00 59 f8 	st        12\(r1\),r31
  2c:	00 00 59 10 	st        0\(r1\),r2
  30:	00 90 38 f8 	jsr       0 <_sfunc>\(r0\),r31
  34:	00 00 00 00 
			34: 32	*ABS*
  38:	00 00 51 10 	ld        0\(r1\),r2
  3c:	0c 00 51 f8 	ld        12\(r1\),r31
  40:	1f 80 38 00 	jsr       r31\(r0\),r0
  44:	10 80 6c 08 	addu      16,r1,r1

00000048 <_branches>:
  48:	f0 ff 6c 08 	addu      -16,r1,r1
  4c:	0c 00 59 f8 	st        12\(r1\),r31
  50:	00 00 59 10 	st        0\(r1\),r2
  54:	00 00 51 10 	ld        0\(r1\),r2
  58:	04 00 59 10 	st        4\(r1\),r2
  5c:	00 00 51 10 	ld        0\(r1\),r2
  60:	04 00 51 18 	ld        4\(r1\),r3
  64:	0a 80 ac 10 	addu      10,r2,r2
  68:	03 00 ba 10 	cmp       r3,r2,r2
  6c:	12 80 a5 30 	bbo\.a     b4 <_branches+6c>,r2,ge\.f
  70:	04 00 51 10 	ld        4\(r1\),r2
  74:	05 80 a4 f8 	bbz\.a     88 <_branches+40>,r2,eq\.b
  78:	00 90 38 f8 	jsr       24 <_gfunc>\(r0\),r31
  7c:	24 00 00 00 
			7c: 32	*ABS*
  80:	04 00 51 10 	ld        4\(r1\),r2
  84:	04 80 24 00 	br\.a      94 <_branches+4c>
  88:	00 90 38 f8 	jsr       0 <_sfunc>\(r0\),r31
  8c:	00 00 00 00 
			8c: 32	_xfunc
  90:	04 00 51 10 	ld        4\(r1\),r2
  94:	04 00 51 10 	ld        4\(r1\),r2
  98:	01 80 ac 10 	addu      1,r2,r2
  9c:	04 00 59 10 	st        4\(r1\),r2
  a0:	00 00 51 18 	ld        0\(r1\),r3
  a4:	04 00 51 10 	ld        4\(r1\),r2
  a8:	0a 80 ec 18 	addu      10,r3,r3
  ac:	02 00 fa 10 	cmp       r2,r3,r2
  b0:	f0 ff a5 38 	bbo\.a     70 <_branches+28>,r2,lt\.f
  b4:	0c 00 51 f8 	ld        12\(r1\),r31
  b8:	1f 80 38 00 	jsr       r31\(r0\),r0
  bc:	10 80 6c 08 	addu      16,r1,r1
