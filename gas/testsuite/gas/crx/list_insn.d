#as:
#objdump: -dr
#name: list_insn

.*: +file format .*

Disassembly of section .text:

00000000 <push>:
   0:	6e 34 18 00 	push	r14, {r3,r4}
   4:	b2 ff       	push	r2

00000006 <pushx>:
   6:	7f 34 ff 00 	pushx	r15, {r0,r1,r2,r3,r4,r5,r6,r7}

0000000a <pop>:
   a:	40 32 00 04 	loadm	r0, {r10}
   e:	c2 ff       	pop	r2

00000010 <popx>:
  10:	7f 32 fb 00 	popx	r15, {r0,r1,r3,r4,r5,r6,r7}

00000014 <popret>:
  14:	6e 32 02 40 	popret	r14, {r1,r14}
  18:	de ff       	popret	r14

0000001a <loadm>:
  1a:	40 32 03 00 	loadm	r0, {r0,r1}

0000001e <loadma>:
  1e:	5d 32 14 10 	popa	r13, {r2,r4,r12}

00000022 <popa>:
  22:	5e 32 00 80 	popa	r14, {r15}

00000026 <storm>:
  26:	4f 34 00 40 	storm	r15, {r14}

0000002a <storma>:
  2a:	53 34 01 00 	storma	r3, {r0}
