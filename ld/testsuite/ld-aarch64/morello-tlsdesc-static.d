#source: morello-tlsdesc.s
#as: -march=morello+c64 --defsym STATIC=1
#ld:
#objdump: -D -j .got.plt -j .text -j .plt


.*:     file format .*


Disassembly of section .text:

.*<f2>:
.*:	94000008 	bl	.* <_start>
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d2a00001 	movz	x1, #0x0, lsl #16
.*:	d2a00000 	movz	x0, #0x0, lsl #16
.*:	f2800400 	movk	x0, #0x20
.*:	f2800081 	movk	x1, #0x4
.*:	c2a06040 	add	c0, c2, x0, uxtx
.*:	c2c10000 	scbnds	c0, c0, x1

.*<_start>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	d2a00001 	movz	x1, #0x0, lsl #16
.*:	d2a00000 	movz	x0, #0x0, lsl #16
.*:	f2800480 	movk	x0, #0x24
.*:	f2800281 	movk	x1, #0x14
.*:	c2a06040 	add	c0, c2, x0, uxtx
.*:	c2c10000 	scbnds	c0, c0, x1
