#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0000000000000000 <.*>:
   0:	110003ef 	mov	w15, wsp
   4:	910003ef 	mov	x15, sp
   8:	110000ff 	mov	wsp, w7
   c:	910000ff 	mov	sp, x7
  10:	110003ff 	mov	wsp, wsp
  14:	910003ff 	mov	sp, sp
  18:	aa0f03e7 	mov	x7, x15
  1c:	2a0f03e7 	mov	w7, w15
  20:	52800b01 	movz	w1, #0x58
  24:	12800000 	movn	w0, #0x0
  28:	b2607fe0 	orr	x0, xzr, #0xffffffff00000000
  2c:	b2400fff 	orr	sp, xzr, #0xf
  30:	32000fff 	orr	wsp, wzr, #0xf
  34:	d28001ff 	movz	xzr, #0xf
  38:	528001ff 	movz	wzr, #0xf
  3c:	0e1c3de7 	mov	w7, v15\.s\[3\]
  40:	4e183fef 	mov	x15, v31\.d\[1\]
  44:	d2801fe0 	movz	x0, #0xff
  48:	320de400 	orr	w0, w0, #0x99999999
