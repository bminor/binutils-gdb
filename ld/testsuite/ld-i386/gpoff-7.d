#as: --32
#ld: -melf_i386
#objdump: -dw --sym

.*: +file format .*

SYMBOL TABLE:
#...
[a-f0-9]+ l       .data	0+ __gp
#...

Disassembly of section .text:

0+[a-f0-9]+ <_start>:
 +[a-f0-9]+:[ \t]+64 a1 fe ff ff ff[ \t]+mov[ \t]+%fs:0xfffffffe,%eax
#pass
