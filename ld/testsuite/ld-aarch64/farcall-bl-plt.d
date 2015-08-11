#name: aarch64-farcall-bl-plt
#source: farcall-bl-plt.s
#as:
#ld: -shared
#objdump: -dr
#...

Disassembly of section .plt:

.* <foo@plt-0x20>:
.*:	a9bf7bf0 	stp	x16, x30, \[sp,#-16\]!
.*:	90040090 	adrp	x16, 8010000 <__foo_veneer\+.*>
.*:	f941f611 	ldr	x17, \[x16,#1000\]
.*:	910fa210 	add	x16, x16, #0x3e8
.*:	d61f0220 	br	x17
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	d503201f 	nop

.* <foo@plt>:
.*:	90040090 	adrp	x16, 8010000 <__foo_veneer\+.*>
.*:	f941fa11 	ldr	x17, \[x16,#1008\]
.*:	910fc210 	add	x16, x16, #0x3f0
.*:	d61f0220 	br	x17

Disassembly of section .text:

.* <_start>:
	...
.*:	94000003 	bl	80002c8 <__foo_veneer>
.*:	d65f03c0 	ret
.*:	14000007 	b	80002e0 <__foo_veneer\+.*>

.* <__foo_veneer>:
.*:	90fc0010 	adrp	x16, 0 <foo@plt-0x2b0>
.*:	910ac210 	add	x16, x16, #0x2b0
.*:	d61f0200 	br	x16
	...
