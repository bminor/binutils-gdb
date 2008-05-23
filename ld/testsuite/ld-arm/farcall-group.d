
.*:     file format .*

Disassembly of section .text:

00001000 <_start-0x40>:
    1000:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1004 <_start-0x3c>
    1004:	02003024 	.word	0x02003024
    1008:	e59fc000 	ldr	ip, \[pc, #0\]	; 1010 <_start-0x30>
    100c:	e12fff1c 	bx	ip
    1010:	02003021 	.word	0x02003021
    1014:	e51ff004 	ldr	pc, \[pc, #-4\]	; 1018 <_start-0x28>
    1018:	02003028 	.word	0x02003028
    101c:	e59fc000 	ldr	ip, \[pc, #0\]	; 1024 <_start-0x1c>
    1020:	e12fff1c 	bx	ip
    1024:	0200302d 	.word	0x0200302d
    1028:	e59fc000 	ldr	ip, \[pc, #0\]	; 1030 <_start-0x10>
    102c:	e12fff1c 	bx	ip
    1030:	0200302f 	.word	0x0200302f
	...

00001040 <_start>:
    1040:	ebfffff0 	bl	1008 <_start-0x38>
    1044:	ebffffed 	bl	1000 <_start-0x40>

00001048 <myfunc>:
    1048:	ebfffff1 	bl	1014 <_start-0x2c>
    104c:	ebfffff2 	bl	101c <_start-0x24>
    1050:	ebfffff4 	bl	1028 <_start-0x18>
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
