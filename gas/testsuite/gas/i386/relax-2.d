#name: i386 relax 3
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

0+ <.text>:
#...
  38:	0f 85 88 00 00 00    	jne    (0x)?c6( .*)?
#...
  48:	75 7c                	jne    (0x)?c6( .*)?
#...
  c6:	90                   	nop
#pass
