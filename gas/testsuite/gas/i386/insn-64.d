#objdump: -dw
#name: .insn (64-bit code)

.*: +file format .*

Disassembly of section .text:

0+ <insn>:
[ 	]*[a-f0-9]+:	90[ 	]+nop
[ 	]*[a-f0-9]+:	f3 90[ 	]+pause
[ 	]*[a-f0-9]+:	f3 90[ 	]+pause
[ 	]*[a-f0-9]+:	d9 ee[ 	]+fldz
[ 	]*[a-f0-9]+:	f3 0f 01 e8[ 	]+setssbsy
[ 	]*[a-f0-9]+:	c5 fc 77[ 	]+vzeroall
[ 	]*[a-f0-9]+:	c4 e1 7c 77[ 	]+vzeroall
#pass
