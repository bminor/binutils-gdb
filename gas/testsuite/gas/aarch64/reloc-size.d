#as: -march=morello+c64
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

0000000000000000 <_start>:
   0:	f280000c 	movk	x12, #0x0
			0: R_MORELLO_MOVW_SIZE_G0	sym1
   4:	f280000c 	movk	x12, #0x0
			4: R_MORELLO_MOVW_SIZE_G0_NC	sym1
   8:	f2a0000c 	movk	x12, #0x0, lsl #16
			8: R_MORELLO_MOVW_SIZE_G1	sym1
   c:	f2a0000c 	movk	x12, #0x0, lsl #16
			c: R_MORELLO_MOVW_SIZE_G1_NC	sym1
  10:	f2c0000c 	movk	x12, #0x0, lsl #32
			10: R_MORELLO_MOVW_SIZE_G2	sym1
  14:	f2c0000c 	movk	x12, #0x0, lsl #32
			14: R_MORELLO_MOVW_SIZE_G2_NC	sym1
  18:	f2e0000c 	movk	x12, #0x0, lsl #48
			18: R_MORELLO_MOVW_SIZE_G3	sym1
  1c:	f280000d 	movk	x13, #0x0
			1c: R_MORELLO_MOVW_SIZE_G0_NC	sym2
  20:	f2a0000d 	movk	x13, #0x0, lsl #16
			20: R_MORELLO_MOVW_SIZE_G1	sym2
  24:	f2a0000d 	movk	x13, #0x0, lsl #16
			24: R_MORELLO_MOVW_SIZE_G1_NC	sym2
  28:	f2c0000d 	movk	x13, #0x0, lsl #32
			28: R_MORELLO_MOVW_SIZE_G2	sym2
  2c:	f2c0000d 	movk	x13, #0x0, lsl #32
			2c: R_MORELLO_MOVW_SIZE_G2_NC	sym2
  30:	f2e0000d 	movk	x13, #0x0, lsl #48
			30: R_MORELLO_MOVW_SIZE_G3	sym2
  34:	f280000d 	movk	x13, #0x0
			34: R_MORELLO_MOVW_SIZE_G0_NC	sym3
  38:	f2a0000d 	movk	x13, #0x0, lsl #16
			38: R_MORELLO_MOVW_SIZE_G1_NC	sym3
  3c:	f2c0000d 	movk	x13, #0x0, lsl #32
			3c: R_MORELLO_MOVW_SIZE_G2	sym3
  40:	f2c0000d 	movk	x13, #0x0, lsl #32
			40: R_MORELLO_MOVW_SIZE_G2_NC	sym3
  44:	f2e0000d 	movk	x13, #0x0, lsl #48
			44: R_MORELLO_MOVW_SIZE_G3	sym3
  48:	f280000d 	movk	x13, #0x0
			48: R_MORELLO_MOVW_SIZE_G0_NC	sym4
  4c:	f2a0000d 	movk	x13, #0x0, lsl #16
			4c: R_MORELLO_MOVW_SIZE_G1_NC	sym4
  50:	f2c0000d 	movk	x13, #0x0, lsl #32
			50: R_MORELLO_MOVW_SIZE_G2_NC	sym4
  54:	f2e0000d 	movk	x13, #0x0, lsl #48
			54: R_MORELLO_MOVW_SIZE_G3	sym4
  58:	d280000c 	mov	x12, #0x0                   	// #0
			58: R_MORELLO_MOVW_SIZE_G0	sym1
  5c:	d280000c 	mov	x12, #0x0                   	// #0
			5c: R_MORELLO_MOVW_SIZE_G0_NC	sym1
  60:	d2a0000c 	movz	x12, #0x0, lsl #16
			60: R_MORELLO_MOVW_SIZE_G1	sym1
  64:	d2a0000c 	movz	x12, #0x0, lsl #16
			64: R_MORELLO_MOVW_SIZE_G1_NC	sym1
  68:	d2c0000c 	movz	x12, #0x0, lsl #32
			68: R_MORELLO_MOVW_SIZE_G2	sym1
  6c:	d2c0000c 	movz	x12, #0x0, lsl #32
			6c: R_MORELLO_MOVW_SIZE_G2_NC	sym1
  70:	d2e0000c 	movz	x12, #0x0, lsl #48
			70: R_MORELLO_MOVW_SIZE_G3	sym1
  74:	d280000d 	mov	x13, #0x0                   	// #0
			74: R_MORELLO_MOVW_SIZE_G0_NC	sym2
  78:	d2a0000d 	movz	x13, #0x0, lsl #16
			78: R_MORELLO_MOVW_SIZE_G1	sym2
  7c:	d2a0000d 	movz	x13, #0x0, lsl #16
			7c: R_MORELLO_MOVW_SIZE_G1_NC	sym2
  80:	d2c0000d 	movz	x13, #0x0, lsl #32
			80: R_MORELLO_MOVW_SIZE_G2	sym2
  84:	d2c0000d 	movz	x13, #0x0, lsl #32
			84: R_MORELLO_MOVW_SIZE_G2_NC	sym2
  88:	d2e0000d 	movz	x13, #0x0, lsl #48
			88: R_MORELLO_MOVW_SIZE_G3	sym2
  8c:	d280000d 	mov	x13, #0x0                   	// #0
			8c: R_MORELLO_MOVW_SIZE_G0_NC	sym3
  90:	d2a0000d 	movz	x13, #0x0, lsl #16
			90: R_MORELLO_MOVW_SIZE_G1_NC	sym3
  94:	d2c0000d 	movz	x13, #0x0, lsl #32
			94: R_MORELLO_MOVW_SIZE_G2	sym3
  98:	d2c0000d 	movz	x13, #0x0, lsl #32
			98: R_MORELLO_MOVW_SIZE_G2_NC	sym3
  9c:	d2e0000d 	movz	x13, #0x0, lsl #48
			9c: R_MORELLO_MOVW_SIZE_G3	sym3
  a0:	d280000d 	mov	x13, #0x0                   	// #0
			a0: R_MORELLO_MOVW_SIZE_G0_NC	sym4
  a4:	d2a0000d 	movz	x13, #0x0, lsl #16
			a4: R_MORELLO_MOVW_SIZE_G1_NC	sym4
  a8:	d2c0000d 	movz	x13, #0x0, lsl #32
			a8: R_MORELLO_MOVW_SIZE_G2_NC	sym4
  ac:	d2e0000d 	movz	x13, #0x0, lsl #48
			ac: R_MORELLO_MOVW_SIZE_G3	sym4
