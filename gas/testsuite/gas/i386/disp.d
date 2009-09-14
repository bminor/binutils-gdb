#as: -J
#objdump: -drw
#name: i386 displacement

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	8b 98 ff ff ff 7f    	mov    0x7fffffff\(%eax\),%ebx
[ 	]*[a-f0-9]+:	8b 98 00 00 00 80    	mov    -0x80000000\(%eax\),%ebx
[ 	]*[a-f0-9]+:	8b 98 00 00 00 80    	mov    -0x80000000\(%eax\),%ebx
[ 	]*[a-f0-9]+:	8b 1d ff ff ff 7f    	mov    0x7fffffff,%ebx
[ 	]*[a-f0-9]+:	8b 1d 00 00 00 80    	mov    0x80000000,%ebx
[ 	]*[a-f0-9]+:	8b 1d 00 00 00 80    	mov    0x80000000,%ebx
[ 	]*[a-f0-9]+:	a1 ff ff ff 7f       	mov    0x7fffffff,%eax
[ 	]*[a-f0-9]+:	a1 00 00 00 80       	mov    0x80000000,%eax
[ 	]*[a-f0-9]+:	a1 00 00 00 80       	mov    0x80000000,%eax
#pass
