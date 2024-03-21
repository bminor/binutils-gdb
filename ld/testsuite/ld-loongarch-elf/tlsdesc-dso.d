#as:
#ld: -shared -z norelro --hash-style=both
#objdump: -dr
#skip: loongarch32-*-*

.*:     file format .*


Disassembly of section .text:

[0-9a-f]+ <fun_gl1>:
 +[0-9a-f]+:	18021584 	pcaddi      	\$a0, 4268
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28dd4084 	ld.d        	\$a0, \$a0, 1872
 +[0-9a-f]+:	18021364 	pcaddi      	\$a0, 4251
 +[0-9a-f]+:	180213c4 	pcaddi      	\$a0, 4254
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28dc0084 	ld.d        	\$a0, \$a0, 1792
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28dc0084 	ld.d        	\$a0, \$a0, 1792
 +[0-9a-f]+:	18021364 	pcaddi      	\$a0, 4251
 +[0-9a-f]+:	180213c4 	pcaddi      	\$a0, 4254
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28dce084 	ld.d        	\$a0, \$a0, 1848

[0-9a-f]+ <fun_lo>:
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28daa084 	ld.d        	\$a0, \$a0, 1704
 +[0-9a-f]+:	18020de4 	pcaddi      	\$a0, 4207
 +[0-9a-f]+:	18020f04 	pcaddi      	\$a0, 4216
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0
 +[0-9a-f]+:	18020e24 	pcaddi      	\$a0, 4209
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28db4084 	ld.d        	\$a0, \$a0, 1744
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28db4084 	ld.d        	\$a0, \$a0, 1744
 +[0-9a-f]+:	18020f44 	pcaddi      	\$a0, 4218
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0
 +[0-9a-f]+:	18020e64 	pcaddi      	\$a0, 4211
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, 4
 +[0-9a-f]+:	28dba084 	ld.d        	\$a0, \$a0, 1768

[0-9a-f]+ <fun_external>:
 +[0-9a-f]+:	18020ec4 	pcaddi      	\$a0, 4214
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0

[0-9a-f]+ <fun_hidden>:
 +[0-9a-f]+:	18021224 	pcaddi      	\$a0, 4241
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0
 +[0-9a-f]+:	18021144 	pcaddi      	\$a0, 4234
 +[0-9a-f]+:	28c00081 	ld.d        	\$ra, \$a0, 0
 +[0-9a-f]+:	4c000021 	jirl        	\$ra, \$ra, 0
