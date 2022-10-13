# Checking that the introduction of a CAPINIT relocation against an IFUNC does
# not change the behaviour of us requiring pointer equality, and checking that
# the CAPINIT relocation respects that pointer equality requirement by pointing
# at the PLT stub.
#source: morello-ifunc.s
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#objdump: -DR --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: PCC_START
  0 \.[^ ]* +[0-9a-f]*  ([0-9a-f]+)  .*
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#record: PCC_LAST_SIZE PCC_LAST_START
#...
 *[0-9]+ \.got\.plt *([0-9a-f]+)  ([0-9a-f]+) .*
                  CONTENTS, ALLOC, LOAD, DATA
#...
Disassembly of section \.plt:

#record: PLT0_ADDR
([0-9a-f]+) <\.plt>:
#...
.*nop
.*nop
.*nop
# Note that for a none-linux-gnu target we don't emit a symbol specific to the
# PLT stub for an IFUNC, but for a none-elf target we do.  That means we can't
# generally check the offset in the symbols name matches the `foo` address
# below.
#?
#?[0-9a-f]+ <\*ABS\*\+0x[0-9a-f]+@plt>:
#record: PLTADDR PLTGOT_PAGE
 *([0-9a-f]+):	.* 	adrp	c16, ([0-9a-f]+) .*
#record: FOO_PLTGOT_OFFSET
 *[0-9a-f]+:	.* 	ldr	c17, \[c16, #([0-9]+)\]
 *[0-9a-f]+:	.* 	add	c16, c16, #0x.*
 *[0-9a-f]+:	c2c21220 	br	c17

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
#...

[0-9a-f]+ <foo_2>:
#...

#record: FOO_ADDR
([0-9a-f]+) <foo>:
 *[0-9a-f]+:	.* 	mov	x1, #0x2a                  	// #42
 *[0-9a-f]+:	.* 	tst	x0, x1
 *[0-9a-f]+:	.* 	b\.ne	.*
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, c0, .*
 *[0-9a-f]+:	.* 	ret	c30
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, c0, .*
 *[0-9a-f]+:	.* 	b	.*

[0-9a-f]+ <_start>:
#check: PLTLOC string tolower $PLTADDR
#check: PLT_PAGE format %x [expr "0x$PLTADDR & ~0xfff"]
#check: FOO_PLT_OFF format %x [expr "(0x$PLTADDR & 0xfff) + 1"]
 *[0-9a-f]+:	.* 	bl	PLTLOC .*
#record: GOT_PAGE
 *[0-9a-f]+:	.* 	adrp	c0, ([0-9a-f]+) .*
#record: FOO_GOTOFF_DECIMAL
 *[0-9a-f]+:	.* 	ldr	c0, \[c0, #([0-9]+)\]
 *[0-9a-f]+:	.* 	adrp	c0, PLT_PAGE .*
 *[0-9a-f]+:	.* 	add	c0, c0, #0xFOO_PLT_OFF
 *[0-9a-f]+:	.* 	ret	c30

#...
Disassembly of section \.got:

#check: PLT_ADDEND  format %x [expr "0x$PLTADDR - 0x$PCC_START + 1"]
#check: FRAGBASE format %08x 0x$PCC_START
#check: FRAGSIZE format %08x [expr "0x$PCC_LAST_SIZE + 0x$PCC_LAST_START - 0x$PCC_START"]
#check: FRAGMENT_LOC  aarch64_page_plus_decimal_offset $GOT_PAGE $FOO_GOTOFF_DECIMAL
#...
 *FRAGMENT_LOC:	FRAGBASE 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*\+0xPLT_ADDEND
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*

Disassembly of section \.got\.plt:

#check: PLTGOTLOC  aarch64_page_plus_decimal_offset $PLTGOT_PAGE $FOO_PLTGOT_OFFSET
#check: FOO_OFFSET format %x [expr "0x$FOO_ADDR + 1 - 0x$PCC_START"]
[0-9a-f]+ <.*>:
	\.\.\.
 *PLTGOTLOC:	FRAGBASE 	.*
			PLTGOTLOC: R_MORELLO_IRELATIVE	\*ABS\*\+0xFOO_OFFSET
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
	\.\.\.

Disassembly of section \.data:

[0-9a-f]+ <.*>:
 *[0-9a-f]+:	FRAGBASE 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0xPLT_ADDEND
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#pass
