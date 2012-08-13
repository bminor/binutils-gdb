#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0000000000000000 <.*>:
   0:	4f03e420 	movi	v0\.16b, #0x61
   4:	98000181 	ldrsw	x1, 34 <\.text\+0x34>
   8:	98000007 	ldrsw	x7, 0 <\.text>
			8: R_AARCH64_LD_PREL_LO19	\.data\+0x4
   c:	fa42a02a 	ccmp	x1, x2, #0xa, ge
  10:	53001eaf 	uxtb	w15, w21
  14:	53003f67 	uxth	w7, w27
  18:	2a1f03e8 	mov	w8, wzr
  1c:	ab2013e0 	adds	x0, sp, w0, uxtb #4
  20:	ab2033e0 	adds	x0, sp, w0, uxth #4
  24:	ab2053e0 	adds	x0, sp, w0, uxtw #4
  28:	ab2083e0 	adds	x0, sp, w0, sxtb
  2c:	ab20a7e0 	adds	x0, sp, w0, sxth #1
  30:	ab20cbe0 	adds	x0, sp, w0, sxtw #2
  34:	deadbeef 	\.word	0xdeadbeef
