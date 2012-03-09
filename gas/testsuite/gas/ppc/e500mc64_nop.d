#as: -mppc -me500mc64
#objdump: -dr -Me500mc64
#name: Power E500MC64 nop tests

.*: +file format elf(32)?(64)?-powerpc.*

Disassembly of section \.text:

0+00 <start>:
   0:	60 00 00 00 	nop
   4:	60 00 00 00 	nop
   8:	60 00 00 00 	nop
   c:	60 00 00 00 	nop
