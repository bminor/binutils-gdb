#source: icache1.s
#ld: --soft-icache --num-lines=4 --non-ia-text --auto-overlay=tmpdir/icache1.lnk --auto-relink
#objdump: -D

.* elf32-spu


Disassembly of section .ovl.init:
00000800 <__icache_fileoff>:
.*	00 00 00 00.*
.*	00 00 07 00.*
	\.\.\.

Disassembly of section \.ovly1:

00000800 <\.ovly1>:
.*	ai	\$1,\$1,64	# 40
.*	lqd	\$0,16\(\$1\)
.*	bi	\$0
	\.\.\.

Disassembly of section \.ovly2:

00000c00 <f1>:
.*	40 20 00 00 	nop	\$0
.*	24 00 40 80 	stqd	\$0,16\(\$1\)
.*	1c f0 00 81 	ai	\$1,\$1,-64
.*	24 00 00 81 	stqd	\$1,0\(\$1\)
.*	33 00 73 80 	brsl	\$0,fac .*
.*	33 00 77 00 	brsl	\$0,fcc .*
	\.\.\.
.*	32 00 16 80 	br	fec .*
	\.\.\.
     fa0:	00 00 00 02.*
     fa4:	00 04 11 04.*
     fa8:	a0 00 0c 10.*
     fac:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
     fb0:	00 00 ed 00.*
	\.\.\.
     fc0:	00 00 00 02.*
     fc4:	00 04 10 00.*
     fc8:	a0 00 0c 14.*
     fcc:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
     fd0:	00 00 00 00.*
     fd4:	00 00 0a 80.*
	\.\.\.
     fe4:	00 04 08 00.*
     fe8:	20 00 0f 38.*
     fec:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
	\.\.\.
     ff8:	00 7f 0f 80.*
     ffc:	00 00 00 00.*

Disassembly of section \.ovly3:

00001000 <f3>:
	\.\.\.
.*	35 00 00 00 	bi	\$0

00001104 <f2>:
.*	1c e0 00 81 	ai	\$1,\$1,-128
.*	24 00 00 81 	stqd	\$1,0\(\$1\)
	\.\.\.
.*	1c 20 00 81 	ai	\$1,\$1,128	# 80
.*	35 00 00 00 	bi	\$0
	\.\.\.

Disassembly of section \.ovly4:

00001400 <f5>:
.*	24 00 40 80 	stqd	\$0,16\(\$1\)
.*	24 f8 00 81 	stqd	\$1,-512\(\$1\)
.*	1c 80 00 81 	ai	\$1,\$1,-512
.*	33 7f fe 80 	brsl	\$0,1400 <f5>	# 1400
	\.\.\.
.*	42 01 00 03 	ila	\$3,200.*
.*	18 00 c0 81 	a	\$1,\$1,\$3
.*	34 00 40 80 	lqd	\$0,16\(\$1\)
.*	35 00 00 00 	bi	\$0
	\.\.\.

Disassembly of section \.ovly5:

00000800 <\.ovly5>:
	\.\.\.
.*	42 01 00 03 	ila	\$3,200 .*
.*	18 00 c0 81 	a	\$1,\$1,\$3
.*	34 00 40 80 	lqd	\$0,16\(\$1\)
.*	30 01 7d 80 	bra	bec .*
	\.\.\.
 be0:	00 00 00 03.*
 be4:	00 04 14 00.*
 be8:	a0 00 0b 2c.*
 bec:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
	\.\.\.
 bfc:	00 03 fd 80.*

Disassembly of section \.ovly6:

00000c00 <\.ovly6>:
.*	31 01 f5 80 	brasl	\$0,fac .*
.*	33 00 79 00 	brsl	\$0,fcc .*
	\.\.\.
.*	32 00 18 80 	br	fec .*
	\.\.\.
     fa0:	00 00 00 07.*
     fa4:	00 08 14 00.*
     fa8:	a0 00 0c 00.*
     fac:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
     fb0:	00 03 75 80.*
	\.\.\.
     fc0:	00 00 00 07.*
     fc4:	00 08 14 00.*
     fc8:	a0 00 0c 04.*
     fcc:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
     fd0:	00 00 00 00.*
     fd4:	00 00 86 80.*
	\.\.\.
     fe0:	00 00 00 04.*
     fe4:	00 08 08 00.*
     fe8:	20 00 0f 28.*
     fec:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
	\.\.\.
     ff8:	00 7f 03 80.*
     ffc:	00 00 00 00.*

Disassembly of section \.ovly7:

00001000 <\.ovly7>:
.*	41 7f ff 83 	ilhu	\$3,65535	# ffff
.*	60 f8 30 03 	iohl	\$3,61536	# f060
.*	18 00 c0 84 	a	\$4,\$1,\$3
.*	00 20 00 00 	lnop
.*	04 00 02 01 	ori	\$1,\$4,0
.*	24 00 02 04 	stqd	\$4,0\(\$4\)
.*	33 00 72 80 	brsl	\$0,13ac .*
.*	33 00 76 00 	brsl	\$0,13cc .*
.*	34 00 00 81 	lqd	\$1,0\(\$1\)
	\.\.\.
.*	32 00 15 00 	br	13ec .*
	\.\.\.
    13a0:	00 00 00 03.*
    13a4:	00 04 14 00.*
    13a8:	a0 00 10 18.*
    13ac:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
	\.\.\.
    13b8:	00 00 0f 80.*
    13bc:	00 00 00 00.*
    13c0:	00 00 00 07.*
    13c4:	00 08 14 00.*
    13c8:	a0 00 10 1c.*
    13cc:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
	\.\.\.
    13dc:	00 00 0a 80.*
    13e0:	00 00 00 05.*
    13e4:	00 08 0c 00.*
    13e8:	20 00 13 44.*
    13ec:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
    13f0:	00 00 00 00.*
    13f4:	00 7f 02 80.*
	\.\.\.

Disassembly of section \.ovly8:

00001400 <f4>:
.*	24 00 40 80 	stqd	\$0,16\(\$1\)
.*	24 f8 00 81 	stqd	\$1,-512\(\$1\)
.*	1c 80 00 81 	ai	\$1,\$1,-512
.*	31 02 f9 80 	brasl	\$0,17cc .*
	\.\.\.
.*	32 00 17 80 	br	17ec .*
	\.\.\.
    17c0:	00 00 00 02.*
    17c4:	00 04 11 04.*
    17c8:	a0 00 14 0c.*
    17cc:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
	\.\.\.
    17dc:	00 00 d9 00.*
    17e0:	00 00 00 06.*
    17e4:	00 08 10 00.*
    17e8:	20 00 17 30.*
    17ec:	31 03 01 cb 	brasl	\$75,180c <__icache_br_handler>
    17f0:	00 7f 0d 80.*
	\.\.\.

Disassembly of section \.text:

00001800 <_start>:
.*	41 00 02 03 	ilhu	\$3,4
.*	60 8a 00 03 	iohl	\$3,5120	# 1400
.*	32 00 04 80 	br	182c.*
0000180c <__icache_br_handler>:
    180c:	00 00 00 00 	stop
00001810 <__icache_call_handler>:
	\.\.\.
    1820:	00 00 00 01.*
    1824:	00 04 0c 00.*
    1828:	a0 00 18 08.*
    182c:	31 03 02 4b 	brasl	\$75,1810 <__icache_call_handler>
	\.\.\.
    1838:	00 7e 7b 80.*
	\.\.\.

#pass
