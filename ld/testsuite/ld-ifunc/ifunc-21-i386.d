#ld: -m elf_i386
#as: --32
#objdump: -dw
#target: x86_64-*-* i?86-*-*

.*: +file format .*


Disassembly of section .plt:

0+8048080 <.plt>:
[ 	]*[a-f0-9]+:	ff 25 b8 90 04 08    	jmp    \*0x80490b8
[ 	]*[a-f0-9]+:	68 00 00 00 00       	push   \$0x0
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmp    8048090 <__start>

Disassembly of section .text:

0+8048090 <__start>:
[ 	]*[a-f0-9]+:	e8 00 00 00 00       	call   8048095 <__start\+0x5>
[ 	]*[a-f0-9]+:	5b                   	pop    %ebx
[ 	]*[a-f0-9]+:	81 c3 17 10 00 00    	add    \$0x1017,%ebx
[ 	]*[a-f0-9]+:	8b 83 0c 00 00 00    	mov    0xc\(%ebx\),%eax
[ 	]*[a-f0-9]+:	8d ([0-9a-f]{2} ){5}[ 	]+lea[ 	]+.*

0+80480a8 <foo>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+80480a9 <bar>:
[ 	]*[a-f0-9]+:	c3                   	ret    
#pass
