#objdump: -S -r
#as: -m68hc12 -gdwarf2
#name: 68HC12 PC-relative addressing modes (bug-1825)

.*:     file format elf32\-m68hc12

Disassembly of section \.text:

0+ <_main>:
;;; 
	\.sect \.text
	\.globl _main
_main:
	nop
   0:	a7          	nop
	ldx	L1,pc		; Assemble to 5-bit > 0 offset
   1:	ee c2       	ldx	2,PC \{5 <L1>\}
	bra	L2
   3:	20 02       	bra	7 <L2>
			3: R_M68HC12_RL_JUMP	\*ABS\*

0+5 <L1>:
   5:	aa bb       	oraa	5,SP\-

0+7 <L2>:
L1:
	.dc.w	0xaabb
L2:
	subd	L1,pc		; Assemble to 5-bit < 0 offset
   7:	a3 dc       	subd	\-4,PC \{5 <L1>\}

0+9 <L3>:
   9:	a7          	nop
   a:	a7          	nop
   b:	a7          	nop
   c:	a7          	nop
   d:	a7          	nop
   e:	a7          	nop
   f:	a7          	nop
  10:	a7          	nop
  11:	a7          	nop
  12:	a7          	nop
  13:	a7          	nop
  14:	a7          	nop
  15:	a7          	nop
  16:	a7          	nop
L3:
	.ds.b	14, 0xA7
	ldab	L3,pc		; 5-bit < 0 offset
  17:	e6 d0       	ldab	\-16,PC \{9 <L3>\}
	ldab	L4,pc		; 5-bit > 0 offset
  19:	e6 f8 20    	ldab	32,PC \{3b <L4>\} # SCz: FIXME this is wrong
	...

0+3b <L4>:
	...
	.skip	31
L4:
	.skip	128
	subd	L4,pc		; 9-bit < 0 offset
  bb:	a3 f9 7e    	subd	\-130,PC \{3b <L4>\}
	addd	L5,pc		; 9-bit > 0 offset
  be:	e3 f8 81    	addd	129,PC \{141 <L5>\} # SCz: FIXME
	...

0+141 <L5>:
	...
 23d:	00          	bgnd
 23e:	00          	bgnd
	.skip	128
L5:
	.skip	256-2
	orab	L5,pc		; 9 bit < 0 offset (min value)
 23f:	ea f9 00    	orab	-256,PC \{141 <L5>\}
	oraa	L6,pc		; 9 bit > 0 offset (max value)
 242:	aa fa 00 ff 	oraa	255,PC \{343 <L5\+0x202>\} # SCz: FIXME
	...

0+345 <L6>:
	.skip	255
L6:
	anda	_main,pc	; 16 bit < 0 offset
 345:	a4 fa fc b7 	anda	-841,PC \{fffffffe <\.L0\+0xfffffb8c>\} # SCz: FIXME
	andb	L7,pc
 349:	e4 fa 01 00 	andb	256,PC \{44b <L6\+0x106>\} # SCz: FIXME
	...

0+44d <L7>:
	.skip	256
L7:
	stab	external,pc	; External 16\-bit PCREL
 44d:	6b fa 00 00 	stab	0,PC \{44f <L7\+0x2>\}
			44f: R_M68HC12_16	external # SCz: FIXME
	ldd	_table,pc
 451:	ec cf       	ldd	15,PC \{462 <_table>\}
	addd	_table\+2,pc
 453:	e3 cf       	addd	15,PC \{464 <_table\+0x2>\}
	subd	_table\+4,pc
 455:	a3 cf       	subd	15,PC \{466 <_table\+0x4>\}
	addd	_table\+8,pc
 457:	e3 f8 11    	addd	17,PC \{46a <_table\+0x8>\}
	addd	_table\+12,pc
 45a:	e3 f8 12    	addd	18,PC \{46e <_table\+0xc>\}
	addd	_table\+16,pc
 45d:	e3 f8 13    	addd	19,PC \{472 <.L0>\}
	rts
 460:	3d          	rts
	nop
 461:	a7          	nop

0+462 <_table>:
	\.\.\.
