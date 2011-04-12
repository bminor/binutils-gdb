# as:
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <[^>]+> bf18      	it	ne
0+002 <[^>]+> f7ff bffd 	bne.w	0+0 <[^>]+>
0+006 <[^>]+> bf38      	it	cc
0+008 <[^>]+> f7ff bffa 	bcc.w	0+0 <[^>]+>
0+00c <[^>]+> bf28      	it	cs
0+00e <[^>]+> f7ff fff7 	blcs	0+0 <[^>]+>
0+012 <[^>]+> bfb8      	it	lt
0+014 <[^>]+> 47a8      	blxlt	r5
0+016 <[^>]+> bf08      	it	eq
0+018 <[^>]+> 4740      	bxeq	r8
0+01a <[^>]+> bfc8      	it	gt
0+01c <[^>]+> e8d4 f001 	tbbgt	\[r4, r1\]
0+020 <[^>]+> bfb8      	it	lt
0+022 <[^>]+> df00      	svclt	0
0+024 <[^>]+> bf08      	it	eq
0+026 <[^>]+> f8d0 f000 	ldreq.w	pc, \[r0\]
0+02a <[^>]+> bfdc      	itt	le
0+02c <[^>]+> be00      	bkpt	0x0000
0+02e <[^>]+> bf00      	nople
0+030 <[^>]+> bf00      	nop
#...
