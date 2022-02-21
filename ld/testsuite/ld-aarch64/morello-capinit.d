# This testsuite is used largely to check our handling of linker script defined
# symbols.  In the general case we want to be able to access the entire output
# section following this symbol (i.e. we treat this symbol as a start symbol).
#
# There is an exception for a symbol that points into an executable section --
# we want this to have the bounds of the PCC as we see it.
#
# There is also an exception for a symbol which is *outside* of an output
# section where we have a compatibility hack to try and guess whether it
# represents the start of the next section or end of the previous section.
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
.*:	00000278 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000278 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct1_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x[0-9a-f]+
.*:	00000000 	.*
.*:	00000278 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct1_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x[0-9a-f]+
.*:	00000000 	.*
.*:	00000278 	.*
.*:	04000000 	.*

[0-9a-f]+ <ct_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x[0-9a-f]+
.*:	00000000 	.*
.*:	00000278 	.*
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
