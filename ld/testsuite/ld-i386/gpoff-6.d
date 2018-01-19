#as: --32
#ld: -melf_i386 --gc-sections
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

0+[a-f0-9]+ <_start>:
 +[a-f0-9]+:	c3                   	ret    
#pass
