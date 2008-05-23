
.*:     file format .*

Disassembly of section .text:

00001000 <_start-0x18>:
    1000:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1004 <_start-0x14>
    1004:	02003024 	.word	0x02003024
    1008:	e59fc000 	ldr	ip, \[pc, #0\]	; 1010 <_start-0x8>
    100c:	e12fff1c 	bx	ip
    1010:	02003021 	.word	0x02003021
    1014:	00000000 	.word	0x00000000

00001018 <_start>:
    1018:	ebfffffa 	bl	1008 <_start-0x10>
    101c:	ebfffff7 	bl	1000 <_start-0x18>
    1020:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1024 <_start\+0xc>
    1024:	02003028 	.word	0x02003028
    1028:	e59fc000 	ldr	ip, \[pc, #0\]	; 1030 <_start\+0x18>
    102c:	e12fff1c 	bx	ip
    1030:	0200302f 	.word	0x0200302f
    1034:	e59fc000 	ldr	ip, \[pc, #0\]	; 103c <_start\+0x24>
    1038:	e12fff1c 	bx	ip
    103c:	0200302d 	.word	0x0200302d
	...

00001048 <myfunc>:
    1048:	ebfffff4 	bl	1020 <_start\+0x8>
    104c:	ebfffff8 	bl	1034 <_start\+0x1c>
    1050:	ebfffff4 	bl	1028 <_start\+0x10>
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
