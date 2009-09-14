#as: -J
#objdump: -drw
#name: x86-64 displacement

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	8b 98 ff ff ff 7f    	mov    0x7fffffff\(%rax\),%ebx
[ 	]*[a-f0-9]+:	8b 98 00 00 00 80    	mov    -0x80000000\(%rax\),%ebx
[ 	]*[a-f0-9]+:	8b 1c 25 00 00 00 80 	mov    0xffffffff80000000,%ebx
[ 	]*[a-f0-9]+:	8b 1c 25 00 00 00 80 	mov    0xffffffff80000000,%ebx
[ 	]*[a-f0-9]+:	8b 1c 25 ff ff ff 7f 	mov    0x7fffffff,%ebx
[ 	]*[a-f0-9]+:	8b 04 25 00 00 00 80 	mov    0xffffffff80000000,%eax
[ 	]*[a-f0-9]+:	8b 04 25 00 00 00 80 	mov    0xffffffff80000000,%eax
[ 	]*[a-f0-9]+:	8b 04 25 ff ff ff 7f 	mov    0x7fffffff,%eax
[ 	]*[a-f0-9]+:	a1 00 00 00 80 00 00 00 00 	mov    0x80000000,%eax
#pass
