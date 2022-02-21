#source: emit-relocs-morello-8.s
#source: emit-relocs-morello-8b.s
#as: -march=morello+c64
#ld: -static -pie
#objdump: -DR -j .data.rel.ro
#warning: .*relocation R_MORELLO_CAPINIT against symbol 'absolute_sym' in section without permission flags '\*ABS\*'\.  Assuming Read-Write\.

.*:     file format .*


Disassembly of section \.data\.rel\.ro:

0.* <\.LC1>:
   .*:	00001000 	udf	#4096
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   .*:	00000000 	udf	#0
   .*:	00000004 	udf	#4
   .*:	02000000 	add	c0, c0, #0x0
