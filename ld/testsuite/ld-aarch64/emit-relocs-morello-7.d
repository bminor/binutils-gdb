# Checking that the "compatibility hack" around symbols named with _start is
# working.
#
# Use known offsets into the GOT.  Have ensured that the GOT is 12bit aligned
# in the linker script, which means we know that the lo12 relocations should
# all have a known value (assuming a constant order of entries into the GOT).
#
# Need this since a large point of this test is to demonstrate that different
# symbols with the same address end up having different valued relocations, so
# we want to ensure a direct link between the relevant relocations and their
# GOT entries.
#
# As far as the linker is concerned, each of the symbols defined in our linker
# script are associated with the output section just before them.  However, we
# add a hack for symbols which are named in the pattern __*_start so that their
# Morello bounds are given as if the symbol was associated with the section just
# after them.
#
# Hence the __text_start and __data_other symbols should end up with permissions
# for the text section.  This should mean that they have PCC bounds, and the
# value of the __data_other symbol should include an addend.  These are the
# symbols used in the first and last LDR instructions in the function.
#
# Meanwhile, the __text_other symbol should have bounds of everything remaining
# in the section *before* it (which is nothing), and the __data_start symbol
# should have bounds spanning the entire section *after* it (the .data section).
#
#source: emit-relocs-morello-7.s
#as: -march=morello+c64
#ld: -static -pie -T emit-relocs-morello-7.ld
#objdump: -DR --section-headers -j .text -j .got -j .data

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: TEXT_START
.* .text         00000010  ([0-9a-f]+)  .*  .*  2\*\*2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
#record: DATA_START
.* .data         00000004  ([0-9a-f]+)  .*  .*  2\*\*0
                  CONTENTS, ALLOC, LOAD, DATA
.* .got          00000050  0000000000001000  0000000000001000  00001000  2\*\*4
                  CONTENTS, ALLOC, LOAD, DATA

#check: TXT_START string tolower $TEXT_START
#check: DTA_START string tolower $DATA_START

Disassembly of section \.text:

TXT_START <obj>:
.*:	c2400400 	ldr	c0, \[c0, #16\]
.*:	c2401000 	ldr	c0, \[c0, #64\]
.*:	c2400c00 	ldr	c0, \[c0, #48\]
.*:	c2400800 	ldr	c0, \[c0, #32\]


Disassembly of section \.data:

DTA_START <\.data>:
.*:	0000000a 	.*

Disassembly of section \.got:

#check: PCC_START aarch64_8digit_addr $TEXT_START
#check: DAT_START aarch64_8digit_addr $DATA_START
#check: PCC_LENGTH aarch64_8digit_addr [format %x [expr "0x1050 - 0x$TEXT_START"]]
0000000000001000 <\.got>:
    1000:	[0-9a-f]+ 	.*
	\.\.\.
    1010:	PCC_START 	.*
			1010: R_MORELLO_RELATIVE	\*ABS\*
    1014:	00000000 	.*
    1018:	PCC_LENGTH 	.*
    101c:	04000000 	.*
    1020:	PCC_START 	.*
			1020: R_MORELLO_RELATIVE	\*ABS\*\+0x10
    1024:	00000000 	.*
    1028:	PCC_LENGTH 	.*
    102c:	04000000 	.*
    1030:	DAT_START 	.*
			1030: R_MORELLO_RELATIVE	\*ABS\*
    1034:	00000000 	.*
    1038:	00000004 	.*
    103c:	02000000 	.*
    1040:	PCC_START 	.*
			1040: R_MORELLO_RELATIVE	\*ABS\*
	\.\.\.
    104c:	02000000 	.*

