#objdump: -dw
#name: Disassemble bad broadcast

.*: +file format .*


Disassembly of section .text:

0+ <.text>:
 +[a-f0-9]+:	62                   	.byte 0x62
 +[a-f0-9]+:	c3                   	ret    
 +[a-f0-9]+:	8c 1d 66 90 66 90    	mov    %ds,0x90669066
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax
#pass
