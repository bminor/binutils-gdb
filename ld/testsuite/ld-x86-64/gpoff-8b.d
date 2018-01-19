#source: gpoff-8.s
#as: --64 -mrelax-relocations=yes
#ld: -melf_x86_64 -pie
#objdump: -dw --sym

.*: +file format .*

SYMBOL TABLE:
#...
[a-f0-9]+ l       .data	0+ __gp
#...

Disassembly of section .text:

0+[a-f0-9]+ <_start>:
 +[a-f0-9]+:[ \t]+65 8b 04 25 fe ff ff ff[ \t]+mov[ \t]+%gs:0xfffffffffffffffe,%eax
 +[a-f0-9]+:[ \t]+48 8d 05 [a-f0-9 \t]+lea[ \t]+0x[a-f0-9]+\(%rip\),%rax[ \t]+# [a-f0-9]+ <__gp>
#pass
