#as: -J
#objdump: -dw
#name: i386 intelpic

.*: +file format .*

Disassembly of section .text:

0+ <gs_foo>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1 <bar>:
[ 	]*[a-f0-9]+:	8d 83 14 00 00 00    	lea    0x14\(%ebx\),%eax
[ 	]*[a-f0-9]+:	8b 83 00 00 00 00    	mov    0x0\(%ebx\),%eax
[ 	]*[a-f0-9]+:	ff 24 85 0d 00 00 00 	jmp    \*0xd\(,%eax,4\)
[ 	]*[a-f0-9]+:	8d 83 14 00 00 00    	lea    0x14\(%ebx\),%eax
#pass
