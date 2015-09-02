#as: --64
#ld: -melf_x86_64
#objdump: -dw
#target: x86_64-*-*

.*: +file format .*


Disassembly of section .plt:

0+4000d0 <.plt>:
[ 	]*[a-f0-9]+:	ff 25 32 00 20 00    	jmpq   \*0x200032\(%rip\)        # 600108 <_GLOBAL_OFFSET_TABLE_\+0x18>
[ 	]*[a-f0-9]+:	68 00 00 00 00       	pushq  \$0x0
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   4000e0 <__start>

Disassembly of section .text:

0+4000e0 <__start>:
[ 	]*[a-f0-9]+:	48 8b 05 21 00 20 00 	mov    0x200021\(%rip\),%rax        # 600108 <_GLOBAL_OFFSET_TABLE_\+0x18>
[ 	]*[a-f0-9]+:	48 8d ([0-9a-f]{2} ){5}[ 	]+lea[ 	]+.*

0+4000ee <foo>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+4000ef <bar>:
[ 	]*[a-f0-9]+:	c3                   	retq   
#pass
