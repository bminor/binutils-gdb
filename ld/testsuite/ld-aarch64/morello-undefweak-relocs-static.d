# Exercise handling morello relocations against weak undef symbols.
#
# What we want to check is:
#   1) The adrp/ldr instructions point to entries in the GOT.
#   2) There are no relocations in the GOT.
#   3) The GOT entries for weak undefined symbols are zero.
#   4) The values in .data are zero.
#
# (1) is checked by ensuring that the adrp/ldr combinations point to increments
# of 0x10 past the start of the got section.
# (3) is checked by the fact that objdump prints for the .got section.
# (4) is checked by the fact that objdump prints nothing for the data section
# contents.
#
# Since objdump refuses to print out dynamic relocations for non-dynamic
# executables, we check (2) in a separate testcase.
#source: morello-undefweak-relocs.s
#as: -march=morello+c64
#ld: -static
#objdump: -D -j .got -j .data -j .text


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

#check: GOTSTART format %x [expr "0x$GOTPAGE + $GOTOFFSET - 0x30"]
0*GOTSTART <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.

Disassembly of section \.data:

.* <.*>:
	\.\.\.
