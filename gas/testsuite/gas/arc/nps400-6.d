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
