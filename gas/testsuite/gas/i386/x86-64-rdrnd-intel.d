#objdump: -drwMintel
#name: x86-64 RdRnd(Intel mode)
#source: x86-64-rdrnd.s

.*: +file format .*


Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	66 0f c7 f3          	rdrnd  bx
[ 	]*[a-f0-9]+:	0f c7 f3             	rdrnd  ebx
[ 	]*[a-f0-9]+:	48 0f c7 f3          	rdrnd  rbx
[ 	]*[a-f0-9]+:	66 41 0f c7 f0       	rdrnd  r8w
[ 	]*[a-f0-9]+:	41 0f c7 f0          	rdrnd  r8d
[ 	]*[a-f0-9]+:	49 0f c7 f0          	rdrnd  r8
[ 	]*[a-f0-9]+:	66 0f c7 f3          	rdrnd  bx
[ 	]*[a-f0-9]+:	0f c7 f3             	rdrnd  ebx
[ 	]*[a-f0-9]+:	48 0f c7 f3          	rdrnd  rbx
[ 	]*[a-f0-9]+:	66 41 0f c7 f0       	rdrnd  r8w
[ 	]*[a-f0-9]+:	41 0f c7 f0          	rdrnd  r8d
[ 	]*[a-f0-9]+:	49 0f c7 f0          	rdrnd  r8
#pass
