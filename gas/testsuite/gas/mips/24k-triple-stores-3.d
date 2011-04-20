#objdump: -dr
#as: -mfix-24k
#name: 24K: Triple Store (Double-word Check)

.*: +file format .*mips.*

Disassembly of section .text:

00000000 <.text>:
   0:	a3a2000b 	sb	v0,11\(sp\)
   4:	00000000 	nop
   8:	a3a3000b 	sb	v1,11\(sp\)
   c:	a3a40004 	sb	a0,4\(sp\)
  10:	0000000d 	break
  14:	a3a20000 	sb	v0,0\(sp\)
  18:	a3a3000b 	sb	v1,11\(sp\)
  1c:	a3a40005 	sb	a0,5\(sp\)
  20:	0000000d 	break
  24:	a3a20007 	sb	v0,7\(sp\)
  28:	a3a3000b 	sb	v1,11\(sp\)
  2c:	00000000 	nop
  30:	a3a40010 	sb	a0,16\(sp\)
  34:	0000000d 	break
  38:	a1020000 	sb	v0,0\(t0\)
  3c:	a1030008 	sb	v1,8\(t0\)
  40:	00000000 	nop
  44:	a1040009 	sb	a0,9\(t0\)
  48:	0000000d 	break
  4c:	a7a20000 	sh	v0,0\(sp\)
  50:	a7a3ffe1 	sh	v1,-31\(sp\)
  54:	a7a4ffe2 	sh	a0,-30\(sp\)
  58:	0000000d 	break
  5c:	a7a20006 	sh	v0,6\(sp\)
  60:	a7a30008 	sh	v1,8\(sp\)
  64:	00000000 	nop
  68:	a7a40010 	sh	a0,16\(sp\)
  6c:	0000000d 	break
  70:	a5020001 	sh	v0,1\(t0\)
  74:	a5030003 	sh	v1,3\(t0\)
  78:	00000000 	nop
  7c:	a504000b 	sh	a0,11\(t0\)
  80:	0000000d 	break
  84:	afa20008 	sw	v0,8\(sp\)
  88:	afa3fff8 	sw	v1,-8\(sp\)
  8c:	afa40008 	sw	a0,8\(sp\)
  90:	0000000d 	break
  94:	afa20004 	sw	v0,4\(sp\)
  98:	afa30008 	sw	v1,8\(sp\)
  9c:	00000000 	nop
  a0:	afa40010 	sw	a0,16\(sp\)
  a4:	0000000d 	break
  a8:	ad020003 	sw	v0,3\(t0\)
  ac:	ad030007 	sw	v1,7\(t0\)
  b0:	00000000 	nop
  b4:	ad04000f 	sw	a0,15\(t0\)
  b8:	0000000d 	break
  bc:	aba20004 	swl	v0,4\(sp\)
  c0:	aba3000a 	swl	v1,10\(sp\)
  c4:	00000000 	nop
  c8:	aba40011 	swl	a0,17\(sp\)
  cc:	0000000d 	break
  d0:	aba20007 	swl	v0,7\(sp\)
  d4:	aba3000c 	swl	v1,12\(sp\)
  d8:	00000000 	nop
  dc:	aba40010 	swl	a0,16\(sp\)
  e0:	0000000d 	break
  e4:	aba20000 	swl	v0,0\(sp\)
  e8:	aba3000c 	swl	v1,12\(sp\)
  ec:	00000000 	nop
  f0:	aba40017 	swl	a0,23\(sp\)
  f4:	0000000d 	break
  f8:	a9020003 	swl	v0,3\(t0\)
  fc:	a9030008 	swl	v1,8\(t0\)
 100:	00000000 	nop
 104:	a904000c 	swl	a0,12\(t0\)
 108:	0000000d 	break
 10c:	aba20000 	swl	v0,0\(sp\)
 110:	aba3000c 	swl	v1,12\(sp\)
 114:	00000000 	nop
 118:	bba40017 	swr	a0,23\(sp\)
 11c:	0000000d 	break
 120:	a9020005 	swl	v0,5\(t0\)
 124:	a9030011 	swl	v1,17\(t0\)
 128:	00000000 	nop
 12c:	b904001c 	swr	a0,28\(t0\)
 130:	0000000d 	break
	\.\.\.
