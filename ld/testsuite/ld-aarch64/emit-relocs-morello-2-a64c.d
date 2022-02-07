#source: emit-relocs-morello-2.s
#as: -march=morello+a64c
#ld: -shared
#objdump: -DR -j .text -j .data -j .got

.*:     file format .*


Disassembly of section .text:

.* <_start>:
.*:	90000082 	adrp	x2, [0-9a-f]+ <.*>
.*:	826...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	c24...42 	ldr	c2, \[x2, #[0-9]+\]
.*:	f94...42 	ldr	x2, \[x2, #[0-9]+\]

Disassembly of section .got:

.* <.got>:
.*:	[0-9a-f]+ 	.*
	...
			.*: R_MORELLO_GLOB_DAT	cap

Disassembly of section .data:

.* <str>:
.*:	6c6c6548 	.*
.*:	6874206f 	.*
.*:	20657265 	.*
.*:	20676962 	.*
.*:	20646162 	.*
.*:	6c726f77 	.*
.*:	.*
			.*: R_AARCH64_RELATIVE	\*ABS\*\+.*

.* <ptr>:
.*:	[0-9a-f]+ 	.*
	...

.* <cap>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	0000001b 	.*
.*:	02000000 	.*
