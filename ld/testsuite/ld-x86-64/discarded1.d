#as: --64
#ld: -melf_x86_64 -T discarded1.t
#objdump: --sym -dw

.*: +file format .*

SYMBOL TABLE:
#...
0+8 g     O .data	0+4 x
#...

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	48 c7 c0 08 00 00 00 	mov    \$0x8,%rax
#pass
