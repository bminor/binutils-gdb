#source: pr19818-1a.s
#source: pr19818-1b.s
#as: --32
#ld: -pie -m elf_i386
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	b8 ff ff ff ff       	mov    \$0xffffffff,%eax
#pass
