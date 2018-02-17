#source: nop-2.s
#objdump: -drw -Mi8086
#name: x86-64 .nop 2

.*: +file format .*


Disassembly of section .text:

0+ <single>:
 +[a-f0-9]+:	90                   	nop

0+1 <pseudo_1>:
 +[a-f0-9]+:	90                   	nop

0+2 <pseudo_8>:
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di

0+a <pseudo_8_4>:
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si

0+12 <pseudo_20>:
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di

0+26 <pseudo_30>:
 +[a-f0-9]+:	89 f6                	mov    %si,%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di
 +[a-f0-9]+:	8d b4 00 00          	lea    0x0\(%si\),%si
 +[a-f0-9]+:	8d bd 00 00          	lea    0x0\(%di\),%di
 +[a-f0-9]+:	66 31 c0             	xor    %eax,%eax
#pass
