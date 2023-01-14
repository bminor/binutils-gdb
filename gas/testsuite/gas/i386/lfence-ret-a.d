#source: lfence-ret.s
#as: -mlfence-before-ret=or
#objdump: -dw
#name: -mlfence-before-ret=or

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	83 0c 24 00          	orl    \$0x0,\(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	c3                   	ret    
 +[a-f0-9]+:	83 0c 24 00          	orl    \$0x0,\(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	c2 1e 00             	ret    \$0x1e
#pass
