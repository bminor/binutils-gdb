# Checking that when linking as standard, the BL and the GOT access both use
# the PLT.  This link does not do anything special to maintain pointer equality
# since there is no access which directly uses the address.
#
# Things this testcase checks:
#   1) Fragment of IRELATIVE relocation is PCC_START with bounds and
#      permissions of PCC.
#   2) GOT access uses IRELATIVE relocation in the PLTGOT when no direct access
#      to address is used.
#   3) BL to an IFUNC branches to a PLT stub which uses the a PLTGOT slot
#      initialised by an IRELATIVE relocation against our resolver.
#source: morello-ifunc1.s
#as: -march=morello+c64
#ld: -pie
#objdump: -DR --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: PCC_START
  0 \.[^ ]+ +[0-9a-f]+  ([0-9a-f]+)  [0-9a-f]+  [0-9a-f]+  2\*\*.
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#record: PCC_LAST_SIZE PCC_LAST_START
#...
 *[0-9]+ \.got\.plt +([0-9a-f]+)  ([0-9a-f]+)  [0-9a-f]+  [0-9a-f]+  2\*\*.
                  CONTENTS, ALLOC, LOAD, DATA
#...
Disassembly of section \.plt:

[0-9a-f]+ <\.plt>:
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
#record: PLTGOT_DEC_OFF
 *[0-9a-f]+:	.* 	ldr	c17, \[c16, #([0-9]+)\]
 *[0-9a-f]+:	.* 	add	c16, c16, #0x.*
 *[0-9a-f]+:	.* 	br	c17

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
#...

[0-9a-f]+ <foo_2>:
#...

#record: FOO_ADDR
([0-9a-f]+) <foo>:
 *[0-9a-f]+:	.* 	mov	x1, #0x2a                  	// #42
 *[0-9a-f]+:	.* 	tst	x0, x1
 *[0-9a-f]+:	.* 	b\.ne	.* <foo\+0x18>  // b\.any
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, c0, .*
 *[0-9a-f]+:	.* 	ret	c30
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, .*
 *[0-9a-f]+:	.* 	b	.* <foo\+0x14>

[0-9a-f]+ <_start>:
#check: PLTLOC string tolower $PLTADDR
#check: PLTGOTPAGE string tolower $PLTGOT_PAGE
#check: PLTGOT_DECOFF string tolower $PLTGOT_DEC_OFF
 *[0-9a-f]+:	.* 	bl	PLTLOC .*
 *[0-9a-f]+:	.* 	adrp	c0, PLTGOTPAGE .*
 *[0-9a-f]+:	.* 	ldr	c0, \[c0, #PLTGOT_DECOFF\]
 *[0-9a-f]+:	.* 	ret	c30
#...
Disassembly of section \.got\.plt:

#check: GOTLOC aarch64_page_plus_decimal_offset $PLTGOT_PAGE $PLTGOT_DEC_OFF
#check: FRAGBASE format %08x 0x$PCC_START
#check: FRAGSIZE format %08x [expr "0x$PCC_LAST_SIZE + 0x$PCC_LAST_START - 0x$PCC_START"]
#check: FOO_OFFSET format %x [expr "0x$FOO_ADDR + 1 - 0x$PCC_START"]
[0-9a-f]+ <.*>:
	\.\.\.
 *GOTLOC:	FRAGBASE 	.*
			GOTLOC: R_MORELLO_IRELATIVE	\*ABS\*\+0xFOO_OFFSET
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#...
Disassembly of section \.data:

[0-9a-f]+ <.*>:
 *[0-9a-f]+:	FRAGBASE 	.*
			[0-9a-f]+: R_MORELLO_IRELATIVE	\*ABS\*\+0xFOO_OFFSET
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#pass
