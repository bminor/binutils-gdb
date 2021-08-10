#as: -march=armv8-a+c64
#objdump: -drt

.*\.o:     file format .*

SYMBOL TABLE:
0000000000000000 l    d  \.text	0000000000000000 \.text
0000000000000000 l    d  \.data	0000000000000000 \.data
0000000000000000 l    d  \.bss	0000000000000000 \.bss
000000000000000c l     F \.text	0000000000000000 a
000000000000000c l       \.text	0000000000000000 altlabel
000000000000000c g     F \.text	0000000000000000 f
0000000000000000         \*UND\*	0000000000000000 x



Disassembly of section \.text:

0000000000000000 <f-0xc>:
	\.\.\.

000000000000000c <f>:
   c:	10000000 	adr	c0, 0 <x>
			c: R_AARCH64_ADR_PREL_LO21	x
  10:	10000000 	adr	c0, c <f>
			10: R_AARCH64_ADR_PREL_LO21	f
  14:	30ffffc0 	adr	c0, d <f\+0x1>
  18:	10ffffa0 	adr	c0, c <f>
  1c:	14000000 	b	0 <x>
			1c: R_MORELLO_JUMP26	x
  20:	14000000 	b	c <f>
			20: R_MORELLO_JUMP26	f
  24:	17fffffa 	b	c <f>
  28:	17fffff9 	b	c <f>
  2c:	5400000d 	b\.le	0 <x>
			2c: R_MORELLO_CONDBR19	x
  30:	5400000d 	b\.le	c <f>
			30: R_MORELLO_CONDBR19	f
  34:	54fffecd 	b\.le	c <f>
  38:	54fffead 	b\.le	c <f>
  3c:	36080001 	tbz	w1, #1, 0 <x>
			3c: R_MORELLO_TSTBR14	x
  40:	36080001 	tbz	w1, #1, c <f>
			40: R_MORELLO_TSTBR14	f
  44:	360ffe41 	tbz	w1, #1, c <f>
  48:	360ffe21 	tbz	w1, #1, c <f>
  4c:	94000000 	bl	0 <x>
			4c: R_MORELLO_CALL26	x
  50:	94000000 	bl	c <f>
			50: R_MORELLO_CALL26	f
  54:	97ffffee 	bl	c <f>
  58:	97ffffed 	bl	c <f>
