#as: -J
#objdump: -dw
#name: i386 branch

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	3e 74 03[ 	]+je,pt  +0x[0-9a-f]+
[ 	]*[a-f0-9]+:	2e 74 00[ 	]+je,pn  +0x[0-9a-f]+
#pass
