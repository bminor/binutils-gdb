#source: tls32.s
#as: -a32
#ld: -melf32ppc tmpdir/libtlslib32.so
#objdump: -dr
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Disassembly of section \.text:

01800264 <_start>:
 1800264:	80 7f 00 0c 	lwz     r3,12\(r31\)
 1800268:	7c 63 12 14 	add     r3,r3,r2
 180026c:	38 7f 00 10 	addi    r3,r31,16
 1800270:	48 01 01 85 	bl      18103f4 .*
 1800274:	3c 62 00 00 	addis   r3,r2,0
 1800278:	38 63 90 1c 	addi    r3,r3,-28644
 180027c:	3c 62 00 00 	addis   r3,r2,0
 1800280:	38 63 10 00 	addi    r3,r3,4096
 1800284:	39 23 80 20 	addi    r9,r3,-32736
 1800288:	3d 23 00 00 	addis   r9,r3,0
 180028c:	81 49 80 24 	lwz     r10,-32732\(r9\)
 1800290:	3d 22 00 00 	addis   r9,r2,0
 1800294:	a1 49 90 2c 	lhz     r10,-28628\(r9\)
 1800298:	89 42 90 30 	lbz     r10,-28624\(r2\)
 180029c:	3d 22 00 00 	addis   r9,r2,0
 18002a0:	99 49 90 34 	stb     r10,-28620\(r9\)
 18002a4:	3c 62 00 00 	addis   r3,r2,0
 18002a8:	38 63 90 00 	addi    r3,r3,-28672
 18002ac:	3c 62 00 00 	addis   r3,r2,0
 18002b0:	38 63 10 00 	addi    r3,r3,4096
 18002b4:	91 43 80 04 	stw     r10,-32764\(r3\)
 18002b8:	3d 23 00 00 	addis   r9,r3,0
 18002bc:	91 49 80 08 	stw     r10,-32760\(r9\)
 18002c0:	3d 22 00 00 	addis   r9,r2,0
 18002c4:	b1 49 90 2c 	sth     r10,-28628\(r9\)
 18002c8:	a1 42 90 14 	lhz     r10,-28652\(r2\)
 18002cc:	3d 22 00 00 	addis   r9,r2,0
 18002d0:	a9 49 90 18 	lha     r10,-28648\(r9\)
Disassembly of section \.got:

01810390 <_GLOBAL_OFFSET_TABLE_-0x4>:
 1810390:	4e 80 00 21 	blrl
01810394 <_GLOBAL_OFFSET_TABLE_>:
 1810394:	01 81 02 f0 00 00 00 00 00 00 00 00 00 00 00 00     .*
	\.\.\.
