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
   c:	14000000 	b	0 <x>
			c: R_MORELLO_JUMP26	x
  10:	14000000 	b	c <f>
			10: R_MORELLO_JUMP26	f
  14:	17fffffe 	b	c <f>
  18:	17fffffd 	b	c <f>
  1c:	5400000d 	b\.le	0 <x>
			1c: R_MORELLO_CONDBR19	x
  20:	5400000d 	b\.le	c <f>
			20: R_MORELLO_CONDBR19	f
  24:	54ffff4d 	b\.le	c <f>
  28:	54ffff2d 	b\.le	c <f>
  2c:	36080001 	tbz	w1, #1, 0 <x>
			2c: R_MORELLO_TSTBR14	x
  30:	36080001 	tbz	w1, #1, c <f>
			30: R_MORELLO_TSTBR14	f
  34:	360ffec1 	tbz	w1, #1, c <f>
  38:	360ffea1 	tbz	w1, #1, c <f>
  3c:	94000000 	bl	0 <x>
			3c: R_MORELLO_CALL26	x
  40:	94000000 	bl	c <f>
			40: R_MORELLO_CALL26	f
  44:	97fffff2 	bl	c <f>
  48:	97fffff1 	bl	c <f>
