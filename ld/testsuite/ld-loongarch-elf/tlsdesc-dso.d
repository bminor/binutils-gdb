#as:
#ld: -shared -z norelro --hash-style=both
#objdump: -dr
#skip: loongarch32-*-*

.*:     file format .*


Disassembly of section .text:

0+448 <fun_gl1>:
 448:	18021584 	pcaddi      	\$a0, 4268
 44c:	1a000084 	pcalau12i   	\$a0, 4
 450:	28dc2084 	ld.d        	\$a0, \$a0, 1800
 454:	18021364 	pcaddi      	\$a0, 4251
 458:	180213c4 	pcaddi      	\$a0, 4254
 45c:	28c00081 	ld.d        	\$ra, \$a0, 0
 460:	4c000021 	jirl        	\$ra, \$ra, 0
 464:	1a000084 	pcalau12i   	\$a0, 4
 468:	28dae084 	ld.d        	\$a0, \$a0, 1720
 46c:	1a000084 	pcalau12i   	\$a0, 4
 470:	28dae084 	ld.d        	\$a0, \$a0, 1720
 474:	18021364 	pcaddi      	\$a0, 4251
 478:	18021344 	pcaddi      	\$a0, 4250
 47c:	28c00081 	ld.d        	\$ra, \$a0, 0
 480:	4c000021 	jirl        	\$ra, \$ra, 0
 484:	1a000084 	pcalau12i   	\$a0, 4
 488:	28dbc084 	ld.d        	\$a0, \$a0, 1776

0+48c <fun_lo>:
 48c:	1a000084 	pcalau12i   	\$a0, 4
 490:	28d98084 	ld.d        	\$a0, \$a0, 1632
 494:	18020de4 	pcaddi      	\$a0, 4207
 498:	18020f04 	pcaddi      	\$a0, 4216
 49c:	28c00081 	ld.d        	\$ra, \$a0, 0
 4a0:	4c000021 	jirl        	\$ra, \$ra, 0
 4a4:	18020e24 	pcaddi      	\$a0, 4209
 4a8:	1a000084 	pcalau12i   	\$a0, 4
 4ac:	28da2084 	ld.d        	\$a0, \$a0, 1672
 4b0:	1a000084 	pcalau12i   	\$a0, 4
 4b4:	28da2084 	ld.d        	\$a0, \$a0, 1672
 4b8:	18020ec4 	pcaddi      	\$a0, 4214
 4bc:	28c00081 	ld.d        	\$ra, \$a0, 0
 4c0:	4c000021 	jirl        	\$ra, \$ra, 0
 4c4:	18020e64 	pcaddi      	\$a0, 4211
 4c8:	1a000084 	pcalau12i   	\$a0, 4
 4cc:	28da8084 	ld.d        	\$a0, \$a0, 1696

0+4d0 <fun_external>:
 4d0:	18020ec4 	pcaddi      	\$a0, 4214
 4d4:	28c00081 	ld.d        	\$ra, \$a0, 0
 4d8:	4c000021 	jirl        	\$ra, \$ra, 0

0+4dc <fun_hidden>:
 4dc:	18021224 	pcaddi      	\$a0, 4241
 4e0:	28c00081 	ld.d        	\$ra, \$a0, 0
 4e4:	4c000021 	jirl        	\$ra, \$ra, 0
 4e8:	18021144 	pcaddi      	\$a0, 4234
 4ec:	28c00081 	ld.d        	\$ra, \$a0, 0
 4f0:	4c000021 	jirl        	\$ra, \$ra, 0
