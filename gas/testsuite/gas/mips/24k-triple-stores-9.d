#objdump: -dr
#as: -mfix-24k
#name: 24K: Triple store (Intervening data #1)

.*: +file format .*mips.*

Disassembly of section .text:

00000000 <.text>:
   0:	a1020000 	sb	v0,0\(t0\)
   4:	00000000 	nop
   8:	a1030008 	sb	v1,8\(t0\)
   c:	a1040010 	sb	a0,16\(t0\)
