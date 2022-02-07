#source: emit-relocs-morello-3.s
#as: -march=morello+c64 --defsym C64MODE=1
#ld: -shared
#objdump: -DR -j .text -j .data -j .got

.*:     file format .*


Disassembly of section .text:

.* <_start>:
.*:	90800082 	adrp	c2, [0-9a-f]+ <.*>
.*:	c24...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	c24...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	c24...42 	ldr	c2, \[c2, #[0-9]+\]

Disassembly of section .got:

.* <.got>:
.*:	[0-9a-f]+ 	.*
	...
			.*: R_MORELLO_GLOB_DAT	cap3
			.*: R_MORELLO_GLOB_DAT	cap2
			.*: R_MORELLO_GLOB_DAT	cap

Disassembly of section .data:

.* <str>:
.*:	6c6c6548 	.*
.*:	6874206f 	.*
.*:	20657265 	.*
.*:	20676962 	.*
.*:	20646162 	.*
.*:	6c726f77 	.*
.*:	00002164 	.*
.*:	00000000 	.*

.* <cap>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_CAPINIT	str
.*:	00000000 	.*
.*:	0000001b 	.*
.*:	02000000 	.*

.* <cap2>:
	...
			.*: R_MORELLO_CAPINIT	str2
