#source: tls32.s
#as: -a32
#ld: -shared -melf32ppc
#objdump: -dr
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Disassembly of section \.text:

0+498 <_start>:
 498:	38 7f 00 1c 	addi    r3,r31,28
 49c:	48 00 00 01 	bl      49c .*
 4a0:	38 7f 00 0c 	addi    r3,r31,12
 4a4:	48 00 00 01 	bl      4a4 .*
 4a8:	38 7f 00 24 	addi    r3,r31,36
 4ac:	48 01 01 95 	bl      10640 .*
 4b0:	38 7f 00 0c 	addi    r3,r31,12
 4b4:	48 01 01 8d 	bl      10640 .*
 4b8:	39 23 80 20 	addi    r9,r3,-32736
 4bc:	3d 23 00 00 	addis   r9,r3,0
 4c0:	81 49 80 24 	lwz     r10,-32732\(r9\)
 4c4:	81 3f 00 2c 	lwz     r9,44\(r31\)
 4c8:	7d 49 12 2e 	lhzx    r10,r9,r2
 4cc:	89 42 00 00 	lbz     r10,0\(r2\)
 4d0:	3d 22 00 00 	addis   r9,r2,0
 4d4:	99 49 00 00 	stb     r10,0\(r9\)
 4d8:	38 7e 00 14 	addi    r3,r30,20
 4dc:	48 00 00 01 	bl      4dc .*
 4e0:	38 7e 00 0c 	addi    r3,r30,12
 4e4:	48 00 00 01 	bl      4e4 .*
 4e8:	91 43 80 04 	stw     r10,-32764\(r3\)
 4ec:	3d 23 00 00 	addis   r9,r3,0
 4f0:	91 49 80 08 	stw     r10,-32760\(r9\)
 4f4:	81 3e 00 2c 	lwz     r9,44\(r30\)
 4f8:	7d 49 13 2e 	sthx    r10,r9,r2
 4fc:	a1 42 00 00 	lhz     r10,0\(r2\)
 500:	3d 22 00 00 	addis   r9,r2,0
 504:	a9 49 00 00 	lha     r10,0\(r9\)
Disassembly of section \.got:

000105c4 <\.got>:
   105c4:	4e 80 00 21 	blrl
   105c8:	00 01 05 24 	\.long 0x10524
	\.\.\.
