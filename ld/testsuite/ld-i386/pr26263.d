#source: ../ld-x86-64/pr26263.s
#as: --32
#ld: -shared -melf_i386
#objdump: -dw

.*: +file format .*


Disassembly of section .text:

0+1000 <printk>:
 +[a-f0-9]+:	c3                   	ret    

Disassembly of section .init.text:

0+1001 <foo>:
 +[a-f0-9]+:	e8 fa ff ff ff       	call   1000 <printk>
 +[a-f0-9]+:	e8 f5 ff ff ff       	call   1000 <printk>
#pass
