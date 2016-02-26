#source: pr19609-1.s
#as: --32 -mrelax-relocations=no
#ld: -melf_i386
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	3b 82 fc ff ff ff    	cmp    -0x4\(%edx\),%eax
[ 	]*[a-f0-9]+:	3b 8a fc ff ff ff    	cmp    -0x4\(%edx\),%ecx
[ 	]*[a-f0-9]+:	c7 c0 00 00 00 00    	mov    \$0x0,%eax
[ 	]*[a-f0-9]+:	c7 c1 00 00 00 00    	mov    \$0x0,%ecx
[ 	]*[a-f0-9]+:	85 82 fc ff ff ff    	test   %eax,-0x4\(%edx\)
[ 	]*[a-f0-9]+:	85 8a fc ff ff ff    	test   %ecx,-0x4\(%edx\)
