#source: morello-sizeless-got-syms.s
#as: -march=morello+c64
#ld: -static -pie
#objdump: -DRz -j .text -j .got -j .bss

.*:     file format elf64-littleaarch64

Disassembly of section \.text:

[0-9a-f]+ <get_foo>:
 [0-9a-f]+:	90800080 	adrp	c0, 10000 .*
 [0-9a-f]+:	c240dc00 	ldr	c0, \[c0, #880\]
 [0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <get_bar>:
 [0-9a-f]+:	90800080 	adrp	c0, 10000 .*
 [0-9a-f]+:	c240e000 	ldr	c0, \[c0, #896\]
 [0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <get_baz>:
 [0-9a-f]+:	90800080 	adrp	c0, 10000 .*
 [0-9a-f]+:	c240d800 	ldr	c0, \[c0, #864\]
 [0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <_start>:
.*

Disassembly of section \.got:

0000000000010350 <\.got>:
   [0-9a-f]+:	.*
   [0-9a-f]+:	.*
   [0-9a-f]+:	.*
   [0-9a-f]+:	.*
   [0-9a-f]+:	000103c0 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	02000000 	.*
   [0-9a-f]+:	000103c8 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	02000000 	.*
   [0-9a-f]+:	000103c4 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	02000000 	.*

Disassembly of section .bss:

00000000000103c0 <baz>:
   [0-9a-f]+:	00000000 	.*

[0-9a-f]+ <bar>:
   [0-9a-f]+:	00000000 	.*

[0-9a-f]+ <foo>:
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	00000000 	.*
