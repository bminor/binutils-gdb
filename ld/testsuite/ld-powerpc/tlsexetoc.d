#source: tlstoc.s
#as: -a64
#ld: -melf64ppc tmpdir/libtlslib.so
#objdump: -dr
#target: powerpc64*-*-*

.*: +file format elf64-powerpc

Disassembly of section \.text:

0+10000338 <_start>:
    10000338:	e8 62 80 08 	ld	r3,-32760\(r2\)
    1000033c:	60 00 00 00 	nop
    10000340:	7c 63 6a 14 	add	r3,r3,r13
    10000344:	e8 62 80 18 	ld	r3,-32744\(r2\)
    10000348:	60 00 00 00 	nop
    1000034c:	7c 63 6a 14 	add	r3,r3,r13
    10000350:	3c 6d 00 00 	addis	r3,r13,0
    10000354:	60 00 00 00 	nop
    10000358:	38 63 91 40 	addi	r3,r3,-28352
    1000035c:	3c 6d 00 00 	addis	r3,r13,0
    10000360:	60 00 00 00 	nop
    10000364:	38 63 10 00 	addi	r3,r3,4096
    10000368:	39 23 80 40 	addi	r9,r3,-32704
    1000036c:	3d 23 00 00 	addis	r9,r3,0
    10000370:	81 49 80 48 	lwz	r10,-32696\(r9\)
    10000374:	3d 2d 00 00 	addis	r9,r13,0
    10000378:	7d 49 18 2a 	ldx	r10,r9,r3
    1000037c:	3d 2d 00 00 	addis	r9,r13,0
    10000380:	a1 49 91 88 	lhz	r10,-28280\(r9\)
    10000384:	89 4d 90 60 	lbz	r10,-28576\(r13\)
    10000388:	3d 2d 00 00 	addis	r9,r13,0
    1000038c:	99 49 90 68 	stb	r10,-28568\(r9\)
