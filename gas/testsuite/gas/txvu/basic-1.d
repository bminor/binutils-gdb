#as:
#objdump: -dr
#name: basic-1

.*: +file format .*

Disassembly of section .text:

0* <foo>:
 *0:	bf 07 00 80[ 	]*abs.xyz vf10xyz,vf20xyz[ 	]*waitp
 *4:	fd a1 ca 01[ 	]*
 *8:	bf 03 00 80[ 	]*add.xyzw vf10xyzw,vf20xyzw,vf30xyzw[ 	]*waitq
 *c:	a8 a2 fe 01[ 	]*
