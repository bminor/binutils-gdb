#objdump: --prefix-addresses -dr
#name: FFxx1

# Test for FFxx:8 addressing.

.*:     file format .*h8300.*

Disassembly of section .text:
	...
			0: 16	main
00000400 <main> f8 7f             mov.b	#0x7f,r0l
00000402 <main[+]2> 28 bb             mov.b	@0xbb:8,r0l
00000404 <main[+]4> 6a 88 ff b9       mov.b	r0l,@0xffb9:16
00000408 <main[+]8> f8 01             mov.b	#0x1,r0l
0000040a <loop> 6a 88 ff bb       mov.b	r0l,@0xffbb:16
0000040e <delay> 79 01 00 00       mov.w	#0x0,r1
00000412 <deloop> 0b 01             adds	#0x1,er1
00000414 <deloop[+]2> 46 00             bne	.0 \(416\)
			415: DISP8	deloop[+]0xffffffff
00000416 <deloop[+]4> 12 88             rotl	r0l
00000418 <deloop[+]6> 40 00             bra	.0 \(41a\)
			419: DISP8	loop[+]0xffffffff
	...
