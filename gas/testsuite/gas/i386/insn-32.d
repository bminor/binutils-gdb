#objdump: -dw
#name: .insn (32-bit code)

.*: +file format .*

Disassembly of section .text:

0+ <insn>:
[ 	]*[a-f0-9]+:	90[ 	]+nop
[ 	]*[a-f0-9]+:	f3 90[ 	]+pause
[ 	]*[a-f0-9]+:	f3 90[ 	]+pause
[ 	]*[a-f0-9]+:	d9 ee[ 	]+fldz
[ 	]*[a-f0-9]+:	f3 0f 01 e8[ 	]+setssbsy
#pass
