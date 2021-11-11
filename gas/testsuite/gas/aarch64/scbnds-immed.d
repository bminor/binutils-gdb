#name: test valid scbnds immediates
#as: -march=morello
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

.* <\.text>:
.*:	c2c03820 	scbnds	c0, c1, #0x0
.*:	c2d03820 	scbnds	c0, c1, #0x20
.*:	c2dfb820 	scbnds	c0, c1, #0x3f
.*:	c2c27820 	scbnds	c0, c1, #0x4, lsl #4
.*:	c2c2f820 	scbnds	c0, c1, #0x5, lsl #4
.*:	c2dff820 	scbnds	c0, c1, #0x3f, lsl #4
