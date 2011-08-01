#as: --x32
#ld: -m elf32_x86_64 -Ttext-segment 0xe0000000
#objdump: -dw

.*: +file format elf32-x86-64


Disassembly of section .text:

e0000054 <_start>:
[ 	]*[a-f0-9]+:	48 b8 54 00 00 e0 00 00 00 00 	movabs \$0xe0000054,%rax
[ 	]*[a-f0-9]+:	48 a1 54 00 00 e0 00 00 00 00 	movabs 0xe0000054,%rax
#pass
