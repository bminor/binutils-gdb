#name: i386 relax 1
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

0+ <.text>:
#...
   e:	e9 8d 00 00 00       	jmp    0xa0
#...
  21:	eb 7d                	jmp    0xa0
#...
  a0:	90                   	nop
#pass
