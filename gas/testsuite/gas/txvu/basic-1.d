#as:
#objdump: -dr
#name: basic-1

.*: +file format .*

Disassembly of section .text:

0* <foo>:
 *0:	fd a1 ca 01 	abs.xyz vf10,vf20 || waitp
 *4:	bf 07 00 80 
 *8:	a8 a2 fe 01 	add.xyzw vf10,vf20,vf30 || waitq
 *c:	bf 03 00 80 
