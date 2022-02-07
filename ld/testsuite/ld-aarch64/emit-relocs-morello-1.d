#source: emit-relocs-morello-1.s
#as: -march=morello+c64
#ld: -pie -static
#objdump: -DR -j .got -j .data -j .text

.*:     file format .*


Disassembly of section .text:

.* <_start>:
 .*:	c240.800 	ldr	c0, \[c0, #[0-9af]+\]

Disassembly of section .got:

.* <.got>:
   .*:	[0-9a-f]+ 	.*
	...
   .*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
   .*:	00000000 	.*
   .*:	00000003 	.*
   .*:	02000000 	.*

Disassembly of section .data:

.* <val>:
   .*:	00424242 	.*
	...

.* <cap>:
   .*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
   .*:	00000000 	.*
   .*:	00000003 	.*
   .*:	02000000 	.*
