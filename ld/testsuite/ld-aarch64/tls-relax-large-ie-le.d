#source: tls-relax-large-ie-le.s
#ld: -T relocs.ld -e0
#objdump: -dr
#...
0000000000010000 <test>:
  +10000:	d53bd041 	mrs	x1, tpidr_el0
  +10004:	d2c00006 	movz	x6, #0x0, lsl #32
  +10008:	f2a00006 	movk	x6, #0x0, lsl #16
  +1000c:	f2800206 	movk	x6, #0x10
  +10010:	8b0100c6 	add	x6, x6, x1
  +10014:	d2c00007 	movz	x7, #0x0, lsl #32
  +10018:	f2a00007 	movk	x7, #0x0, lsl #16
  +1001c:	f2800287 	movk	x7, #0x14
  +10020:	8b0100e7 	add	x7, x7, x1
