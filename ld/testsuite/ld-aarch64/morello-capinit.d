# This testcase is used largely to check our handling of linker script defined
# symbols.  In the general case we want to be able to access the entire output
# section following the symbol (i.e. we treat the symbol as a start symbol).
#
# There is an exception for a symbol that points into an executable section --
# we want this to have the bounds of the PCC as we see it.
#
# There is also an exception for a symbol which is *outside* of an output
# section where we have a compatibility hack to try and guess whether it
# represents the start of the next section or end of the previous section.
#
# Rather than check the PCC bounds exactly, we check that the PCC bounds are
# consistent for all symbols pointing into the text section (which allows for
# variation between aarch64-linux-gnu and aarch64-none-elf coming from an extra
# GNU_HASH section or a differing start position of the text causing a
# differing bounds action).
#source: morello-capinit.s
#as: -march=morello+c64
#ld: -static -pie -T morello-capinit.ld
#objdump: -DR -j .inspectionsection

.*:     file format .*


Disassembly of section \.inspectionsection:

[0-9a-f]+ <ct_outer_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
#record: PCC_BOUNDS
.*:	([0-9a-f]+) 	.*
.*:	04000000 	.*

#check: PCC_SIZE string tolower $PCC_BOUNDS
[0-9a-f]+ <ct_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	PCC_SIZE 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct1_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x[0-9a-f]+
.*:	00000000 	.*
.*:	PCC_SIZE 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct1_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x[0-9a-f]+
.*:	00000000 	.*
.*:	PCC_SIZE 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x[0-9a-f]+
.*:	00000000 	.*
.*:	PCC_SIZE 	.*
.*:	04000000 	.*

[0-9a-f]+ <cd_outer_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	0000000c 	.*
.*:	02000000 	.*

[0-9a-f]+ <cd_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	0000000c 	.*
.*:	02000000 	.*

[0-9a-f]+ <cd1_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000008 	.*
.*:	02000000 	.*

[0-9a-f]+ <cd1_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000004 	.*
.*:	02000000 	.*

[0-9a-f]+ <cd_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
	\.\.\.
.*:	02000000 	.*

[0-9a-f]+ <cdynamic>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	000*[1-9a-f]+[0-9]* 	.*
.*:	02000000 	.*
