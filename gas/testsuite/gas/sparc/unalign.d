#as: 
#objdump: -Dr
#name: sparc unaligned relocs

.*: +file format .*sparc.*

Disassembly of section .text:
Disassembly of section .data:

0+ <foo>:
   0:	01 00 00 00 	nop 
			1: R_SPARC_UA32	fred
   4:	00 02 00 00 	(unimp|illtrap)  0x20000
			6: R_SPARC_UA16	jim
   8:	03 04 05 06 	sethi  %hi\(0x10141800\), %g1
