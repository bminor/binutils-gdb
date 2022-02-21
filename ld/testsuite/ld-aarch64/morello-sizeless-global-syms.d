#source: morello-sizeless-global-syms.s
#as: -march=morello+c64
#ld: -static -pie
#objdump: -DRz -j .data -j .bss

.*:     file format elf64-littleaarch64


Disassembly of section \.data:

[0-9a-f]+ <ptr3>:
   [0-9a-f]+:	00010450.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	02000000.*

[0-9a-f]+ <ptr2>:
   [0-9a-f]+:	00010454.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	02000000.*

[0-9a-f]+ <ptr1>:
   [0-9a-f]+:	00010458.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	02000000.*

Disassembly of section .bss:

0000000000010450 <baz>:
   [0-9a-f]+:	00000000.*

[0-9a-f]+ <bar>:
   [0-9a-f]+:	00000000.*

[0-9a-f]+ <foo>:
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
