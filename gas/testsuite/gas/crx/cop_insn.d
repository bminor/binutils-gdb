#as:
#objdump: -dr
#name: cop_insn

.*: +file format .*

Disassembly of section .text:

00000000 <mtcr>:
   0:	1f 30 1e 30 	mtcr	\$0xf, r1, c14

00000004 <mfcr>:
   4:	13 30 72 31 	mfcr	\$0x3, c7, r2

00000008 <mtcsr>:
   8:	12 30 51 32 	mtcsr	\$0x2, r5, cs1

0000000c <mfcsr>:
   c:	11 30 ce 33 	mfcsr	\$0x1, cs12, r14

00000010 <ldcr>:
  10:	11 30 38 34 	ldcr	\$0x1, r3, c8

00000014 <stcr>:
  14:	12 30 4b 35 	stcr	\$0x2, r4, c11

00000018 <ldcsr>:
  18:	14 30 6c 36 	ldcsr	\$0x4, r6, cs12

0000001c <stcsr>:
  1c:	17 30 dd 37 	stcsr	\$0x7, r13, cs13

00000020 <loadmcr>:
  20:	13 31 01 30 	loadmcr	\$0x3, r1, {c2,c3,c5}
  24:	2c 00 

00000026 <stormcr>:
  26:	1f 31 1e 30 	stormcr	\$0xf, r14, {c4,c7,c9,c10}
  2a:	90 06 

0000002c <loadmcsr>:
  2c:	1c 31 28 30 	loadmcsr	\$0xc, r8, {cs7,cs8,cs9,cs10,cs11}
  30:	80 0f 

00000032 <stormcsr>:
  32:	19 31 39 30 	stormcsr	\$0x9, r9, {cs4,cs7,cs10}
  36:	90 04 

00000038 <bcop>:
  38:	13 30 48 77 	bcop	\$0x7, \$0x3, \*\+0x90
  3c:	1c 31 fa 76 	bcop	\$0x6, \$0xc, \*\-0xbcdfe
  40:	01 19 

00000042 <cpdop>:
  42:	13 30 45 b2 	cpdop	\$0x3, \$0x2, r4, r5
  46:	17 31 12 ba 	cpdop	\$0x7, \$0xa, r1, r2, \$0x1234
  4a:	34 12 

0000004c <mtpr>:
  4c:	09 30 10 00 	mtpr	r0, hi

00000050 <mfpr>:
  50:	0a 30 05 11 	mfpr	lo, r5
  54:	0a 30 0a 90 	mfpr	uhi, r10

00000058 <cinv>:
  58:	10 30 0f 00 	cinv	\[b,d,i,u\]
