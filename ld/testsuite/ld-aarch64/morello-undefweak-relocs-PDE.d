# Exercise a corner case of handling morello GOT relocations against weak undef
# symbols.
#
# What we want to check is:
#   1) The adrp/ldr insns point to the relevant entries in the GOT.
#   2) There are relocations for all except the hidden weak undef symbol in the
#      GOT (the only symbol which can never be given a value at runtime since
#      it is hidden).
#   3) The GOT entry for this weak undefined symbol is zero.
#   4) The values in .data are initialized as zero.
#   5) There are relocations to initialize the non-hidden .data variables and
#      no relocation to initialize the value of the hidden symbol.
#
# (1) is checked by ensuring that the adrp/ldr combinations point to increments
# of 0x10 past the start of the got section.
# (2) and (3) are checked by the fact that objdump prints nothing except
# relocations for the non-hidden symbols after the first line in the .got
# section.
# (4) and (5) is checked by the fact that objdump prints nothing except
# relocations for the non-hidden symbols for the data section contents.
#source: morello-undefweak-relocs.s
#as: -march=morello+c64
#ld: tmpdir/morello-weakdefinitions.so
#objdump: -DR -j .got -j .data -j .text

.*:     file format .*


Disassembly of section \.text:

.* <_start>:
#record: GOTPAGE
.* 	adrp	c0, ([0-9a-f]+) .*
#record: GOTOFFSET
.* 	ldr	c0, \[c0, #([0-9]+)\]
#check: PAGEG string tolower $GOTPAGE
#check: GOTOFF2 expr "$GOTOFFSET - 0x10"
#check: GOTOFF3 expr "$GOTOFFSET - 0x20"
.* 	adrp	c0, PAGEG .*
.* 	ldr	c0, \[c0, #GOTOFF2\]
.* 	adrp	c0, PAGEG .*
.* 	ldr	c0, \[c0, #GOTOFF3\]
.* 	ret	c30

Disassembly of section \.got:

.* <_GLOBAL_OFFSET_TABLE_>:
#check: GOTSTART format %x [expr "0x$GOTPAGE + $GOTOFFSET - 0x30"]
#check: GOTGLOBAL format %x [expr "0x$GOTPAGE + $GOTOFFSET"]
#check: GOTLOCAL format %x [expr "0x$GOTPAGE + $GOTOFFSET - 0x20"]
 *GOTSTART:	.*
	\.\.\.
			GOTLOCAL: R_MORELLO_GLOB_DAT	localsym
			GOTGLOBAL: R_MORELLO_GLOB_DAT	globalsym

Disassembly of section \.data:

.* <.*>:
	\.\.\.
			[0-9a-f]*: R_MORELLO_CAPINIT	globalsym
			[0-9a-f]*: R_MORELLO_CAPINIT	localsym
