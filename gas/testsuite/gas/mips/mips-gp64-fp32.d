#objdump: -d -mmips:8000
#as: -march=8000 -EB -mfp32
#name: MIPS -mgp64 -mfp32

.*: +file format.*

Disassembly of section .text:

0+000 <[^>]*>:
   0:	3c041234 	lui	a0,0x1234
   4:	34845678 	ori	a0,a0,0x5678
   8:	2784c000 	addiu	a0,gp,-16384
   c:	3c040000 	lui	a0,0x0
  10:	24840000 	addiu	a0,a0,0
  14:	3c040000 	lui	a0,0x0
  18:	24840154 	addiu	a0,a0,340
  1c:	08000055 	j	154 <[^>]*>
  20:	0c000055 	jal	154 <[^>]*>
  24:	8f84c000 	lw	a0,-16384\(gp\)
  28:	3c040000 	lui	a0,0x0
  2c:	8c840000 	lw	a0,0\(a0\)
  30:	3c040000 	lui	a0,0x0
  34:	8c840154 	lw	a0,340\(a0\)
  38:	df84c000 	ld	a0,-16384\(gp\)
  3c:	3c040000 	lui	a0,0x0
  40:	dc840000 	ld	a0,0\(a0\)
  44:	3c040000 	lui	a0,0x0
  48:	dc840154 	ld	a0,340\(a0\)
  4c:	af84c000 	sw	a0,-16384\(gp\)
  50:	3c010000 	lui	at,0x0
  54:	ac240000 	sw	a0,0\(at\)
  58:	ff84c000 	sd	a0,-16384\(gp\)
  5c:	3c010000 	lui	at,0x0
  60:	fc240000 	sd	a0,0\(at\)
  64:	3c010000 	lui	at,0x0
  68:	24210000 	addiu	at,at,0
  6c:	80240000 	lb	a0,0\(at\)
  70:	90210001 	lbu	at,1\(at\)
  74:	00042200 	sll	a0,a0,0x8
  78:	00812025 	or	a0,a0,at
  7c:	3c010000 	lui	at,0x0
  80:	24210000 	addiu	at,at,0
  84:	a0240001 	sb	a0,1\(at\)
  88:	00042202 	srl	a0,a0,0x8
  8c:	a0240000 	sb	a0,0\(at\)
  90:	90210001 	lbu	at,1\(at\)
  94:	00042200 	sll	a0,a0,0x8
  98:	00812025 	or	a0,a0,at
  9c:	3c010000 	lui	at,0x0
  a0:	24210000 	addiu	at,at,0
  a4:	88240000 	lwl	a0,0\(at\)
  a8:	98240003 	lwr	a0,3\(at\)
  ac:	3c010000 	lui	at,0x0
  b0:	24210000 	addiu	at,at,0
  b4:	a8240000 	swl	a0,0\(at\)
  b8:	b8240003 	swr	a0,3\(at\)
  bc:	3404ffc0 	li	a0,0xffc0
  c0:	000423bc 	dsll32	a0,a0,0xe
  c4:	3c010000 	lui	at,0x0
  c8:	dc240000 	ld	a0,0\(at\)
  cc:	3c013ff0 	lui	at,0x3ff0
  d0:	44810800 	mtc1	at,\$f1
  d4:	44800000 	mtc1	zero,\$f0
  d8:	d780c000 	ldc1	\$f0,-16384\(gp\)
  dc:	64a40064 	daddiu	a0,a1,100
  e0:	2c840001 	sltiu	a0,a0,1
  e4:	64a40064 	daddiu	a0,a1,100
  e8:	0004202b 	sltu	a0,zero,a0
  ec:	00a0202d 	move	a0,a1
  f0:	2784c000 	addiu	a0,gp,-16384
  f4:	3c040000 	lui	a0,0x0
  f8:	24840000 	addiu	a0,a0,0
  fc:	3c010000 	lui	at,0x0
 100:	24210000 	addiu	at,at,0
 104:	68240000 	ldl	a0,0\(at\)
 108:	6c240007 	ldr	a0,7\(at\)
 10c:	3c010000 	lui	at,0x0
 110:	24210000 	addiu	at,at,0
 114:	b0240000 	sdl	a0,0\(at\)
 118:	b4240007 	sdr	a0,7\(at\)
 11c:	3c018000 	lui	at,0x8000
 120:	0081082a 	slt	at,a0,at
 124:	1020000b 	beqz	at,154 <[^>]*>
 128:	34018000 	li	at,0x8000
 12c:	00010c78 	dsll	at,at,0x11
 130:	0081082b 	sltu	at,a0,at
 134:	10200007 	beqz	at,154 <[^>]*>
 138:	3c018000 	lui	at,0x8000
 13c:	0081082a 	slt	at,a0,at
 140:	14200004 	bnez	at,154 <[^>]*>
 144:	34018000 	li	at,0x8000
 148:	00010c78 	dsll	at,at,0x11
 14c:	0081082b 	sltu	at,a0,at
 150:	14200000 	bnez	at,154 <[^>]*>

0+0154 <[^>]*>:
	...
