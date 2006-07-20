#as: -mips32r2
#source: mips16-call-global-1.S mips16-call-global-2.S mips16-call-global-3.S

.*:     file format elf.*mips

Disassembly of section .text:

00400090 <__start>:
  400090:	64c3      	save	24,ra
  400092:	1a00 002e 	jal	4000b8 <x\+0x8>
  400096:	6500      	nop
  400098:	1e00 0032 	jalx	4000c8 <y\+0x8>
  40009c:	6500      	nop
  40009e:	6443      	restore	24,ra
  4000a0:	e8a0      	jrc	ra
  4000a2:	6500      	nop
  4000a4:	6500      	nop
  4000a6:	6500      	nop
  4000a8:	6500      	nop
  4000aa:	6500      	nop
  4000ac:	6500      	nop
  4000ae:	6500      	nop

004000b0 <x>:
  4000b0:	e8a0      	jrc	ra
  4000b2:	6500      	nop
  4000b4:	6500      	nop
  4000b6:	6500      	nop
  4000b8:	6500      	nop
  4000ba:	6500      	nop
  4000bc:	6500      	nop
  4000be:	6500      	nop

004000c0 <y>:
  4000c0:	03e00008 	jr	ra
  4000c4:	00000000 	nop
	\.\.\.
