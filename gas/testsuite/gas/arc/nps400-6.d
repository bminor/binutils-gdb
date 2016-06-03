#as: -mcpu=nps400
#objdump: -dr

.*: +file format .*arc.*

Disassembly of section .text:

[0-9a-f]+ <.*>:
   0:	4820 1020           	addb	r0,r0,r1,0,0x8,0x2
   4:	4830 1820           	addb.f	r0,r0,r1,0x10,0x8,0x2
   8:	4830 74a0           	addb.f.sx	r0,r0,r1,0x8,0x18,0x6
   c:	4820 1024           	subb	r0,r0,r1,0,0x8,0x2
  10:	4830 1824           	subb.f	r0,r0,r1,0x10,0x8,0x2
  14:	4830 74a4           	subb.f.sx	r0,r0,r1,0x8,0x18,0x6
  18:	4820 1025           	adcb	r0,r0,r1,0,0x8,0x2
  1c:	4830 1825           	adcb.f	r0,r0,r1,0x10,0x8,0x2
  20:	4830 74a5           	adcb.f.sx	r0,r0,r1,0x8,0x18,0x6
  24:	4820 1026           	sbcb	r0,r0,r1,0,0x8,0x2
  28:	4830 1826           	sbcb.f	r0,r0,r1,0x10,0x8,0x2
  2c:	4830 74a6           	sbcb.f.sx	r0,r0,r1,0x8,0x18,0x6
  30:	4820 1021           	andb	r0,r0,r1,0,0x8,0x2
  34:	4830 1821           	andb.f	r0,r0,r1,0x10,0x8,0x2
  38:	4820 1022           	xorb	r0,r0,r1,0,0x8,0x2
  3c:	4830 1822           	xorb.f	r0,r0,r1,0x10,0x8,0x2
  40:	4820 1023           	orb	r0,r0,r1,0,0x8,0x2
  44:	4830 1823           	orb.f	r0,r0,r1,0x10,0x8,0x2
  48:	4820 102b           	shlb	r0,r0,r1,0,0x8,0x2
  4c:	4830 182b           	shlb.f	r0,r0,r1,0x10,0x8,0x2
  50:	4820 102c           	shrb	r0,r0,r1,0,0x8,0x2
  54:	4830 182c           	shrb.f	r0,r0,r1,0x10,0x8,0x2
  58:	4820 1007           	fxorb	r0,r0,r1,0,0x8,0x8
  5c:	4830 1807           	fxorb.f	r0,r0,r1,0x10,0x8,0x8
  60:	4820 1008           	wxorb	r0,r0,r1,0,0x8,0x10
  64:	4830 1808           	wxorb.f	r0,r0,r1,0x10,0x8,0x10
  68:	4820 00e9           	notb	r0,r1,0,0x8
  6c:	4830 21e9           	notb.f	r0,r1,0x10,0x10
  70:	4820 00ea           	cntbb	r0,r1,0,0x8
  74:	4830 21ea           	cntbb.f	r0,r1,0x10,0x10
  78:	4840 540d           	div	r0,r0,r2,0x8,0x8
  7c:	4810 47ed           	div.f	r0,r0,0xf,0x8
  80:	4840 940d           	mod	r0,r0,r2,0x8,0x8
  84:	4810 87ed           	mod.f	r0,r0,0xf,0x8
  88:	4840 140d           	divm	r0,r0,r2,0x8,0x8
  8c:	4810 07ed           	divm.f	r0,r0,0xf,0x8
  90:	4a10 9e6e           	qcmp.ar	r2,r2,r0,0x8,0x8,0,0x1,0x3
  94:	4a10 dfee           	qcmp.ar	r2,r2,r0,0x8,0x8,0x1,0x1
  98:	4a10 5fee           	qcmp.ar	r2,r2,r0,0x8,0x8,0x1
  9c:	4a10 1fee           	qcmp.ar	r2,r2,r0,0x8,0x8
  a0:	4a10 9e6f           	qcmp.al	r2,r2,r0,0x8,0x8,0,0x1,0x3
  a4:	4a10 dfef           	qcmp.al	r2,r2,r0,0x8,0x8,0x1,0x1
  a8:	4a10 5fef           	qcmp.al	r2,r2,r0,0x8,0x8,0x1
  ac:	4a10 1fef           	qcmp.al	r2,r2,r0,0x8,0x8
  b0:	4800 0010           	calcsd	r0,r0,r0,0x1
  b4:	4910 0110           	calcsd.f	r1,r1,r0,0x2
  b8:	4a60 0210           	calcsd	r2,r2,r3,0x4
  bc:	4810 0310           	calcsd.f	r0,r0,r0,0x8
  c0:	4b00 0410           	calcsd	r3,r3,r0,0x10
  c4:	4810 0510           	calcsd.f	r0,r0,r0,0x20
  c8:	4820 0610           	calcsd	r0,r0,r1,0x40
  cc:	4a10 0710           	calcsd.f	r2,r2,r0,0x80
  d0:	4940 0810           	calcsd	r1,r1,r2,0x100
  d4:	4800 4010           	calcxd	r0,r0,r0,0x1
  d8:	4910 4110           	calcxd.f	r1,r1,r0,0x2
  dc:	4a60 4210           	calcxd	r2,r2,r3,0x4
  e0:	4810 4310           	calcxd.f	r0,r0,r0,0x8
  e4:	4b00 4410           	calcxd	r3,r3,r0,0x10
  e8:	4810 4510           	calcxd.f	r0,r0,r0,0x20
  ec:	4820 4610           	calcxd	r0,r0,r1,0x40
  f0:	4a10 4710           	calcxd.f	r2,r2,r0,0x80
  f4:	4940 4810           	calcxd	r1,r1,r2,0x100
  f8:	4800 0030           	calcbsd	r0,r0,r0
  fc:	4910 0030           	calcbsd.f	r1,r1,r0
 100:	4940 0030           	calcbsd	r1,r1,r2
 104:	4b50 0030           	calcbsd.f	r3,r3,r2
 108:	4800 4030           	calcbxd	r0,r0,r0
 10c:	4910 4030           	calcbxd.f	r1,r1,r0
 110:	4940 4030           	calcbxd	r1,r1,r2
 114:	4b50 4030           	calcbxd.f	r3,r3,r2
 118:	4800 0050           	calckey	r0,r0,r0
 11c:	4910 0050           	calckey.f	r1,r1,r0
 120:	4940 0050           	calckey	r1,r1,r2
 124:	4b50 0050           	calckey.f	r3,r3,r2
 128:	4800 4050           	calcxkey	r0,r0,r0
 12c:	4910 4050           	calcxkey.f	r1,r1,r0
 130:	4940 4050           	calcxkey	r1,r1,r2
 134:	4b50 4050           	calcxkey.f	r3,r3,r2
 138:	582b 0440           	mxb	r0,r1,0,0x1,0x2
 13c:	582b 0e38           	mxb	r0,r1,0x7,0x8,0x7
 140:	582b 01f8           	mxb	r0,r1,0x7,0x7,0x8
 144:	5a6b a708           	mxb.s	r2,r3,0x1,0x4,0x3,0x2
 148:	5a6b fe38           	mxb.s	r2,r3,0x7,0x8,0x7,0x7
 14c:	5a6b f1f8           	mxb.s	r2,r3,0x7,0x7,0x8,0x7
 150:	5a6b 8e38           	mxb.s	r2,r3,0x7,0x8,0x7,0x8
 154:	582b 0441           	imxb	r0,r1,0,0x1,0x2
 158:	582b 0e39           	imxb	r0,r1,0x7,0x8,0x7
 15c:	582b 01f9           	imxb	r0,r1,0x7,0x7,0x8
 160:	5a6b a709           	imxb.s	r2,r3,0x1,0x4,0x3,0x2
 164:	5a6b fe39           	imxb.s	r2,r3,0x7,0x8,0x7,0x7
 168:	5a6b f1f9           	imxb.s	r2,r3,0x7,0x7,0x8,0x7
 16c:	5a6b 8e39           	imxb.s	r2,r3,0x7,0x8,0x7,0x8
 170:	480a 0000           	addl	r0,r0,0
 174:	484a 7fff           	addl	r2,r2,32767
 178:	48aa 8000           	addl	r5,r5,-32768
 17c:	495a 0000           	addl.f	r10,r10,0
 180:	497a 0001           	addl.f	r11,r11,1
 184:	499a ffff           	addl.f	r12,r12,-1
 188:	480b 0000           	subl	r0,r0,0
 18c:	484b 7fff           	subl	r2,r2,32767
 190:	48ab 8000           	subl	r5,r5,-32768
 194:	495b 0000           	subl.f	r10,r10,0
 198:	497b 0001           	subl.f	r11,r11,1
 19c:	499b ffff           	subl.f	r12,r12,-1
 1a0:	480d 0000           	andl	r0,r0,0
 1a4:	482d 0001           	andl	r1,r1,0x1
 1a8:	484d ffff           	andl	r2,r2,0xffff
 1ac:	495d 0000           	andl.f	r10,r10,0
 1b0:	497d 0001           	andl.f	r11,r11,0x1
 1b4:	499d ffff           	andl.f	r12,r12,0xffff
 1b8:	480c 0000           	orl	r0,r0,0
 1bc:	482c 0001           	orl	r1,r1,0x1
 1c0:	484c ffff           	orl	r2,r2,0xffff
 1c4:	495c 0000           	orl.f	r10,r10,0
 1c8:	497c 0001           	orl.f	r11,r11,0x1
 1cc:	499c ffff           	orl.f	r12,r12,0xffff
 1d0:	480e 0000           	xorl	r0,r0,0
 1d4:	482e 0001           	xorl	r1,r1,0x1
 1d8:	484e ffff           	xorl	r2,r2,0xffff
 1dc:	495e 0000           	xorl.f	r10,r10,0
 1e0:	497e 0001           	xorl.f	r11,r11,0x1
 1e4:	499e ffff           	xorl.f	r12,r12,0xffff
 1e8:	4800 0011           	andab	r0,r0,0,0x1
 1ec:	4940 3df1           	andab	r1,r2,0xf,0x10
 1f0:	4a70 0011           	andab.f	r2,r3,0,0x1
 1f4:	4cb0 3e11           	andab.f	r12,r13,0x10,0x10
 1f8:	4800 8011           	andab	r0,r0,r0,0,0x1
 1fc:	4940 bdf1           	andab	r1,r1,r2,0xf,0x10
 200:	4a70 8011           	andab.f	r2,r2,r3,0,0x1
 204:	4cb0 be11           	andab.f	r12,r12,r13,0x10,0x10
 208:	4800 0012           	orab	r0,r0,0,0x1
 20c:	4940 3df2           	orab	r1,r2,0xf,0x10
 210:	4a70 0012           	orab.f	r2,r3,0,0x1
 214:	4cb0 3e12           	orab.f	r12,r13,0x10,0x10
 218:	4800 8012           	orab	r0,r0,r0,0,0x1
 21c:	4940 bdf2           	orab	r1,r1,r2,0xf,0x10
 220:	4a70 8012           	orab.f	r2,r2,r3,0,0x1
 224:	4cb0 be12           	orab.f	r12,r12,r13,0x10,0x10
 228:	382f 0045           	lbdsize	r0,r1
 22c:	3a2f 00c5           	lbdsize	r2,r3
 230:	382f 8045           	lbdsize.f	r0,r1
 234:	3a2f 80c5           	lbdsize.f	r2,r3
 238:	4820 0033           	bdlen	r0,r1,0x1
 23c:	4960 0013           	bdlen	r1,r3,0x100
 240:	4940 1e13           	bdlen	r1,r2,0xf0
 244:	4ca0 4013           	bdlen	r12,r13
 248:	4830 0033           	bdlen.f	r0,r1,0x1
 24c:	4970 0013           	bdlen.f	r1,r3,0x100
 250:	4cb0 4013           	bdlen.f	r12,r13
 254:	3c2a 150a           	csma	r10,r12,r20
 258:	3e2a 7500 1234 5678 	csma	r0,0x12345678,r20
 260:	3f2a 0f86 ffff ffff 	csma	r6,r7,0xffffffff
 268:	3e2a 7f88 ffff ffff 	csma	r8,0xffffffff,0xffffffff
 270:	3e2a 137e           	csma	0,r14,r13
 274:	3e2a 72be ffff ffff 	csma	0,0xffffffff,r10
 27c:	3c2a 1fbe ffff ffff 	csma	0,r12,0xffffffff
 284:	3d6a 0044           	csma	r4,r5,0x1
 288:	3e6a 7083 1234 5678 	csma	r3,0x12345678,0x2
 290:	396a 013e           	csma	0,r1,0x4
 294:	3e6a 707e ffff ffff 	csma	0,0xffffffff,0x1
 29c:	3c2c 150a           	csms	r10,r12,r20
 2a0:	3e2c 7500 1234 5678 	csms	r0,0x12345678,r20
 2a8:	3f2c 0f86 ffff ffff 	csms	r6,r7,0xffffffff
 2b0:	3e2c 7f88 ffff ffff 	csms	r8,0xffffffff,0xffffffff
 2b8:	3e2c 137e           	csms	0,r14,r13
 2bc:	3e2c 72be ffff ffff 	csms	0,0xffffffff,r10
 2c4:	3c2c 1fbe ffff ffff 	csms	0,r12,0xffffffff
 2cc:	3d6c 0044           	csms	r4,r5,0x1
 2d0:	3e6c 7083 1234 5678 	csms	r3,0x12345678,0x2
 2d8:	396c 013e           	csms	0,r1,0x4
 2dc:	3e6c 707e ffff ffff 	csms	0,0xffffffff,0x1
 2e4:	3c2d 150a           	cbba	r10,r12,r20
 2e8:	3e2d 7500 1234 5678 	cbba	r0,0x12345678,r20
 2f0:	3f2d 0f86 ffff ffff 	cbba	r6,r7,0xffffffff
 2f8:	3e2d 7f88 ffff ffff 	cbba	r8,0xffffffff,0xffffffff
 300:	3e2d 137e           	cbba	0,r14,r13
 304:	3e2d 72be ffff ffff 	cbba	0,0xffffffff,r10
 30c:	3c2d 1fbe ffff ffff 	cbba	0,r12,0xffffffff
 314:	3d6d 0044           	cbba	r4,r5,0x1
 318:	3e6d 7083 1234 5678 	cbba	r3,0x12345678,0x2
 320:	396d 013e           	cbba	0,r1,0x4
 324:	3e6d 707e ffff ffff 	cbba	0,0xffffffff,0x1
 32c:	3c2d 950a           	cbba.f	r10,r12,r20
 330:	3e2d f500 1234 5678 	cbba.f	r0,0x12345678,r20
 338:	3f2d 8f86 ffff ffff 	cbba.f	r6,r7,0xffffffff
 340:	3e2d ff88 ffff ffff 	cbba.f	r8,0xffffffff,0xffffffff
 348:	3e2d 937e           	cbba.f	0,r14,r13
 34c:	3e2d f2be ffff ffff 	cbba.f	0,0xffffffff,r10
 354:	3c2d 9fbe ffff ffff 	cbba.f	0,r12,0xffffffff
 35c:	3d6d 8044           	cbba.f	r4,r5,0x1
 360:	3e6d f083 1234 5678 	cbba.f	r3,0x12345678,0x2
 368:	396d 813e           	cbba.f	0,r1,0x4
 36c:	3e6d f07e ffff ffff 	cbba.f	0,0xffffffff,0x1
 374:	3c35 150a           	zncv.rd	r10,r12,r20
 378:	3e35 7500 1234 5678 	zncv.rd	r0,0x12345678,r20
 380:	3f35 0f86 ffff ffff 	zncv.rd	r6,r7,0xffffffff
 388:	3e35 7f88 ffff ffff 	zncv.rd	r8,0xffffffff,0xffffffff
 390:	3e35 137e           	zncv.rd	0,r14,r13
 394:	3e35 72be ffff ffff 	zncv.rd	0,0xffffffff,r10
 39c:	3c35 1fbe ffff ffff 	zncv.rd	0,r12,0xffffffff
 3a4:	3d75 0044           	zncv.rd	r4,r5,0x1
 3a8:	3e75 7083 1234 5678 	zncv.rd	r3,0x12345678,0x2
 3b0:	3975 013e           	zncv.rd	0,r1,0x4
 3b4:	3e75 707e ffff ffff 	zncv.rd	0,0xffffffff,0x1
 3bc:	39b5 0fff           	zncv.rd	r1,r1,-1
 3c0:	3eb5 7fff ffff ffff 	zncv.rd	0,0xffffffff,-1
 3c8:	3c35 950a           	zncv.wr	r10,r12,r20
 3cc:	3e35 f500 1234 5678 	zncv.wr	r0,0x12345678,r20
 3d4:	3f35 8f86 ffff ffff 	zncv.wr	r6,r7,0xffffffff
 3dc:	3e35 ff88 ffff ffff 	zncv.wr	r8,0xffffffff,0xffffffff
 3e4:	3e35 937e           	zncv.wr	0,r14,r13
 3e8:	3e35 f2be ffff ffff 	zncv.wr	0,0xffffffff,r10
 3f0:	3c35 9fbe ffff ffff 	zncv.wr	0,r12,0xffffffff
 3f8:	3d75 8044           	zncv.wr	r4,r5,0x1
 3fc:	3e75 f083 1234 5678 	zncv.wr	r3,0x12345678,0x2
 404:	3975 813e           	zncv.wr	0,r1,0x4
 408:	3e75 f07e ffff ffff 	zncv.wr	0,0xffffffff,0x1
 410:	39b5 8fff           	zncv.wr	r1,r1,-1
 414:	3eb5 ffff ffff ffff 	zncv.wr	0,0xffffffff,-1
 41c:	3a36 00c1           	hofs	r1,r2,r3
 420:	3d36 8184           	hofs.f	r4,r5,r6
 424:	3876 13c7           	hofs	r7,r8,0xf0,0
 428:	3876 9807           	hofs.f	r7,r8,0,0x1
