#source: morello-sizeless-local-syms.s
#as: -march=morello+c64
#ld: -static -pie
#objdump: -DRz -j .data -j .bss

.*:     file format elf64-littleaarch64


Disassembly of section .data:

[0-9a-f]+ <ptr3>:
#record: BSS_START
   [0-9a-f]+:	([0-9a-f]+) .*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	02000000.*

[0-9a-f]+ <ptr2>:
#check: BAR_POS format %08x [expr "0x$BSS_START + 0x4"]
   [0-9a-f]+:	BAR_POS .*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	02000000.*

[0-9a-f]+ <ptr1>:
#check: FOO_POS format %08x [expr "0x$BSS_START + 0x8"]
   [0-9a-f]+:	FOO_POS .*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	02000000.*

Disassembly of section \.bss:

#check: BAZL_POS format %016x 0x$BSS_START
#check: BARL_POS format %016x [expr "0x$BSS_START + 0x4"]
#check: FOOL_POS format %016x [expr "0x$BSS_START + 0x8"]
BAZL_POS <baz>:
   [0-9a-f]+:	00000000.*

BARL_POS <bar>:
   [0-9a-f]+:	00000000.*

FOOL_POS <foo>:
   [0-9a-f]+:	00000000.*
   [0-9a-f]+:	00000000.*
