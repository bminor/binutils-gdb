#objdump: -d -mmips:8000
#as: -march=8000 -EB -mfp32 -KPIC
#name: MIPS -mgp64 -mfp32 (SVR4 PIC)

.*: +file format.*

Disassembly of section .text:

0+000 <[^>]*>:
   0:	3c1c0000 	lui	gp,0x0
   4:	279c0000 	addiu	gp,gp,0
   8:	0399e021 	addu	gp,gp,t9
   c:	ffbc0008 	sd	gp,8\(sp\)
  10:	009c202d 	daddu	a0,a0,gp
  14:	3c041234 	lui	a0,0x1234
  18:	34845678 	ori	a0,a0,0x5678
  1c:	8f840000 	lw	a0,0\(gp\)
  20:	00000000 	nop
  24:	64840000 	daddiu	a0,a0,0
  28:	8f840000 	lw	a0,0\(gp\)
  2c:	00000000 	nop
  30:	64840000 	daddiu	a0,a0,0
  34:	8f840000 	lw	a0,0\(gp\)
  38:	00000000 	nop
  3c:	64840228 	daddiu	a0,a0,552
  40:	10000079 	b	228 <[^>]*>
  44:	00000000 	nop
  48:	df990000 	ld	t9,0\(gp\)
  4c:	00000000 	nop
  50:	67390228 	daddiu	t9,t9,552
  54:	0320f809 	jalr	t9
  58:	00000000 	nop
  5c:	dfbc0008 	ld	gp,8\(sp\)
  60:	df840000 	ld	a0,0\(gp\)
  64:	00000000 	nop
  68:	64840000 	daddiu	a0,a0,0
  6c:	8c840000 	lw	a0,0\(a0\)
  70:	df840000 	ld	a0,0\(gp\)
  74:	00000000 	nop
  78:	64840000 	daddiu	a0,a0,0
  7c:	8c840000 	lw	a0,0\(a0\)
  80:	df840000 	ld	a0,0\(gp\)
  84:	00000000 	nop
  88:	64840228 	daddiu	a0,a0,552
  8c:	8c840000 	lw	a0,0\(a0\)
  90:	df840000 	ld	a0,0\(gp\)
  94:	00000000 	nop
  98:	64840000 	daddiu	a0,a0,0
  9c:	dc840000 	ld	a0,0\(a0\)
  a0:	df840000 	ld	a0,0\(gp\)
  a4:	00000000 	nop
  a8:	64840000 	daddiu	a0,a0,0
  ac:	dc840000 	ld	a0,0\(a0\)
  b0:	df840000 	ld	a0,0\(gp\)
  b4:	00000000 	nop
  b8:	64840228 	daddiu	a0,a0,552
  bc:	dc840000 	ld	a0,0\(a0\)
  c0:	df810000 	ld	at,0\(gp\)
  c4:	00000000 	nop
  c8:	64210000 	daddiu	at,at,0
  cc:	ac240000 	sw	a0,0\(at\)
  d0:	df810000 	ld	at,0\(gp\)
  d4:	00000000 	nop
  d8:	64210000 	daddiu	at,at,0
  dc:	ac240000 	sw	a0,0\(at\)
  e0:	df810000 	ld	at,0\(gp\)
  e4:	00000000 	nop
  e8:	64210000 	daddiu	at,at,0
  ec:	fc240000 	sd	a0,0\(at\)
  f0:	df810000 	ld	at,0\(gp\)
  f4:	00000000 	nop
  f8:	64210000 	daddiu	at,at,0
  fc:	fc240000 	sd	a0,0\(at\)
 100:	df810000 	ld	at,0\(gp\)
 104:	00000000 	nop
 108:	64210000 	daddiu	at,at,0
 10c:	80240000 	lb	a0,0\(at\)
 110:	90210001 	lbu	at,1\(at\)
 114:	00042200 	sll	a0,a0,0x8
 118:	00812025 	or	a0,a0,at
 11c:	df810000 	ld	at,0\(gp\)
 120:	00000000 	nop
 124:	64210000 	daddiu	at,at,0
 128:	a0240001 	sb	a0,1\(at\)
 12c:	00042202 	srl	a0,a0,0x8
 130:	a0240000 	sb	a0,0\(at\)
 134:	90210001 	lbu	at,1\(at\)
 138:	00042200 	sll	a0,a0,0x8
 13c:	00812025 	or	a0,a0,at
 140:	df810000 	ld	at,0\(gp\)
 144:	00000000 	nop
 148:	64210000 	daddiu	at,at,0
 14c:	88240000 	lwl	a0,0\(at\)
 150:	98240003 	lwr	a0,3\(at\)
 154:	df810000 	ld	at,0\(gp\)
 158:	00000000 	nop
 15c:	64210000 	daddiu	at,at,0
 160:	a8240000 	swl	a0,0\(at\)
 164:	b8240003 	swr	a0,3\(at\)
 168:	3404ffc0 	li	a0,0xffc0
 16c:	000423bc 	dsll32	a0,a0,0xe
 170:	df810000 	ld	at,0\(gp\)
 174:	dc240000 	ld	a0,0\(at\)
 178:	3c013ff0 	lui	at,0x3ff0
 17c:	44810800 	mtc1	at,\$f1
 180:	44800000 	mtc1	zero,\$f0
 184:	df810000 	ld	at,0\(gp\)
 188:	d4200008 	ldc1	\$f0,8\(at\)
 18c:	64a40064 	daddiu	a0,a1,100
 190:	2c840001 	sltiu	a0,a0,1
 194:	64a40064 	daddiu	a0,a1,100
 198:	0004202b 	sltu	a0,zero,a0
 19c:	00a0202d 	move	a0,a1
 1a0:	df840000 	ld	a0,0\(gp\)
 1a4:	00000000 	nop
 1a8:	64840000 	daddiu	a0,a0,0
 1ac:	df840000 	ld	a0,0\(gp\)
 1b0:	00000000 	nop
 1b4:	64840000 	daddiu	a0,a0,0
 1b8:	df810000 	ld	at,0\(gp\)
 1bc:	00000000 	nop
 1c0:	64210000 	daddiu	at,at,0
 1c4:	68240000 	ldl	a0,0\(at\)
 1c8:	6c240007 	ldr	a0,7\(at\)
 1cc:	df810000 	ld	at,0\(gp\)
 1d0:	00000000 	nop
 1d4:	64210000 	daddiu	at,at,0
 1d8:	b0240000 	sdl	a0,0\(at\)
 1dc:	b4240007 	sdr	a0,7\(at\)
 1e0:	3c018000 	lui	at,0x8000
 1e4:	0081082a 	slt	at,a0,at
 1e8:	1020000f 	beqz	at,228 <[^>]*>
 1ec:	00000000 	nop
 1f0:	34018000 	li	at,0x8000
 1f4:	00010c78 	dsll	at,at,0x11
 1f8:	0081082b 	sltu	at,a0,at
 1fc:	1020000a 	beqz	at,228 <[^>]*>
 200:	00000000 	nop
 204:	3c018000 	lui	at,0x8000
 208:	0081082a 	slt	at,a0,at
 20c:	14200006 	bnez	at,228 <[^>]*>
 210:	00000000 	nop
 214:	34018000 	li	at,0x8000
 218:	00010c78 	dsll	at,at,0x11
 21c:	0081082b 	sltu	at,a0,at
 220:	14200001 	bnez	at,228 <[^>]*>
 224:	00000000 	nop
