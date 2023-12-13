#as:
#ld: -z norelro -e 0x0
#objdump: -dr
#skip: loongarch32-*-*

.*:     file format .*


Disassembly of section .text:

0+1200000e8 <fn1>:
   1200000e8:	14000004 	lu12i.w     	\$a0, 0
   1200000ec:	03800084 	ori         	\$a0, \$a0, 0x0
   1200000f0:	03400000 	nop.*
   1200000f4:	03400000 	nop.*
