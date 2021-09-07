#as: -march=rv64gcxtheadc
#objdump: -dr

.*:[ 	]+file format .*


Disassembly of section .text:

0000000000000000 <.text>:
   0:	cff01073          	csrw	0xcff,zero
   4:	0010000b          	dcache.call
   8:	0030000b          	dcache.ciall
   c:	02b5000b          	dcache.cipa	a0
  10:	0235000b          	dcache.cisw	a0
  14:	0275000b          	dcache.civa	a0
  18:	0295000b          	dcache.cpa	a0
  1c:	0285000b          	dcache.cpal1	a0
  20:	0255000b          	dcache.cva	a0
  24:	0245000b          	dcache.cval1	a0
  28:	02a5000b          	dcache.ipa	a0
  2c:	0225000b          	dcache.isw	a0
  30:	0265000b          	dcache.iva	a0
  34:	0020000b          	dcache.iall
  38:	0215000b          	dcache.csw	a0
  3c:	0100000b          	icache.iall
  40:	0110000b          	icache.ialls
  44:	0305000b          	icache.iva	a0
  48:	0385000b          	icache.ipa	a0
  4c:	0160000b          	l2cache.iall
  50:	0150000b          	l2cache.call
  54:	0170000b          	l2cache.ciall
  58:	04b5000b          	sfence.vmas	a0,a1
  5c:	0180000b          	sync
  60:	01a0000b          	sync.i
  64:	0190000b          	sync.s
  68:	01b0000b          	sync.is
  6c:	02c5950b          	addsl	a0,a1,a2,1
  70:	20c5950b          	mula	a0,a1,a2
  74:	22c5950b          	muls	a0,a1,a2
  78:	24c5950b          	mulaw	a0,a1,a2
  7c:	26c5950b          	mulsw	a0,a1,a2
  80:	28c5950b          	mulah	a0,a1,a2
  84:	2ac5950b          	mulsh	a0,a1,a2
  88:	1105950b          	srri	a0,a1,16
  8c:	1505950b          	srriw	a0,a1,16
  90:	40c5950b          	mveqz	a0,a1,a2
  94:	42c5950b          	mvnez	a0,a1,a2
  98:	8905950b          	tst	a0,a1,16
  9c:	8005950b          	tstnbz	a0,a1
  a0:	4105a50b          	ext	a0,a1,16,16
  a4:	4105b50b          	extu	a0,a1,16,16
  a8:	8605950b          	ff1	a0,a1
  ac:	8405950b          	ff0	a0,a1
  b0:	8205950b          	rev	a0,a1
  b4:	9005950b          	revw	a0,a1
  b8:	62b5600b          	flrd	ft0,a0,a1,1
  bc:	42b5600b          	flrw	ft0,a0,a1,1
  c0:	72b5600b          	flurd	ft0,a0,a1,1
  c4:	52b5600b          	flurw	ft0,a0,a1,1
  c8:	02c5c50b          	lrb	a0,a1,a2,1
  cc:	22c5c50b          	lrh	a0,a1,a2,1
  d0:	42c5c50b          	lrw	a0,a1,a2,1
  d4:	62c5c50b          	lrd	a0,a1,a2,1
  d8:	82c5c50b          	lrbu	a0,a1,a2,1
  dc:	a2c5c50b          	lrhu	a0,a1,a2,1
  e0:	c2c5c50b          	lrwu	a0,a1,a2,1
  e4:	12c5c50b          	lurb	a0,a1,a2,1
  e8:	32c5c50b          	lurh	a0,a1,a2,1
  ec:	52c5c50b          	lurw	a0,a1,a2,1
  f0:	72c5c50b          	lurd	a0,a1,a2,1
  f4:	92c5c50b          	lurbu	a0,a1,a2,1
  f8:	b2c5c50b          	lurhu	a0,a1,a2,1
  fc:	d2c5c50b          	lurwu	a0,a1,a2,1
 100:	1af5c50b          	lbia	a0,\(a1\),15,1
 104:	0af5c50b          	lbib	a0,\(a1\),15,1
 108:	3af5c50b          	lhia	a0,\(a1\),15,1
 10c:	2af5c50b          	lhib	a0,\(a1\),15,1
 110:	5af5c50b          	lwia	a0,\(a1\),15,1
 114:	4af5c50b          	lwib	a0,\(a1\),15,1
 118:	7af5c50b          	ldia	a0,\(a1\),15,1
 11c:	6af5c50b          	ldib	a0,\(a1\),15,1
 120:	9af5c50b          	lbuia	a0,\(a1\),15,1
 124:	8af5c50b          	lbuib	a0,\(a1\),15,1
 128:	baf5c50b          	lhuia	a0,\(a1\),15,1
 12c:	aaf5c50b          	lhuib	a0,\(a1\),15,1
 130:	daf5c50b          	lwuia	a0,\(a1\),15,1
 134:	caf5c50b          	lwuib	a0,\(a1\),15,1
 138:	fab6450b          	ldd	a0,a1,\(a2\),1
 13c:	e2b6450b          	lwd	a0,a1,\(a2\),1
 140:	f2b6450b          	lwud	a0,a1,\(a2\),1
 144:	62b5700b          	fsrd	ft0,a0,a1,1
 148:	42b5700b          	fsrw	ft0,a0,a1,1
 14c:	72b5700b          	fsurd	ft0,a0,a1,1
 150:	52b5700b          	fsurw	ft0,a0,a1,1
 154:	02c5d50b          	srb	a0,a1,a2,1
 158:	22c5d50b          	srh	a0,a1,a2,1
 15c:	42c5d50b          	srw	a0,a1,a2,1
 160:	62c5d50b          	srd	a0,a1,a2,1
 164:	12c5d50b          	surb	a0,a1,a2,1
 168:	32c5d50b          	surh	a0,a1,a2,1
 16c:	52c5d50b          	surw	a0,a1,a2,1
 170:	72c5d50b          	surd	a0,a1,a2,1
 174:	1af5d50b          	sbia	a0,\(a1\),15,1
 178:	0af5d50b          	sbib	a0,\(a1\),15,1
 17c:	3af5d50b          	shia	a0,\(a1\),15,1
 180:	2af5d50b          	shib	a0,\(a1\),15,1
 184:	5ac5d50b          	swia	a0,\(a1\),12,1
 188:	4af5d50b          	swib	a0,\(a1\),15,1
 18c:	7af5d50b          	sdia	a0,\(a1\),15,1
 190:	6af5d50b          	sdib	a0,\(a1\),15,1
 194:	fab6550b          	sdd	a0,a1,\(a2\),1
 198:	e2b6550b          	swd	a0,a1,\(a2\),1
