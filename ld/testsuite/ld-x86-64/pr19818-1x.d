#source: pr19818-1a.s
#as: --x32
#ld: -pie -m elf32_x86_64 -T pr19818-1.t
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	b8 ff ff ff ff       	mov    \$0xffffffff,%eax
#pass
