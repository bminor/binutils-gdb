#as:
#objdump: -dr
#name: allinsn

.*: +file format .*

Disassembly of section .text:

0+000 <abs>:
   0:	01 e0       	abs	r0

0+002 <addc>:
   2:	06 21       	addc	r1, r2

0+004 <addi>:
   4:	20 03       	addi	r3, 1

0+006 <addu>:
   6:	1c 54       	addu	r4, r5

0+008 <and>:
   8:	16 76       	and	r6, r7

0+00a <andi>:
   a:	2e 28       	andi	r8, 2

0+00c <andn>:
   c:	1f a9       	andn	r9, r10

0+00e <asr>:
   e:	1a cb       	asr	r11, r12

0+010 <asrc>:
  10:	3a 0d       	asrc	r13

0+012 <asri>:
  12:	3b fe       	asri	r14, 31

0+014 <bclri>:
  14:	30 0f       	bclri	r15, 0

0+016 <bf>:
  16:	ef f4       	bf	0x0

0+018 <bgeni>:
  18:	32 70       	bgeni	r0, 7

0+01a <BGENI>:
  1a:	32 80       	bgeni	r0, 8

0+01c <BGENi>:
  1c:	33 f0       	bgeni	r0, 31

0+01e <bgenr>:
  1e:	13 21       	bgenr	r1, r2

0+020 <bkpt>:
	...

0+022 <bmaski>:
  22:	2c 83       	bmaski	r3, 8

0+024 <BMASKI>:
  24:	2d f3       	bmaski	r3, 31

0+026 <br>:
  26:	f7 ff       	br	0x26

0+028 <brev>:
  28:	00 f4       	brev	r4

0+02a <bseti>:
  2a:	35 e5       	bseti	r5, 30

0+02c <bsr>:
  2c:	ff e9       	bsr	0x0

0+02e <bt>:
  2e:	e7 e8       	bt	0x0

0+030 <btsti>:
  30:	37 b6       	btsti	r6, 27

0+032 <clrc>:
  32:	0f 00       	cmpne	r0, r0

0+034 <clrf>:
  34:	01 d7       	clrf	r7

0+036 <clrt>:
  36:	01 c8       	clrt	r8

0+038 <cmphs>:
  38:	0c a9       	cmphs	r9, r10

0+03a <cmplt>:
  3a:	0d cb       	cmplt	r11, r12

0+03c <cmplei>:
  3c:	22 eb       	cmplti	r11, 15

0+03e <cmplti>:
  3e:	23 fd       	cmplti	r13, 32

0+040 <cmpne>:
  40:	0f fe       	cmpne	r14, r15

0+042 <cmpnei>:
  42:	2a 00       	cmpnei	r0, 0

0+044 <decf>:
  44:	00 91       	decf	r1

0+046 <decgt>:
  46:	01 a2       	decgt	r2

0+048 <declt>:
  48:	01 83       	declt	r3

0+04a <decne>:
  4a:	01 b4       	decne	r4

0+04c <dect>:
  4c:	00 85       	dect	r5

0+04e <divs>:
  4e:	32 16       	divs	r6, r1

0+050 <divu>:
  50:	2c 18       	divu	r8, r1

0+052 <doze>:
  52:	00 06       	doze

0+054 <ff1>:
  54:	00 ea       	ff1	r10

0+056 <incf>:
  56:	00 bb       	incf	r11

0+058 <inct>:
  58:	00 ac       	inct	r12

0+05a <ixh>:
  5a:	1d ed       	ixh	r13, r14

0+05c <ixw>:
  5c:	15 0f       	ixw	r15, r0

0+05e <jbf>:
  5e:	ef d0       	bf	0x0

0+060 <jbr>:
  60:	f0 0e       	br	0x7e

0+062 <jbsr>:
  62:	7f 0a       	jsri	0x.*

0+064 <jbt>:
  64:	e0 0c       	bt	0x7e

0+066 <jmp>:
  66:	00 c1       	jmp	r1

0+068 <jmpi>:
  68:	70 09       	jmpi	0x.*

0+06a <jsr>:
  6a:	00 d2       	jsr	r2

0+06c <jsri>:
  6c:	7f 08       	jsri	0x.*

0+06e <ld.b>:
  6e:	a3 04       	ldb	r3, \(r4, 0\)

0+070 <ld.h>:
  70:	c5 16       	ldh	r5, \(r6, 2\)

0+072 <ld.w>:
  72:	87 18       	ld	r7, \(r8, 4\)

0+074 <ldb>:
  74:	a9 fa       	ldb	r9, \(r10, 15\)

0+076 <ldh>:
  76:	cb fc       	ldh	r11, \(r12, 30\)

0+078 <ld>:
  78:	8d 5e       	ld	r13, \(r14, 20\)

0+07a <ldw>:
  7a:	8d fe       	ld	r13, \(r14, 60\)

0+07c <ldm>:
  7c:	00 62       	ldm	r2-r15, \(r0\)

0+07e <fooloop>:
  7e:	00 41       	ldq	r4-r7, \(r1\)

0+080 <loopt>:
  80:	04 8e       	loopt	r8, 0x64

0+082 <LRW>:
  82:	79 03       	lrw	r9, 0x.*

0+084 <lrw>:
  84:	79 04       	lrw	r9, 0x4321

0+086 <foolit>:
  86:	12 34       	mov	r4, r3

0+088 <lsl>:
  88:	1b ba       	lsl	r10, r11

0+08a <lslc>:
  8a:	3c 0c       	lslc	r12

0+08c <.XP0001>:
	...
			8c: ADDR32	.text
			90: ADDR32	.text.*
  94:	00 00       	bkpt
  96:	43 21       	.word 0x4321

0+098 <lsli>:
  98:	3d fd       	lsli	r13, 31

0+09a <lsr>:
  9a:	0b fe       	lsr	r14, r15

0+09c <lsrc>:
  9c:	3e 00       	lsrc	r0

0+09e <lsri>:
  9e:	3e 11       	lsri	r1, 1

0+0a0 <mclri>:
  a0:	30 64       	bclri	r4, 6

0+0a2 <mfcr>:
  a2:	10 02       	mfcr	r2, psr

0+0a4 <mov>:
  a4:	12 43       	mov	r3, r4

0+0a6 <movf>:
  a6:	0a 65       	movf	r5, r6

0+0a8 <movi>:
  a8:	67 f7       	movi	r7, 127

0+0aa <movt>:
  aa:	02 98       	movt	r8, r9

0+0ac <mtcr>:
  ac:	18 0a       	mtcr	r10, psr

0+0ae <mult>:
  ae:	03 cb       	mult	r11, r12

0+0b0 <mvc>:
  b0:	00 2d       	mvc	r13

0+0b2 <mvcv>:
  b2:	00 3e       	mvcv	r14

0+0b4 <neg>:
  b4:	28 02       	rsubi	r2, 0

0+0b6 <not>:
  b6:	01 ff       	not	r15

0+0b8 <or>:
  b8:	1e 10       	or	r0, r1

0+0ba <rfi>:
  ba:	00 03       	rfi

0+0bc <rolc>:
  bc:	06 66       	addc	r6, r6

0+0be <rori>:
  be:	39 a9       	rotli	r9, 26

0+0c0 <rotlc>:
  c0:	06 66       	addc	r6, r6

0+0c2 <rotli>:
  c2:	38 a2       	rotli	r2, 10

0+0c4 <rotri>:
  c4:	39 a9       	rotli	r9, 26

0+0c6 <rsub>:
  c6:	14 43       	rsub	r3, r4

0+0c8 <rsubi>:
  c8:	28 05       	rsubi	r5, 0

0+0ca <rte>:
  ca:	00 02       	rte

0+0cc <rts>:
  cc:	00 cf       	jmp	r15

0+0ce <setc>:
  ce:	0c 00       	cmphs	r0, r0

0+0d0 <sextb>:
  d0:	01 56       	sextb	r6

0+0d2 <sexth>:
  d2:	01 77       	sexth	r7

0+0d4 <st.b>:
  d4:	b8 09       	stb	r8, \(r9, 0\)

0+0d6 <st.h>:
  d6:	da 1b       	sth	r10, \(r11, 2\)

0+0d8 <st.w>:
  d8:	9c 1d       	st	r12, \(r13, 4\)

0+0da <stb>:
  da:	be ff       	stb	r14, \(r15, 15\)

0+0dc <sth>:
  dc:	d0 f1       	sth	r0, \(r1, 30\)

0+0de <stw>:
  de:	92 f3       	st	r2, \(r3, 60\)

0+0e0 <st>:
  e0:	94 05       	st	r4, \(r5, 0\)

0+0e2 <stm>:
  e2:	00 7e       	stm	r14-r15, \(r0\)

0+0e4 <stop>:
  e4:	00 04       	stop

0+0e6 <stq>:
  e6:	00 51       	stq	r4-r7, \(r1\)

0+0e8 <subc>:
  e8:	07 d7       	subc	r7, r13

0+0ea <subi>:
  ea:	25 fe       	subi	r14, 32

0+0ec <subu>:
  ec:	05 39       	subu	r9, r3

0+0ee <sync>:
  ee:	00 01       	sync

0+0f0 <tstlt>:
  f0:	37 f5       	btsti	r5, 31

0+0f2 <tstne>:
  f2:	2a 07       	cmpnei	r7, 0

0+0f4 <trap>:
  f4:	00 0a       	trap	2

0+0f6 <tst>:
  f6:	0e ee       	tst	r14, r14

0+0f8 <tstnbz>:
  f8:	01 92       	tstnbz	r2

0+0fa <wait>:
  fa:	00 05       	wait

0+0fc <xor>:
  fc:	17 0f       	xor	r15, r0

0+0fe <xsr>:
  fe:	38 0b       	xsr	r11

0+0100 <xtrb0>:
 100:	01 31       	xtrb0	r1, r1

0+0102 <xtrb1>:
 102:	01 22       	xtrb1	r1, r2

0+0104 <xtrb2>:
 104:	01 10       	xtrb2	r1, r0

0+0106 <xtrb3>:
 106:	01 0d       	xtrb3	r1, r13

0+0108 <zextb>:
 108:	01 48       	zextb	r8

0+010a <zexth>:
 10a:	01 64       	zexth	r4
 10c:	0f 00       	cmpne	r0, r0
 10e:	0f 00       	cmpne	r0, r0
