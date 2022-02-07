#source: emit-relocs-morello-3.s
#as: -march=morello
#ld: -shared
#objdump: -DR -j .text -j .data -j .got

.*:     file format .*


Disassembly of section .text:

.* <_start>:
.*:	90000082 	adrp	x2, [0-9a-f]+ <.*>
.*:	826...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	826...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	826...42 	ldr	c2, \[c2, #[0-9]+\]

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
