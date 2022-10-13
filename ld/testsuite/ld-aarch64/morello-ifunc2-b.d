# Checking that having a CAPINIT relocation against an IFUNC symbol stops the
# garbage collection of the IFUNC's PLT.
# In order to show that the PLT stub is indeed the one related to FOO we show
# that the PLT stub loads from the PLTGOT, and that the entry in the PLTGOT it
# loads is initialised with an IRELATIVE relocation pointing to the foo
# resolver.
#source: morello-ifunc2.s
#as: -march=morello+c64
#ld: -pie
#objdump: -DR --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: TEXT_START
.* .interp       [0-9a-f]+  ([0-9a-f]+)  .*  .*  2\*\*.
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#record: DATA_START
#...
.* .data         00000010  ([0-9a-f]+)  .*  .*  2\*\*.
                  CONTENTS, ALLOC, LOAD, DATA

#...
Disassembly of section \.plt:

[0-9a-f]+ <\.plt>:
#...
 *[0-9a-f]+:	d503201f 	nop
 *[0-9a-f]+:	d503201f 	nop
 *[0-9a-f]+:	d503201f 	nop

#record: PLTSTUB_ADDR FOO_OFF
([0-9a-f]+) <\*ABS\*\+0x([0-9a-f]+)@plt>:
#record: PLTGOT_PAGE
 *[0-9a-f]+:	........ 	adrp	c16, ([0-9a-f]+) <.*
#record: FOO_PLTGOT_OFF
 *[0-9a-f]+:	........ 	ldr	c17, \[c16, #([0-9]+)\]
#check: FOO_PLTGOT_HEXOFF format %x $FOO_PLTGOT_OFF
 *[0-9a-f]+:	........ 	add	c16, c16, #0xFOO_PLTGOT_HEXOFF
 *[0-9a-f]+:	c2c21220 	br	c17
#clearcheck:

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
 *[0-9a-f]+:	52800020 	mov	w0, #0x1                   	// #1
 *[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <foo_2>:
 *[0-9a-f]+:	52800040 	mov	w0, #0x2                   	// #2
 *[0-9a-f]+:	c2c253c0 	ret	c30

#check: FOO_LOC format %016x [expr "0x$TEXT_START + 0x$FOO_OFF - 1"]
FOO_LOC <foo>:
 *[0-9a-f]+:	d2800541 	mov	x1, #0x2a                  	// #42
 *[0-9a-f]+:	ea01001f 	tst	x0, x1
 *[0-9a-f]+:	........ 	b\.ne	.*
 *[0-9a-f]+:	........ 	adrp	c0, .*
 *[0-9a-f]+:	........ 	add	c0, c0, .*
 *[0-9a-f]+:	c2c253c0 	ret	c30
 *[0-9a-f]+:	........ 	adrp	c0, .*
 *[0-9a-f]+:	........ 	add	c0, c0, .*
 *[0-9a-f]+:	17fffffd 	b	.* <foo\+0x14>

[0-9a-f]+ <_start>:
 *[0-9a-f]+:	c2c253c0 	ret	c30

#...
Disassembly of section \.got\.plt:
#check: PCC_START aarch64_8digit_addr $TEXT_START
#check: PCC_SIZE  format %08x [expr "0x$DATA_START - 0x$TEXT_START"]
#check: FOO_OFFSET string tolower $FOO_OFF

[0-9a-f]+ <.*>:
	\.\.\.
 *[0-9a-f]+:	PCC_START 	.*
			.*: R_MORELLO_IRELATIVE	\*ABS\*\+0xFOO_OFFSET
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	PCC_SIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#pass
