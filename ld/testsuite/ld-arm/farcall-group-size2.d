
.*:     file format .*

Disassembly of section .text:

00001000 <_start>:
    1000:	eb000002 	bl	1010 <__bar_from_arm>
    1004:	ebffffff 	bl	1008 <__bar2_veneer>
00001008 <__bar2_veneer>:
    1008:	e51ff004 	ldr	pc, \[pc, #-4\]	; 100c <__bar2_veneer\+0x4>
    100c:	02003024 	.word	0x02003024
00001010 <__bar_from_arm>:
    1010:	e59fc000 	ldr	ip, \[pc, #0\]	; 1018 <__bar_from_arm\+0x8>
    1014:	e12fff1c 	bx	ip
    1018:	02003021 	.word	0x02003021
    101c:	00000000 	.word	0x00000000
00001020 <myfunc>:
    1020:	eb000002 	bl	1030 <__bar3_veneer>
    1024:	eb000006 	bl	1044 <__bar4_from_arm>
    1028:	eb000002 	bl	1038 <__bar5_from_arm>
    102c:	00000000 	andeq	r0, r0, r0
00001030 <__bar3_veneer>:
    1030:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1034 <__bar3_veneer\+0x4>
    1034:	02003028 	.word	0x02003028
00001038 <__bar5_from_arm>:
    1038:	e59fc000 	ldr	ip, \[pc, #0\]	; 1040 <__bar5_from_arm\+0x8>
    103c:	e12fff1c 	bx	ip
    1040:	0200302f 	.word	0x0200302f
00001044 <__bar4_from_arm>:
    1044:	e59fc000 	ldr	ip, \[pc, #0\]	; 104c <__bar4_from_arm\+0x8>
    1048:	e12fff1c 	bx	ip
    104c:	0200302d 	.word	0x0200302d
	...

Disassembly of section .foo:

02003020 <bar>:
 2003020:	4770      	bx	lr
	...

02003024 <bar2>:
 2003024:	e12fff1e 	bx	lr

02003028 <bar3>:
 2003028:	e12fff1e 	bx	lr

0200302c <bar4>:
 200302c:	4770      	bx	lr

0200302e <bar5>:
 200302e:	4770      	bx	lr
