#source: sysreg-4.s
#as: -march=armv8.5-a+rng
#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:
.*:	d50b7381 	cfp	rctx, x1
.*:	d50b73a2 	dvp	rctx, x2
.*:	d50b73e3 	cpp	rctx, x3
.*:	d50b7d24 	dc	cvadp, x4
.*:	d53b2405 	mrs	x5, rndr
.*:	d53b2426 	mrs	x6, rndrrs
