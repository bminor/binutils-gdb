#source: ovl2.s
#ld: -N -T ovl2.lnk -T ovl.lnk --emit-relocs
#objdump: -D -r

.*elf32-spu

Disassembly of section \.text:

00000100 <_start>:
.*	brsl	\$0,.* <00000000\.ovl_call\.f1_a1>.*
.*SPU_REL16	f1_a1
.*	brsl	\$0,.* <00000000\.ovl_call\.setjmp>.*
.*SPU_REL16	setjmp
.*	br	100 <_start>	# 100
.*SPU_REL16	_start

0000010c <setjmp>:
.*	bi	\$0

00000110 <longjmp>:
.*	bi	\$0
.*

#00000118 <00000000\.ovl_call.f1_a1>:
#.*	brsl	\$75,.* <__ovly_load>.*
#.*00 04 04 00.*
#
#00000120 <00000000\.ovl_call.setjmp>:
#.*	brsl	\$75,.* <__ovly_load>.*
#.*00 00 01 0c.*
#
#00000128 <_SPUEAR_f1_a2>:
#.*	brsl	\$75,.* <__ovly_load>.*
#.*00 08 04 00.*

00000120 <00000000\.ovl_call.f1_a1>:
.*	ila	\$78,1
.*	lnop
.*	ila	\$79,1024	# 400
.*	br	.* <__ovly_load>.*

00000130 <00000000\.ovl_call.setjmp>:
.*	ila	\$78,0
.*	lnop
.*	ila	\$79,268	# 10c
.*	br	.* <__ovly_load>.*

00000140 <_SPUEAR_f1_a2>:
.*	ila	\$78,2
.*	lnop
.*	ila	\$79,1024	# 400
.*	br	.* <__ovly_load>.*

#...
Disassembly of section \.ov_a1:

00000400 <f1_a1>:
 400:	35 00 00 00 	bi	\$0
	\.\.\.
Disassembly of section \.ov_a2:

00000400 <f1_a2>:
 400:	32 7f a2 00 	br	110 <longjmp>	# 110
			400: SPU_REL16	longjmp
	\.\.\.
Disassembly of section \.data:

00000410 <_ovly_table-0x10>:
 410:	00 00 00 00 .*
 414:	00 00 00 01 .*
	\.\.\.
00000420 <_ovly_table>:
 420:	00 00 04 00 .*
 424:	00 00 00 10 .*
# 428:	00 00 03 10 .*
 428:	00 00 03 30 .*
 42c:	00 00 00 01 .*
 430:	00 00 04 00 .*
 434:	00 00 00 10 .*
# 438:	00 00 03 20 .*
 438:	00 00 03 40 .*
 43c:	00 00 00 01 .*

00000440 <_ovly_buf_table>:
 440:	00 00 00 00 .*

Disassembly of section \.toe:

00000450 <_EAR_>:
	\.\.\.
Disassembly of section \.note\.spu_name:

.* <\.note\.spu_name>:
.*:	00 00 00 08 .*
.*:	00 00 00 0c .*
.*:	00 00 00 01 .*
.*:	53 50 55 4e .*
.*:	41 4d 45 00 .*
.*:	74 6d 70 64 .*
.*:	69 72 2f 64 .*
.*:	75 6d 70 00 .*
