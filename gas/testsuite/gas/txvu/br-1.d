#as:
#objdump: -dr
#name: br-1

.*: +file format .*

Disassembly of section .text:

0* <foo>:
 *0:	02 00 00 40[ 	]*nop[ 	]*b 10 <foo1>
 *4:	ff 02 00 00[ 	]*
 *8:	3c 03 00 80[ 	]*nop[ 	]*nop
 *c:	ff 02 00 00[ 	]*

0*10 <foo1>:
  10:	3c 03 00 80[ 	]*nop[ 	]*nop
  14:	ff 02 00 00[ 	]*
