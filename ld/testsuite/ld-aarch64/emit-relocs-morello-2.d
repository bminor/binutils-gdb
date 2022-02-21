#source: emit-relocs-morello-2.s
#as: -march=morello+c64 --defsym C64MODE=1
#ld: -shared
#objdump: -DR -j .text -j .data -j .got

.*:     file format .*


Disassembly of section .text:

.* <_start>:
.*:	90800082 	adrp	c2, [0-9a-f]+ <.*>
.*:	c24...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	826...42 	ldr	c2, \[x2, #[0-9]+\]
.*:	826...42 	ldr	x2, \[x2, #[0-9]+\]

Disassembly of section .got:

.* <.got>:
.*:	[0-9a-f]+ 	.*
	...
			.*: R_MORELLO_GLOB_DAT	cap

Disassembly of section .data:

0000000000010360 <str>:
.*:	6c6c6548 	.*
.*:	6874206f 	.*
.*:	20657265 	.*
.*:	20676962 	.*
.*:	20646162 	.*
.*:	6c726f77 	.*
.*:	 	.*
			.*: R_AARCH64_RELATIVE	\*ABS\*\+.*

.* <ptr>:
.*:	00010360 	.*
	...

.* <cap>:
.*:	00010360 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	0000001b 	.*
.*:	02000000 	.*
