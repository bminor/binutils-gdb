#source: morello-sizeless-global-syms.s
#as: -march=morello+c64
#ld: -static -pie
#objdump: -DRz -j .data -j .bss

.*:     file format elf64-littleaarch64


Disassembly of section \.data:

[0-9a-f]+ <ptr3>:
   [0-9a-f]+:	00010440.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000002.*
   [0-9a-f]+:	00000000.*

[0-9a-f]+ <ptr2>:
   [0-9a-f]+:	00010444.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000002.*
   [0-9a-f]+:	00000000.*

[0-9a-f]+ <ptr1>:
   [0-9a-f]+:	00010448.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000002.*
   [0-9a-f]+:	00000000.*

Disassembly of section .bss:

0000000000010440 <baz>:
   [0-9a-f]+:	00000000.*

[0-9a-f]+ <bar>:
   [0-9a-f]+:	00000000.*

[0-9a-f]+ <foo>:
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
