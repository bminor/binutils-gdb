#objdump: -dr
#as: -mfix-24k -32
#name: 24K: Triple Store (Mix byte/half/word size check)

.*: +file format .*mips.*

Disassembly of section .text:
0+ <.*>:
   0:	a5020007 	sh	v0,7\(t0\)
   4:	00000000 	nop
   8:	a1030000 	sb	v1,0\(t0\)
   c:	ad040001 	sw	a0,1\(t0\)
  10:	0000000d 	break
  14:	a5020016 	sh	v0,22\(t0\)
  18:	a103000f 	sb	v1,15\(t0\)
  1c:	00000000 	nop
  20:	ad040018 	sw	a0,24\(t0\)
  24:	0000000d 	break
  28:	a5020000 	sh	v0,0\(t0\)
  2c:	a1030009 	sb	v1,9\(t0\)
  30:	ad040002 	sw	a0,2\(t0\)
  34:	0000000d 	break
  38:	a5020006 	sh	v0,6\(t0\)
  3c:	a1030010 	sb	v1,16\(t0\)
  40:	00000000 	nop
  44:	ad04000c 	sw	a0,12\(t0\)
  48:	0000000d 	break
  4c:	a502000a 	sh	v0,10\(t0\)
  50:	a103000f 	sb	v1,15\(t0\)
  54:	ad040004 	sw	a0,4\(t0\)
  58:	0000000d 	break
  5c:	a502000a 	sh	v0,10\(t0\)
  60:	a1030010 	sb	v1,16\(t0\)
  64:	00000000 	nop
  68:	ad040004 	sw	a0,4\(t0\)
  6c:	0000000d 	break
	\.\.\.
