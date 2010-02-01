
.*:     file format.*


Disassembly of section destsect:

09000000 <[^>]*>:
 9000000:	e7fe      	b.n	9000000 <dest>

Disassembly of section .text:

00008058 <[^>]*>:
    8058:	b802f000 	.word	0xb802f000
    805c:	00000000 	andeq	r0, r0, r0

00008060 <[^>]*>:
    8060:	4778      	bx	pc
    8062:	46c0      	nop			; \(mov r8, r8\)
    8064:	e59fc000 	ldr	ip, \[pc, #0\]	; 806c <__dest_veneer\+0xc>
    8068:	e12fff1c 	bx	ip
    806c:	09000001 	.word	0x09000001
