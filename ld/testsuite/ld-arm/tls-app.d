
.*:     file format elf32-.*arm
architecture: arm, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x00008274

Disassembly of section .text:

00008274 <foo>:
    8274:	e1a00000 	nop			\(mov r0,r0\)
    8278:	e1a00000 	nop			\(mov r0,r0\)
    827c:	e1a0f00e 	mov	pc, lr
    8280:	000080bc 	streqh	r8, \[r0\], -ip
    8284:	000080b4 	streqh	r8, \[r0\], -r4
    8288:	000080ac 	andeq	r8, r0, ip, lsr #1
    828c:	00000004 	andeq	r0, r0, r4
    8290:	000080c4 	andeq	r8, r0, r4, asr #1
    8294:	00000014 	andeq	r0, r0, r4, lsl r0
