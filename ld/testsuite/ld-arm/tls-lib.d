
.*:     file format elf32-.*arm
architecture: arm, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
start address 0x.*

Disassembly of section .text:

00000328 <foo>:
 328:	e1a00000 	nop			\(mov r0,r0\)
 32c:	e1a00000 	nop			\(mov r0,r0\)
 330:	e1a0f00e 	mov	pc, lr
 334:	00008098 	muleq	r0, r8, r0
 338:	0000808c 	andeq	r8, r0, ip, lsl #1
 33c:	00000004 	andeq	r0, r0, r4
