#source: tls32.s
#as: -a32
#ld: -shared -melf32ppc
#objdump: -dr
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Disassembly of section \.text:

0+46c <_start>:
 46c:	38 7f 00 1c 	addi    r3,r31,28
 470:	48 00 00 01 	bl      470 .*
 474:	38 7f 00 0c 	addi    r3,r31,12
 478:	48 00 00 01 	bl      478 .*
 47c:	38 7f 00 24 	addi    r3,r31,36
 480:	48 01 01 95 	bl      10614 .*
 484:	38 7f 00 0c 	addi    r3,r31,12
 488:	48 01 01 8d 	bl      10614 .*
 48c:	39 23 80 20 	addi    r9,r3,-32736
 490:	3d 23 00 00 	addis   r9,r3,0
 494:	81 49 80 24 	lwz     r10,-32732\(r9\)
 498:	81 3f 00 2c 	lwz     r9,44\(r31\)
 49c:	7d 49 12 2e 	lhzx    r10,r9,r2
 4a0:	89 42 00 00 	lbz     r10,0\(r2\)
 4a4:	3d 22 00 00 	addis   r9,r2,0
 4a8:	99 49 00 00 	stb     r10,0\(r9\)
 4ac:	38 7e 00 14 	addi    r3,r30,20
 4b0:	48 00 00 01 	bl      4b0 .*
 4b4:	38 7e 00 0c 	addi    r3,r30,12
 4b8:	48 00 00 01 	bl      4b8 .*
 4bc:	91 43 80 04 	stw     r10,-32764\(r3\)
 4c0:	3d 23 00 00 	addis   r9,r3,0
 4c4:	91 49 80 08 	stw     r10,-32760\(r9\)
 4c8:	81 3e 00 2c 	lwz     r9,44\(r30\)
 4cc:	7d 49 13 2e 	sthx    r10,r9,r2
 4d0:	a1 42 00 00 	lhz     r10,0\(r2\)
 4d4:	3d 22 00 00 	addis   r9,r2,0
 4d8:	a9 49 00 00 	lha     r10,0\(r9\)
Disassembly of section \.got:

00010598 <\.got>:
   10598:	4e 80 00 21 	blrl
   1059c:	00 01 04 f8 	\.long 0x104f8
	\.\.\.
