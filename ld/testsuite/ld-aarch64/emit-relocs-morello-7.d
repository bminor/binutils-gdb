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
# Point here is that the __text_start and __data_other symbols should end up
# pointing into the text section, which should mean that they have PCC bounds
# and the value of the __data_other symbol should include an addend.  These are
# the symbols used in the first and last LDR's in the function.
#
# Meanwhile, the __text_other symbol should have bounds of everything remaining
# in the section *before* it (which is nothing), and the __data_start symbol
# should have bounds spanning the entire section *after* it (which happens to
# be the .interp section).
#
#source: emit-relocs-morello-7.s
#as: -march=morello+c64
#ld: -static -pie -T emit-relocs-morello-7.ld
#objdump: -DR -j .text -j .data -j .got

.*:     file format .*


Disassembly of section \.text:

0000000000000230 <obj>:
.*:	c2400400 	ldr	c0, \[c0, #16\]
.*:	c2401000 	ldr	c0, \[c0, #64\]
.*:	c2400c00 	ldr	c0, \[c0, #48\]
.*:	c2400800 	ldr	c0, \[c0, #32\]

Disassembly of section \.data:

.* <\.data>:
.*:	0000000a 	.*

Disassembly of section \.got:

0000000000001000 <\.got>:
    1000:	[0-9a-f]+ 	.*
	\.\.\.
    1010:	00000230 	.*
			1010: R_MORELLO_RELATIVE	\*ABS\*
    1014:	00000000 	.*
    1018:	00000e20 	.*
    101c:	04000000 	.*
    1020:	00000230 	.*
			1020: R_MORELLO_RELATIVE	\*ABS\*\+0x10
    1024:	00000000 	.*
    1028:	00000e20 	.*
    102c:	04000000 	.*
    1030:	00000240 	.*
			1030: R_MORELLO_RELATIVE	\*ABS\*
    1034:	00000000 	.*
    1038:	0000000d 	.*
    103c:	01000000 	.*
    1040:	00000230 	.*
			1040: R_MORELLO_RELATIVE	\*ABS\*
	\.\.\.
    104c:	02000000 	.*

