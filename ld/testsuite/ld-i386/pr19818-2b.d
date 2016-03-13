#source: pr19818-2.s
#as: --32
#ld: -pie -m elf_i386 --defsym foo=0xffffffff
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	c7 c0 ff ff ff ff    	mov    \$0xffffffff,%eax
#pass
