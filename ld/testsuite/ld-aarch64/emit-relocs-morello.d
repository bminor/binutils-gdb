#source: emit-relocs-morello.s
#as: -march=morello+c64
#ld: -shared
#objdump: -DR -j .text -j .data -j .got

.*:     file format .*


Disassembly of section .text:

.* <_start>:
.*:	c24...42 	ldr	c2, \[c2, #[0-9]+\]
.*:	826...42 	ldr	c2, \[x2, #[0-9]+\]
.*:	826...42 	ldr	x2, \[x2, #[0-9]+\]
.*:	826...42 	ldr	w2, \[x2, #[0-9]+\]

.* <f1>:
.*:	c24...42 	ldr	c2, \[c2, #[0-9]+\]

Disassembly of section .got:

.* <.got>:
.*:	[0-9a-f]+ 	.*
.*:	00000000 	.*

Disassembly of section .data:

.* <num>:
.*:	00000042 	.*

.* <ptr>:
.*:	[0-9a-f]+ 	.*
			.*: R_AARCH64_RELATIVE	\*ABS\*\+.*
	...

.* <cap>:
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
.*:	00000000 	.*
.*:	00000004 	.*
.*:	02000000 	.*

.* <globcap>:
	...
			.*: R_MORELLO_CAPINIT	globvar
