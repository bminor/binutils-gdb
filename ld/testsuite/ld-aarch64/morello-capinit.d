#source: morello-capinit.s
#as: -march=morello+c64
#ld: -static -pie -T morello-capinit.ld
#objdump: -DR -j .data

.*:     file format .*


Disassembly of section \.data:

[0-9a-f]+ <ct_outer_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000c01 	.*
.*:	00000000 	.*

[0-9a-f]+ <ct_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000c01 	.*
.*:	00000000 	.*

[0-9a-f]+ <ct1_start>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000801 	.*
.*:	00000000 	.*

[0-9a-f]+ <ct1_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000401 	.*
.*:	00000000 	.*

[0-9a-f]+ <ct_end>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000001 	.*
.*:	00000000 	.*

[0-9a-f]+ <cdynamic>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	0*[1-9a-f]+[0-9]*02 	.*
.*:	00000000 	.*
