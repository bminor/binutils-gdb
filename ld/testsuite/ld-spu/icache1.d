#source: icache1.s
#ld: --soft-icache --num-lines=4 --non-ia-text --auto-overlay=tmpdir/icache1.lnk --auto-relink
#objdump: -D

.* elf32-spu


Disassembly of section .ovl.init:
00000000 <__icache_fileoff>:
.*	00 00 00 00.*
.*	00 00 02 00.*
	\.\.\.

Disassembly of section \.ovly1:

00000000 <\.ovly1>:
.*	ai	\$1,\$1,64	# 40
.*	lqd	\$0,16\(\$1\)
.*	bi	\$0
	\.\.\.

Disassembly of section \.ovly2:

00000400 <f1>:
.*	40 20 00 00 	nop	\$0
.*	24 00 40 80 	stqd	\$0,16\(\$1\)
.*	1c f0 00 81 	ai	\$1,\$1,-64
.*	24 00 00 81 	stqd	\$1,0\(\$1\)
.*	33 00 78 80 	brsl	\$0,7d4 .*
.*	33 00 7a 00 	brsl	\$0,7e4 .*
	\.\.\.
.*	32 00 17 80 	br	7f4 .*
	\.\.\.
 7d0:	00 04 09 04.*
 7d4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 7d8:	a0 00 04 10.*
 7dc:	00 00 e6 00.*
 7e0:	00 04 08 00.*
 7e4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 7e8:	a0 00 04 14.*
 7ec:	00 00 07 80.*
 7f0:	00 04 00 00.*
 7f4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 7f8:	20 00 07 38.*
 7fc:	00 7f 0e 80.*

Disassembly of section \.ovly3:

00000800 <f3>:
	\.\.\.
.*	35 00 00 00 	bi	\$0

00000904 <f2>:
.*	1c e0 00 81 	ai	\$1,\$1,-128
.*	24 00 00 81 	stqd	\$1,0\(\$1\)
	\.\.\.
.*	1c 20 00 81 	ai	\$1,\$1,128	# 80
.*	35 00 00 00 	bi	\$0
	\.\.\.

Disassembly of section \.ovly4:

00000c00 <f5>:
.*	24 00 40 80 	stqd	\$0,16\(\$1\)
.*	24 f8 00 81 	stqd	\$1,-512\(\$1\)
.*	1c 80 00 81 	ai	\$1,\$1,-512
.*	33 7f fe 80 	brsl	\$0,c00 <f5>	# c00
	\.\.\.
.*	42 01 00 03 	ila	\$3,200.*
.*	18 00 c0 81 	a	\$1,\$1,\$3
.*	34 00 40 80 	lqd	\$0,16\(\$1\)
.*	35 00 00 00 	bi	\$0
	\.\.\.

Disassembly of section \.ovly5:

00000000 <\.ovly5>:
	\.\.\.
.*	42 01 00 03 	ila	\$3,200 .*
.*	18 00 c0 81 	a	\$1,\$1,\$3
.*	34 00 40 80 	lqd	\$0,16\(\$1\)
.*	30 00 7e 80 	bra	3f4 .*
	\.\.\.
 3f0:	00 04 0c 00.*
 3f4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 3f8:	a0 00 03 2c.*
 3fc:	00 01 fe 80.*

Disassembly of section \.ovly6:

00000400 <\.ovly6>:
.*	31 00 fa 80 	brasl	\$0,7d4 .*
.*	33 00 7c 00 	brsl	\$0,7e4 .*
	\.\.\.
.*	32 00 19 80 	br	7f4 .*
	\.\.\.
 7d0:	00 08 0c 00.*
 7d4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 7d8:	a0 00 04 00.*
 7dc:	00 01 7a 80.*
 7e0:	00 08 0c 00.*
 7e4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 7e8:	a0 00 04 04.*
 7ec:	00 00 83 80.*
 7f0:	00 08 00 00.*
 7f4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 7f8:	20 00 07 28.*
 7fc:	00 7f 02 80.*

Disassembly of section \.ovly7:

00000800 <\.ovly7>:
.*	41 7f ff 83 	ilhu	\$3,65535	# ffff
.*	60 f8 30 03 	iohl	\$3,61536	# f060
.*	18 00 c0 84 	a	\$4,\$1,\$3
.*	00 20 00 00 	lnop
.*	04 00 02 01 	ori	\$1,\$4,0
.*	24 00 02 04 	stqd	\$4,0\(\$4\)
.*	33 00 77 80 	brsl	\$0,bd4 .*
.*	33 00 79 00 	brsl	\$0,be4 .*
.*	34 00 00 81 	lqd	\$1,0\(\$1\)
	\.\.\.
.*	32 00 16 00 	br	bf4 .*
	\.\.\.
 bd0:	00 04 0c 00.*
 bd4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 bd8:	a0 00 08 18.*
 bdc:	00 00 0a 80.*
 be0:	00 08 0c 00.*
 be4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 be8:	a0 00 08 1c.*
 bec:	00 00 05 80.*
 bf0:	00 08 04 00.*
 bf4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
 bf8:	20 00 0b 44.*
 bfc:	00 7f 01 80.*

Disassembly of section \.ovly8:

00000c00 <f4>:
.*	24 00 40 80 	stqd	\$0,16\(\$1\)
.*	24 f8 00 81 	stqd	\$1,-512\(\$1\)
.*	1c 80 00 81 	ai	\$1,\$1,-512
.*	31 01 fc 80 	brasl	\$0,fe4 .*
	\.\.\.
.*	32 00 18 80 	br	ff4 .*
	\.\.\.
     fe0:	00 04 09 04.*
     fe4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
     fe8:	a0 00 0c 0c.*
     fec:	00 00 dc 00.*
     ff0:	00 08 08 00.*
     ff4:	31 02 01 cb 	brasl	\$75,100c <__icache_br_handler>
     ff8:	20 00 0f 30.*
     ffc:	00 7f 02 80.*

Disassembly of section \.text:

00001000 <_start>:
.*	41 00 02 03 	ilhu	\$3,4
.*	60 86 00 03 	iohl	\$3,3072	# c00
.*	32 00 03 80 	br	1024.*
0000100c <__icache_br_handler>:
    100c:	00 00 00 00 	stop
00001010 <__icache_call_handler>:
	\.\.\.
    1020:	00 04 04 00.*
    1024:	31 02 02 4b 	brasl	\$75,1010 <__icache_call_handler>
    1028:	a0 00 10 08.*
    102c:	00 7e 7c 80.*

#pass
