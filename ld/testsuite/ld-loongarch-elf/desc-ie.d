#as:
#ld: -shared -z norelro --hash-style=both
#objdump: -dr
#skip: loongarch32-*-*

.*:     file format .*

Disassembly of section .text:

[0-9a-f]+ <fn1>:
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, .*
 +[0-9a-f]+:	28cd0084 	ld.d        	\$a0, \$a0, .*
 +[0-9a-f]+:	1a000084 	pcalau12i   	\$a0, .*
 +[0-9a-f]+:	28cd0084 	ld.d        	\$a0, \$a0, .*
