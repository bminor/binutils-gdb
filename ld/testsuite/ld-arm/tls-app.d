
.*:     file format elf32-.*arm
architecture: arm, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x00008204

Disassembly of section .text:

00008204 <foo>:
    8204:	e1a00000 	nop			\(mov r0,r0\)
    8208:	e1a00000 	nop			\(mov r0,r0\)
    820c:	e1a0f00e 	mov	pc, lr
    8210:	000080bc 	streqh	r8, \[r0\], -ip
    8214:	000080b4 	streqh	r8, \[r0\], -r4
    8218:	000080ac 	andeq	r8, r0, ip, lsr #1
    821c:	00000004 	andeq	r0, r0, r4
    8220:	000080c4 	andeq	r8, r0, r4, asr #1
    8224:	00000014 	andeq	r0, r0, r4, lsl r0
