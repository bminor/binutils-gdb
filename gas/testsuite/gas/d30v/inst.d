#objdump: -ldr
#name: D30V basic instructions
#as: 

.*: +file format elf32-d30v

Disassembly of section .text:

00000000 <start>:
   0:	08815a80 88001083 	abs	r21, r42	->	add	r1, r2, r3
   8:	080b2cda 00f00000 	add	r50, r51, 0x1a	||	nop	
  10:	880b2cf7 8ab1beef 	add	r50, r51, 0xdeadbeef
  18:	08101083 881b2cda 	add2h	r1, r2, r3	->	add2h	r50, r51, 0x1a
  20:	881b2cf7 8ab1beef 	add2h	r50, r51, 0xdeadbeef
  28:	08401083 884b2cda 	addc	r1, r2, r3	->	addc	r50, r51, 0x1a
  30:	884b2cf7 8ab1beef 	addc	r50, r51, 0xdeadbeef
  38:	09001083 890b2cda 	addhlll	r1, r2, r3	->	addhlll	r50, r51, 0x1a
  40:	890b2cf7 8ab1beef 	addhlll	r50, r51, 0xdeadbeef
  48:	09101083 891b2cda 	addhllh	r1, r2, r3	->	addhllh	r50, r51, 0x1a
  50:	891b2cf7 8ab1beef 	addhllh	r50, r51, 0xdeadbeef
  58:	09201083 892b2cda 	addhlhl	r1, r2, r3	->	addhlhl	r50, r51, 0x1a
  60:	892b2cf7 8ab1beef 	addhlhl	r50, r51, 0xdeadbeef
  68:	09301083 893b2cda 	addhlhh	r1, r2, r3	->	addhlhh	r50, r51, 0x1a
  70:	893b2cf7 8ab1beef 	addhlhh	r50, r51, 0xdeadbeef
  78:	09401083 894b2cda 	addhhll	r1, r2, r3	->	addhhll	r50, r51, 0x1a
  80:	894b2cf7 8ab1beef 	addhhll	r50, r51, 0xdeadbeef
  88:	09501083 895b2cda 	addhhlh	r1, r2, r3	->	addhhlh	r50, r51, 0x1a
  90:	895b2cf7 8ab1beef 	addhhlh	r50, r51, 0xdeadbeef
  98:	09601083 896b2cda 	addhhhl	r1, r2, r3	->	addhhhl	r50, r51, 0x1a
  a0:	896b2cf7 8ab1beef 	addhhhl	r50, r51, 0xdeadbeef
  a8:	09701083 897b2cda 	addhhhh	r1, r2, r3	->	addhhhh	r50, r51, 0x1a
  b0:	897b2cf7 8ab1beef 	addhhhh	r50, r51, 0xdeadbeef
  b8:	08601083 886b2cda 	adds	r1, r2, r3	->	adds	r50, r51, 0x1a
  c0:	886b2cf7 8ab1beef 	adds	r50, r51, 0xdeadbeef
  c8:	08701083 887b2cda 	adds2h	r1, r2, r3	->	adds2h	r50, r51, 0x1a
  d0:	887b2cf7 8ab1beef 	adds2h	r50, r51, 0xdeadbeef
  d8:	03801083 838b2cda 	and	r1, r2, r3	->	and	r50, r51, 0x1a
  e0:	838b2cf7 8ab1beef 	and	r50, r51, 0xdeadbeef
  e8:	02800042 82883105 	andfg	f0, f1, f2	->	andfg	f3, s, 0x5
  f0:	08a01083 88a84146 	avg	r1, r2, r3	->	avg	r4, r5, 0x6
  f8:	88ab2cf7 8ab1beef 	avg	r50, r51, 0xdeadbeef
 100:	08b01083 88b84146 	avg2h	r1, r2, r3	->	avg2h	r4, r5, 0x6
 108:	88bb2cf7 8ab1beef 	avg2h	r50, r51, 0xdeadbeef
 110:	02301083 82384146 	bclr	r1, r2, r3	->	bclr	r4, r5, 0x6
 118:	02101083 82185cc6 	bnot	r1, r2, r3	->	bnot	r5, r51, 0x6
 120:	00000029 80080040 	bra	r41	->	bra	320 <start\+320>
 128:	0008f00d 8046902a 	bra	ffff8190 <start\+ffff8190>	->	bratnz	r41, r42
 130:	804c1000 8000f00d 	bratnz	r1, f13d <start\+f13d>
 138:	804c1037 8ab1f00d 	bratnz	r1, deadf145 <start\+deadf145>
 140:	0042902a 00f00000 	bratzr	r41, r42	||	nop	
 148:	80481000 8000f00d 	bratzr	r1, f155 <start\+f155>
 150:	80481037 8ab1f00d 	bratzr	r1, deadf15d <start\+deadf15d>
 158:	02201083 82285cc6 	bset	r1, r2, r3	->	bset	r5, r51, 0x6
 160:	00200029 00f00000 	bsr	r41	||	nop	
 168:	0028f00d 00f00000 	bsr	ffff81d0 <start\+ffff81d0>	||	nop	
 170:	80280037 8ab1f00d 	bsr	deadf17d <start\+deadf17d>
 178:	0066902a 00f00000 	bsrtnz	r41, r42	||	nop	
 180:	806c1000 8000f00d 	bsrtnz	r1, f18d <start\+f18d>
 188:	806c1037 8ab1f00d 	bsrtnz	r1, deadf195 <start\+deadf195>
 190:	0062902a 00f00000 	bsrtzr	r41, r42	||	nop	
 198:	80681000 8000f00d 	bsrtzr	r1, f1a5 <start\+f1a5>
 1a0:	80681037 8ab1f00d 	bsrtzr	r1, deadf1ad <start\+deadf1ad>
 1a8:	02001083 82085cc6 	btst	f1, r2, r3	->	btst	v, r51, 0x6
 1b0:	02c000c1 82c09515 	cmpeq	f0, r3, r1	->	cmpne	f1, r20, r21
 1b8:	02c127e0 82c1b0c4 	cmpgt	f2, r31, r32	->	cmpge	f3, r3, r4
 1c0:	02c240c4 82c2d0c4 	cmplt	s, r3, r4	->	cmple	v, r3, r4
 1c8:	02c360c4 82c3f0c4 	cmpps	va, r3, r4	->	cmpng	c, r3, r4
 1d0:	02d127e0 82d1b0c4 	cmpugt	f2, r31, r32	->	cmpuge	f3, r3, r4
 1d8:	02d240c4 82d2d0c4 	cmpult	s, r3, r4	->	cmpule	v, r3, r4
 1e0:	01001008 81085000 	dbra	r1, r8	->	dbra	r5, 320 <start\+320>
 1e8:	81081037 8ab1f00d 	dbra	r1, deadf1f5 <start\+deadf1f5>
 1f0:	0140001f 8148400a 	dbrai	20f <start\+20f>, r31	->	dbrai	1fa <start\+1fa>, fffff9f0 <start\+fffff9f0>
 1f8:	8148003f 8ab1f00d 	dbrai	1f7 <start\+1f7>, feadf205 <start\+feadf205>
 200:	01201008 00f00000 	dbsr	r1, r8	||	nop	
 208:	01285000 00f00000 	dbsr	r5, 348 <start\+348>	||	nop	
 210:	81281037 8ab1f00d 	dbsr	r1, deadf21d <start\+deadf21d>
 218:	0160001f 00f00000 	dbsri	237 <start\+237>, r31	||	nop	
 220:	0168400a 00f00000 	dbsri	22a <start\+22a>, fffffa20 <start\+fffffa20>	||	nop	
 228:	8168003f 8ab1f00d 	dbsri	227 <start\+227>, feadf235 <start\+feadf235>
 230:	01101020 00f00000 	djmp	r1, r32	||	nop	
 238:	81181000 8000f00d 	djmp	r1, f00d <start\+f00d>
 240:	81181037 8ab1f00d 	djmp	r1, deadf00d <start\+deadf00d>
 248:	0150002a 00f00000 	djmpi	2a <start\+2a>, r42	||	nop	
 250:	8158000a 8000f00d 	djmpi	a <start\+a>, 2800f00d <start\+2800f00d>
 258:	8158003f 8ab1f00d 	djmpi	3f <start\+3f>, feadf00d <start\+feadf00d>
 260:	01301020 00f00000 	djsr	r1, r32	||	nop	
 268:	81381000 8000f00d 	djsr	r1, f00d <start\+f00d>
 270:	81381037 8ab1f00d 	djsr	r1, deadf00d <start\+deadf00d>
 278:	0170002a 00f00000 	djsri	2a <start\+2a>, r42	||	nop	
 280:	8178000a 8000f00d 	djsri	a <start\+a>, 2800f00d <start\+2800f00d>
 288:	8178003f 8ab1f00d 	djsri	3f <start\+3f>, feadf00d <start\+feadf00d>
 290:	00100029 8018f00d 	jmp	r41	->	jmp	78068 <start\+78068>
 298:	80180037 8ab1f00d 	jmp	deadf00d <start\+deadf00d>
 2a0:	0056902a 00f00000 	jmptnz	r41, r42	||	nop	
 2a8:	805c1000 8000f00d 	jmptnz	r1, f00d <start\+f00d>
 2b0:	805c1037 8ab1f00d 	jmptnz	r1, deadf00d <start\+deadf00d>
 2b8:	0052902a 00f00000 	jmptzr	r41, r42	||	nop	
 2c0:	80581000 8000f00d 	jmptzr	r1, f00d <start\+f00d>
 2c8:	80581037 8ab1f00d 	jmptzr	r1, deadf00d <start\+deadf00d>
 2d0:	08c01084 88c8108f 	joinll	r1, r2, r4	->	joinll	r1, r2, 0xf
 2d8:	88c810b7 8ab1f00d 	joinll	r1, r2, 0xdeadf00d
 2e0:	08d01084 88d8108f 	joinlh	r1, r2, r4	->	joinlh	r1, r2, 0xf
 2e8:	88d810b7 8ab1f00d 	joinlh	r1, r2, 0xdeadf00d
 2f0:	08e01084 88e8108f 	joinhl	r1, r2, r4	->	joinhl	r1, r2, 0xf
 2f8:	88e810b7 8ab1f00d 	joinhl	r1, r2, 0xdeadf00d
 300:	08f01084 88f8108f 	joinhh	r1, r2, r4	->	joinhh	r1, r2, 0xf
 308:	88f810b7 8ab1f00d 	joinhh	r1, r2, 0xdeadf00d
 310:	00300029 00f00000 	jsr	r41	||	nop	
 318:	0038f00d 00f00000 	jsr	78068 <start\+78068>	||	nop	
 320:	80380037 8ab1f00d 	jsr	deadf00d <start\+deadf00d>
 328:	0076902a 00f00000 	jsrtnz	r41, r42	||	nop	
 330:	807c1000 8000f00d 	jsrtnz	r1, f00d <start\+f00d>
 338:	807c1037 8ab1f00d 	jsrtnz	r1, deadf00d <start\+deadf00d>
 340:	0072902a 00f00000 	jsrtzr	r41, r42	||	nop	
 348:	80781000 8000f00d 	jsrtzr	r1, f00d <start\+f00d>
 350:	80781037 8ab1f00d 	jsrtzr	r1, deadf00d <start\+deadf00d>
 358:	043061c8 843461c8 	ld2h	r6, \@\(r7, r8\)	->	ld2h	r6, \@\(r7\+, r8\)
 360:	043c61c8 843861da 	ld2h	r6, \@\(r7-, r8\)	->	ld2h	r6, \@\(r7, 0x1a\)
 368:	843861c0 80001234 	ld2h	r6, \@\(r7, 0x1234\)
 370:	046061c8 846461c8 	ld2w	r6, \@\(r7, r8\)	->	ld2w	r6, \@\(r7\+, r8\)
 378:	046c61c8 846861da 	ld2w	r6, \@\(r7-, r8\)	->	ld2w	r6, \@\(r7, 0x1a\)
 380:	846861c0 80001234 	ld2w	r6, \@\(r7, 0x1234\)
 388:	045061c8 845461c8 	ld4bh	r6, \@\(r7, r8\)	->	ld4bh	r6, \@\(r7\+, r8\)
 390:	045c61c8 845861da 	ld4bh	r6, \@\(r7-, r8\)	->	ld4bh	r6, \@\(r7, 0x1a\)
 398:	845861c0 80001234 	ld4bh	r6, \@\(r7, 0x1234\)
 3a0:	04d061c8 84d461c8 	ld4bhu	r6, \@\(r7, r8\)	->	ld4bhu	r6, \@\(r7\+, r8\)
 3a8:	04dc61c8 84d861da 	ld4bhu	r6, \@\(r7-, r8\)	->	ld4bhu	r6, \@\(r7, 0x1a\)
 3b0:	84d861c0 80001234 	ld4bhu	r6, \@\(r7, 0x1234\)
 3b8:	040061c8 840461c8 	ldb	r6, \@\(r7, r8\)	->	ldb	r6, \@\(r7\+, r8\)
 3c0:	040c61c8 840861da 	ldb	r6, \@\(r7-, r8\)	->	ldb	r6, \@\(r7, 0x1a\)
 3c8:	840861c0 80001234 	ldb	r6, \@\(r7, 0x1234\)
 3d0:	049061c8 849461c8 	ldbu	r6, \@\(r7, r8\)	->	ldbu	r6, \@\(r7\+, r8\)
 3d8:	049c61c8 849861da 	ldbu	r6, \@\(r7-, r8\)	->	ldbu	r6, \@\(r7, 0x1a\)
 3e0:	849861c0 80001234 	ldbu	r6, \@\(r7, 0x1234\)
 3e8:	042061c8 842461c8 	ldh	r6, \@\(r7, r8\)	->	ldh	r6, \@\(r7\+, r8\)
 3f0:	042c61c8 842861da 	ldh	r6, \@\(r7-, r8\)	->	ldh	r6, \@\(r7, 0x1a\)
 3f8:	842861c0 80001234 	ldh	r6, \@\(r7, 0x1234\)
 400:	041061c8 841461c8 	ldhh	r6, \@\(r7, r8\)	->	ldhh	r6, \@\(r7\+, r8\)
 408:	041c61c8 841861da 	ldhh	r6, \@\(r7-, r8\)	->	ldhh	r6, \@\(r7, 0x1a\)
 410:	841861c0 80001234 	ldhh	r6, \@\(r7, 0x1234\)
 418:	04a061c8 84a461c8 	ldhu	r6, \@\(r7, r8\)	->	ldhu	r6, \@\(r7\+, r8\)
 420:	04ac61c8 84a861da 	ldhu	r6, \@\(r7-, r8\)	->	ldhu	r6, \@\(r7, 0x1a\)
 428:	84a861c0 80001234 	ldhu	r6, \@\(r7, 0x1234\)
 430:	044061c8 844461c8 	ldw	r6, \@\(r7, r8\)	->	ldw	r6, \@\(r7\+, r8\)
 438:	044c61c8 844861da 	ldw	r6, \@\(r7-, r8\)	->	ldw	r6, \@\(r7, 0x1a\)
 440:	844861c0 80001234 	ldw	r6, \@\(r7, 0x1234\)
 448:	8b48109f 0b401084 	mac0	r1, r2, 0x1f	<-	mac0	r1, r2, r4
 450:	8b4c109f 0b441084 	mac1	r1, r2, 0x1f	<-	mac1	r1, r2, r4
 458:	8b58109f 0b501084 	macs0	r1, r2, 0x1f	<-	macs0	r1, r2, r4
 460:	8b5c109f 0b541084 	macs1	r1, r2, 0x1f	<-	macs1	r1, r2, r4
 468:	047c004a 8474004a 	moddec	r1, 0xa	->	modinc	r1, 0xa
 470:	8b68109f 0b601084 	msub0	r1, r2, 0x1f	<-	msub0	r1, r2, r4
 478:	8b6c109f 0b641084 	msub1	r1, r2, 0x1f	<-	msub1	r1, r2, r4
 480:	8b78109f 0b701084 	msubs0	r1, r2, 0x1f	<-	msubs0	r1, r2, r4
 488:	8b7c109f 0b741084 	msubs1	r1, r2, 0x1f	<-	msubs1	r1, r2, r4
 490:	8b08108a 0b001084 	mul	r1, r2, 0xa	<-	mul	r1, r2, r4
 498:	8a08108a 0a001084 	mul2h	r1, r2, 0xa	<-	mul2h	r1, r2, r4
 4a0:	8a48108a 0a401084 	mulhxll	r1, r2, 0xa	<-	mulhxll	r1, r2, r4
 4a8:	8a58108a 0a501084 	mulhxlh	r1, r2, 0xa	<-	mulhxlh	r1, r2, r4
 4b0:	8a68108a 0a601084 	mulhxhl	r1, r2, 0xa	<-	mulhxhl	r1, r2, r4
 4b8:	8a78108a 0a701084 	mulhxhh	r1, r2, 0xa	<-	mulhxhh	r1, r2, r4
 4c0:	8b88108a 0b800044 	mulx	a1, r2, 0xa	<-	mulx	a0, r1, r4
 4c8:	8a18108a 0a101084 	mulx2h	r1, r2, 0xa	<-	mulx2h	r1, r2, r4
 4d0:	8b98108a 0b900044 	mulxs	a1, r2, 0xa	<-	mulxs	a0, r1, r4
 4d8:	8bf8204a 0bf01004 	mvfacc	r2, a1, 0xa	<-	mvfacc	r1, a0, r4
 4e0:	01e8a000 81e8a180 	mvfsys	r10, pc	->	mvfsys	r10, rpt_c
 4e8:	01e8a040 81e8a042 	mvfsys	r10, psw	->	mvfsys	r10, pswh
 4f0:	01e8a041 81e8a003 	mvfsys	r10, pswl	->	mvfsys	r10, f0
 4f8:	01e8a103 8af01084 	mvfsys	r10, s	->	mvtacc	a1, r2, r4
 500:	00e86280 80e81280 	mvtsys	rpt_c, r10	->	mvtsys	psw, r10
 508:	00e81282 80e81281 	mvtsys	pswh, r10	->	mvtsys	pswl, r10
 510:	00e80283 80e83283 	mvtsys	f0, r10	->	mvtsys	f3, r10
 518:	00e84283 80e85283 	mvtsys	s, r10	->	mvtsys	v, r10
 520:	00e86283 80e87283 	mvtsys	va, r10	->	mvtsys	c, r10
 528:	00f00000 83901080 	nop		->	not	r1, r2
 530:	02901080 83a01084 	notfg	f1, f2	->	or	r1, r2, r4
 538:	03a8109a 00f00000 	or	r1, r2, 0x1a	||	nop	
 540:	83a810b7 8ab1f00d 	or	r1, r2, 0xdeadf00d
 548:	02a01084 82a84081 	orfg	f1, f2, s	->	orfg	s, f2, 0x1
 550:	00800000 81801002 	reit		->	repeat	r1, r2
 558:	81884000 8000dead 	repeat	r4, 0xdead
 560:	81884037 8ab1f00d 	repeat	r4, 0xdeadf00d
 568:	01a0000b 00f00000 	repeati	0xb, r11	||	nop	
 570:	81a8000a 80001001 	repeati	0xa, 0x28001001
 578:	03401084 8348108a 	rot	r1, r2, r4	->	rot	r1, r2, 0xa
 580:	03501084 8358108a 	rot2h	r1, r2, r4	->	rot2h	r1, r2, 0xa
 588:	8a88108a 0a801084 	sat	r1, r2, 0xa	<-	sat	r1, r2, r4
 590:	8a98108a 0a901084 	sat2h	r1, r2, 0xa	<-	sat2h	r1, r2, r4
 598:	8bc8108a 0bc01084 	sathl	r1, r2, 0xa	<-	sathl	r1, r2, r4
 5a0:	8bd8108a 0bd01084 	sathh	r1, r2, 0xa	<-	sathh	r1, r2, r4
 5a8:	8aa8108a 0aa01084 	satz	r1, r2, 0xa	<-	satz	r1, r2, r4
 5b0:	8ab8108a 0ab01084 	satz2h	r1, r2, 0xa	<-	satz2h	r1, r2, r4
 5b8:	03001084 8308108a 	sra	r1, r2, r4	->	sra	r1, r2, 0xa
 5c0:	03101084 8318108a 	sra2h	r1, r2, r4	->	sra2h	r1, r2, 0xa
 5c8:	03601084 8368108a 	src	r1, r2, r4	->	src	r1, r2, 0xa
 5d0:	03201084 8328108a 	srl	r1, r2, r4	->	srl	r1, r2, 0xa
 5d8:	03201084 8328108a 	srl	r1, r2, r4	->	srl	r1, r2, 0xa
 5e0:	053061c8 853461c8 	st2h	r6, \@\(r7, r8\)	->	st2h	r6, \@\(r7\+, r8\)
 5e8:	053c61c8 853861da 	st2h	r6, \@\(r7-, r8\)	->	st2h	r6, \@\(r7, 0x1a\)
 5f0:	853861c0 80001234 	st2h	r6, \@\(r7, 0x1234\)
 5f8:	056061c8 856461c8 	st2w	r6, \@\(r7, r8\)	->	st2w	r6, \@\(r7\+, r8\)
 600:	056c61c8 856861da 	st2w	r6, \@\(r7-, r8\)	->	st2w	r6, \@\(r7, 0x1a\)
 608:	856861c0 80001234 	st2w	r6, \@\(r7, 0x1234\)
 610:	055061c8 855461c8 	st4hb	r6, \@\(r7, r8\)	->	st4hb	r6, \@\(r7\+, r8\)
 618:	055c61c8 855861da 	st4hb	r6, \@\(r7-, r8\)	->	st4hb	r6, \@\(r7, 0x1a\)
 620:	855861c0 80001234 	st4hb	r6, \@\(r7, 0x1234\)
 628:	050061c8 850461c8 	stb	r6, \@\(r7, r8\)	->	stb	r6, \@\(r7\+, r8\)
 630:	050c61c8 850861da 	stb	r6, \@\(r7-, r8\)	->	stb	r6, \@\(r7, 0x1a\)
 638:	850861c0 80001234 	stb	r6, \@\(r7, 0x1234\)
 640:	052061c8 852461c8 	sth	r6, \@\(r7, r8\)	->	sth	r6, \@\(r7\+, r8\)
 648:	052c61c8 852861da 	sth	r6, \@\(r7-, r8\)	->	sth	r6, \@\(r7, 0x1a\)
 650:	852861c0 80001234 	sth	r6, \@\(r7, 0x1234\)
 658:	051061c8 851461c8 	sthh	r6, \@\(r7, r8\)	->	sthh	r6, \@\(r7\+, r8\)
 660:	051c61c8 851861da 	sthh	r6, \@\(r7-, r8\)	->	sthh	r6, \@\(r7, 0x1a\)
 668:	851861c0 80001234 	sthh	r6, \@\(r7, 0x1234\)
 670:	054061c8 854461c8 	stw	r6, \@\(r7, r8\)	->	stw	r6, \@\(r7\+, r8\)
 678:	054c61c8 854861da 	stw	r6, \@\(r7-, r8\)	->	stw	r6, \@\(r7, 0x1a\)
 680:	854861c0 80001234 	stw	r6, \@\(r7, 0x1234\)
 688:	08201083 882b2cda 	sub	r1, r2, r3	->	sub	r50, r51, 0x1a
 690:	882b2cf7 8ab1beef 	sub	r50, r51, 0xdeadbeef
 698:	08301083 883b2cda 	sub2h	r1, r2, r3	->	sub2h	r50, r51, 0x1a
 6a0:	883b2cf7 8ab1beef 	sub2h	r50, r51, 0xdeadbeef
 6a8:	08501083 885b2cda 	subb	r1, r2, r3	->	subb	r50, r51, 0x1a
 6b0:	885b2cf7 8ab1beef 	subb	r50, r51, 0xdeadbeef
 6b8:	09801083 898b2cda 	subhlll	r1, r2, r3	->	subhlll	r50, r51, 0x1a
 6c0:	898b2cf7 8ab1beef 	subhlll	r50, r51, 0xdeadbeef
 6c8:	09901083 899b2cda 	subhllh	r1, r2, r3	->	subhllh	r50, r51, 0x1a
 6d0:	899b2cf7 8ab1beef 	subhllh	r50, r51, 0xdeadbeef
 6d8:	09a01083 89ab2cda 	subhlhl	r1, r2, r3	->	subhlhl	r50, r51, 0x1a
 6e0:	89ab2cf7 8ab1beef 	subhlhl	r50, r51, 0xdeadbeef
 6e8:	09b01083 89bb2cda 	subhlhh	r1, r2, r3	->	subhlhh	r50, r51, 0x1a
 6f0:	89bb2cf7 8ab1beef 	subhlhh	r50, r51, 0xdeadbeef
 6f8:	09c01083 89cb2cda 	subhhll	r1, r2, r3	->	subhhll	r50, r51, 0x1a
 700:	89cb2cf7 8ab1beef 	subhhll	r50, r51, 0xdeadbeef
 708:	09d01083 89db2cda 	subhhlh	r1, r2, r3	->	subhhlh	r50, r51, 0x1a
 710:	89db2cf7 8ab1beef 	subhhlh	r50, r51, 0xdeadbeef
 718:	09e01083 89eb2cda 	subhhhl	r1, r2, r3	->	subhhhl	r50, r51, 0x1a
 720:	89eb2cf7 8ab1beef 	subhhhl	r50, r51, 0xdeadbeef
 728:	09f01083 89fb2cda 	subhhhh	r1, r2, r3	->	subhhhh	r50, r51, 0x1a
 730:	89fb2cf7 8ab1beef 	subhhhh	r50, r51, 0xdeadbeef
 738:	00900001 8098000a 	trap	r1	->	trap	0xa
 740:	03b01084 83b8108a 	xor	r1, r2, r4	->	xor	r1, r2, 0xa
 748:	83b810b7 8ab1f00d 	xor	r1, r2, 0xdeadf00d
 750:	02b01084 82b8110a 	xorfg	f1, f2, s	->	xorfg	f1, s, 0xa
