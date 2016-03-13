#source: pr19818-2.s
#source: pr19818-1c.s
#as: --32
#ld: -shared -m elf_i386
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	c7 c0 ff ff ff ff    	mov    \$0xffffffff,%eax
#pass
