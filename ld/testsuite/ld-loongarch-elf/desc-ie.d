#as:
#ld: -shared -z norelro -e 0x0 --hash-style=both
#objdump: -dr
#skip: loongarch32-*-*

.*:     file format .*

Disassembly of section .text:

0+230 <fn1>:
 230:	1a000084 	pcalau12i   	\$a0, 4
 234:	28cd6084 	ld.d        	\$a0, \$a0, 856
 238:	03400000 	nop.*
 23c:	03400000 	nop.*
 240:	1a000084 	pcalau12i   	\$a0, 4
 244:	28cd6081 	ld.d        	\$ra, \$a0, 856
