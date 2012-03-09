#as: -mppc -me5500
#objdump: -dr -Me5500
#name: Power E5500 nop tests

.*: +file format elf(32)?(64)?-powerpc.*

Disassembly of section \.text:

0+00 <start>:
   0:	60 00 00 00 	nop
   4:	60 00 00 00 	nop
   8:	60 00 00 00 	nop
   c:	60 00 00 00 	nop
