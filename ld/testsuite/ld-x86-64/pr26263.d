#as: --64
#ld: -m elf_x86_64 -shared
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

0+1000 <printk>:
 +[a-f0-9]+:	c3                   	retq   

Disassembly of section .init.text:

0+1001 <foo>:
 +[a-f0-9]+:	e8 fa ff ff ff       	callq  1000 <printk>
 +[a-f0-9]+:	e8 f5 ff ff ff       	callq  1000 <printk>
#pass
