.*:     file format .*

Disassembly of section .text:

00001000 <_start-0x18>:
    1000:	b540      	push	{r6, lr}
    1002:	4e03      	ldr	r6, \[pc, #12\]	\(1010 <_start-0x8>\)
    1004:	46fe      	mov	lr, pc
    1006:	4730      	bx	r6
    1008:	e8bd4040 	pop	{r6, lr}
    100c:	e12fff1e 	bx	lr
	...

00001018 <_start>:
    1018:	f7ff fff2 	bl	1000 <_start-0x18>
Disassembly of section .foo:

02001014 <bar>:
 2001014:	e12fff1e 	bx	lr
