#objdump: -dr
#as: -mfix-24k
#name: 24K: Triple Store (Store Macro Check)

.*: +file format .*mips.*

Disassembly of section .text:
00000000 <.text>:
   0:	abbf0050 	swl	ra,80\(sp\)
   4:	00000000 	nop
   8:	bbbf0053 	swr	ra,83\(sp\)
   c:	abb30058 	swl	s3,88\(sp\)
  10:	bbb3005b 	swr	s3,91\(sp\)
  14:	abbe0060 	swl	s8,96\(sp\)
  18:	bbbe0063 	swr	s8,99\(sp\)
  1c:	0000000d 	break
  20:	a3bf0051 	sb	ra,81\(sp\)
  24:	001f0a02 	srl	at,ra,0x8
  28:	a3a10050 	sb	at,80\(sp\)
  2c:	a3b30059 	sb	s3,89\(sp\)
  30:	00130a02 	srl	at,s3,0x8
  34:	a3a10058 	sb	at,88\(sp\)
  38:	a3be0061 	sb	s8,97\(sp\)
  3c:	001e0a02 	srl	at,s8,0x8
  40:	a3a10060 	sb	at,96\(sp\)
  44:	0000000d 	break
  48:	e7a00050 	swc1	\$f0,80\(sp\)
  4c:	e7a20058 	swc1	\$f2,88\(sp\)
  50:	00000000 	nop
  54:	e7a40060 	swc1	\$f4,96\(sp\)
  58:	0000000d 	break
  5c:	f7a00050 	sdc1	\$f0,80\(sp\)
  60:	f7a20058 	sdc1	\$f2,88\(sp\)
  64:	00000000 	nop
  68:	f7a40060 	sdc1	\$f4,96\(sp\)
  6c:	0000000d 	break
	\.\.\.
