#objdump:  -dw
#name: call operations

.*: +file format .*

Disassembly of section .text:
0+000 <foo>:
   0:	02 3d [ 	]*CALL  #02h
   2:	00 0b [ 	]*MOVE  PFX\[0\], #00h

0+004 <SmallCall>:
   4:	0d 8c [ 	]*RET 
   6:	0d ac [ 	]*RET C
   8:	0d 9c [ 	]*RET Z
   a:	0d dc [ 	]*RET NZ
   c:	0d cc [ 	]*RET S
   e:	8d 8c [ 	]*RETI 
  10:	8d ac [ 	]*RETI C
  12:	8d 9c [ 	]*RETI Z
  14:	8d dc [ 	]*RETI NZ
  16:	8d cc [ 	]*RETI S
  18:	10 7d [ 	]*MOVE  LC\[1\], #10h
  1a:	00 3d [ 	]*CALL  #00h
  1c:	ff 5d [ 	]*DJNZ  LC\[1\], #ffh
  1e:	10 7d [ 	]*MOVE  LC\[1\], #10h
  20:	00 3d [ 	]*CALL  #00h
	...
 422:	00 0b [ 	]*MOVE  PFX\[0\], #00h
 424:	1c 5d [ 	]*DJNZ  LC\[1\], #1ch

 426:	8d 8c [ 	]*RETI 
 428:	8d ac [ 	]*RETI C
 42a:	8d 9c [ 	]*RETI Z
 42c:	8d dc [ 	]*RETI NZ
 42e:	8d cc [ 	]*RETI S
	...
