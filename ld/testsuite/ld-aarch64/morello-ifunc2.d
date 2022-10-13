# Checking that having a CAPINIT relocation against an IFUNC symbol stops the
# garbage collection of the IFUNC's PLT.
# In order to show that the PLT stub is indeed the one related to FOO we show
# that the PLT stub loads from the PLTGOT, and that the entry in the PLTGOT it
# loads is initialised with an IRELATIVE relocation pointing to the foo
# resolver.
#as: -march=morello+c64
#ld:
#objdump: -Dr --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: TEXT_START
.* .[^ ]+ +[0-9a-f]+  ([0-9a-f]+)  .*  .*  2\*\*.
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#record: DATA_START
#...
.* .data         00000010  ([0-9a-f]+)  .*  .*  2\*\*.
                  CONTENTS, ALLOC, LOAD, DATA

#...
Disassembly of section \.rela\.plt:

[0-9a-f]+ <\.rela\.plt>:
#record: PLTGOT_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e804 	.*
 *[0-9a-f]+:	00000000 	.*
#record: PLTGOT_IREL_ADDEND
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
#record: CAPINIT_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e804 	.*
 *[0-9a-f]+:	00000000 	.*
#check: CAPINIT_ADDEND string tolower $PLTGOT_IREL_ADDEND
 *[0-9a-f]+:	CAPINIT_ADDEND 	.*
 *[0-9a-f]+:	00000000 	.*


Disassembly of section \.plt:

#check: PLTGOT_PAGE format %x [expr "0x$PLTGOT_LOC & (~0xfff)"]
#check: PLTGOT_DEC_OFF  expr "0x$PLTGOT_LOC & 0xfff"
[0-9a-f]+ <\.plt>:
 *[0-9a-f]+:	........ 	adrp	c16, PLTGOT_PAGE <.*
 *[0-9a-f]+:	........ 	ldr	c17, \[c16, #PLTGOT_DEC_OFF\]
 *[0-9a-f]+:	........ 	add	c16, c16, #0x.*
 *[0-9a-f]+:	c2c21220 	br	c17
#clearcheck:

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
 *[0-9a-f]+:	52800020 	mov	w0, #0x1                   	// #1
 *[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <foo_2>:
 *[0-9a-f]+:	52800040 	mov	w0, #0x2                   	// #2
 *[0-9a-f]+:	c2c253c0 	ret	c30

#check: FOO_LOC format %016x [expr "0x$TEXT_START + 0x$PLTGOT_IREL_ADDEND - 1"]
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

#check: PLTGOT_LOCATION format %x 0x$PLTGOT_LOC
#check: PCC_START aarch64_8digit_addr $TEXT_START
#check: PCC_SIZE  format %08x [expr "0x$DATA_START - 0x$TEXT_START"]
[0-9a-f]+ <.*>:
 *PLTGOT_LOCATION:	PCC_START 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	PCC_SIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#pass
