#source: gpoff-8.s
#as: --64 -mrelax-relocations=yes
#ld: -melf_x86_64 -T gpoff-8.t
#objdump: -dw --sym

.*: +file format .*

SYMBOL TABLE:
#...
[a-f0-9]+ l       .data	0+ __gp
#...

Disassembly of section .text:

0+[a-f0-9]+ <_start>:
 +[a-f0-9]+:[ \t]+65 8b 04 25 00 00 00 00[ \t]+mov[ \t]+%gs:0x0,%eax
 +[a-f0-9]+:[ \t]+48 c7 c0 [a-f0-9 \t]+mov[ \t]+\$0x[a-f0-9]+,%rax
#pass
