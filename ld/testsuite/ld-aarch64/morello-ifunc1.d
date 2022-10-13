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
#as: -march=morello+c64
#ld:
#objdump: -Dr --section-headers

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

Disassembly of section \.rela\.plt:

[0-9a-f]+ <.*>:
#record: PLTGOT_LOC
 +[0-9a-f]+:	([0-9a-f]+) .*
 +[0-9a-f]+:	00000000 .*
 +[0-9a-f]+:	0000e804 .*
 +[0-9a-f]+:	00000000 .*
#record: IREL_ADDEND
 +[0-9a-f]+:	([0-9a-f]+) .*
 +[0-9a-f]+:	00000000 .*
#record: CHERICAP_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e804 	.*
 *[0-9a-f]+:	00000000 	.*
#check: CHERICAP_ADDEND string tolower $IREL_ADDEND
 *[0-9a-f]+:	CHERICAP_ADDEND 	.*
 *[0-9a-f]+:	00000000 	.*

Disassembly of section \.plt:

#check: PLTGOT_PAGE format %x [expr "0x$PLTGOT_LOC & (~0xfff)"]
#check: PLTGOT_DEC_OFF  expr "0x$PLTGOT_LOC & 0xfff"
[0-9a-f]+ <\.plt>:
#record: PLTADDR
 *([0-9a-f]+):	.* 	adrp	c16, PLTGOT_PAGE .*
 *[0-9a-f]+:	.* 	ldr	c17, \[c16, #PLTGOT_DEC_OFF\]
 *[0-9a-f]+:	.* 	add	c16, c16, #0x.*
 *[0-9a-f]+:	.* 	br	c17

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
#...

[0-9a-f]+ <foo_2>:
#...

#check: FOO_ADDR format %016x [expr "0x$PCC_START + (0x$IREL_ADDEND & ~1)"]
FOO_ADDR <foo>:
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
 *[0-9a-f]+:	.* 	bl	PLTLOC .*
 *[0-9a-f]+:	.* 	adrp	c0, PLTGOT_PAGE .*
 *[0-9a-f]+:	.* 	ldr	c0, \[c0, #PLTGOT_DEC_OFF\]
 *[0-9a-f]+:	.* 	ret	c30

Disassembly of section \.got:

[0-9a-f]+ <.*>:
	\.\.\.

Disassembly of section \.got\.plt:

#check: GOTLOC format %x 0x$PLTGOT_LOC
#check: FRAGBASE format %08x 0x$PCC_START
#check: FRAGSIZE format %08x [expr "0x$PCC_LAST_SIZE + 0x$PCC_LAST_START - 0x$PCC_START"]
[0-9a-f]+ <.*>:
	\.\.\.
 *GOTLOC:	FRAGBASE 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#...
Disassembly of section \.data:

#check: CHERICAPLOC format %x 0x$CHERICAP_LOC
[0-9a-f]+ <.*>:
 *CHERICAPLOC:	FRAGBASE 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#pass

