# Checking that:
#    1) Text references entries in the GOT which correspond to the relevant
#       symbols.
#    2) The size requested for those capabilities in the GOT is zero (matching
#       the size of the symbols).
#source: morello-sizeless-got-syms.s
#as: -march=morello+c64
#ld: -static -pie
#objdump: -DRz -j .text -j .got -j .bss

.*:     file format elf64-littleaarch64

Disassembly of section \.text:

[0-9a-f]+ <get_foo>:
#record: GOT_PAGE
 [0-9a-f]+:	........ 	adrp	c0, ([0-9a-f]+) .*
#record: FOO_OFFSET
 [0-9a-f]+:	........ 	ldr	c0, \[c0, #([0-9]+)\]
 [0-9a-f]+:	c2c253c0 	ret	c30

#check: GOTPAGE string tolower $GOT_PAGE
[0-9a-f]+ <get_bar>:
 [0-9a-f]+:	........ 	adrp	c0, GOTPAGE .*
#record: BAR_OFFSET
 [0-9a-f]+:	........ 	ldr	c0, \[c0, #([0-9]+)\]
 [0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <get_baz>:
 [0-9a-f]+:	........ 	adrp	c0, GOTPAGE .*
#record: BAZ_OFFSET
 [0-9a-f]+:	........ 	ldr	c0, \[c0, #([0-9]+)\]
 [0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <_start>:
.*

Disassembly of section \.got:

#check: FOO_GOTPOS aarch64_page_plus_decimal_offset $GOT_PAGE $FOO_OFFSET
#check: BAR_GOTPOS aarch64_page_plus_decimal_offset $GOT_PAGE $BAR_OFFSET
#check: BAZ_GOTPOS aarch64_page_plus_decimal_offset $GOT_PAGE $BAZ_OFFSET
[0-9a-f]+ <\.got>:
 +[0-9a-f]+:	.*
 +[0-9a-f]+:	.*
 +[0-9a-f]+:	.*
 +[0-9a-f]+:	.*
#record: BAZPOS
 +BAZ_GOTPOS:	([0-9a-f]+) 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
 +[0-9a-f]+:	00000000 	.*
 +[0-9a-f]+:	00000000 	.*
 +[0-9a-f]+:	02000000 	.*
#record: FOOPOS
 +FOO_GOTPOS:	([0-9a-f]+) 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
 +[0-9a-f]+:	00000000 	.*
 +[0-9a-f]+:	00000000 	.*
 +[0-9a-f]+:	02000000 	.*
#record: BARPOS
 +BAR_GOTPOS:	([0-9a-f]+) 	.*
			[0-9a-f]+: R_MORELLO_RELATIVE	\*ABS\*
 +[0-9a-f]+:	00000000 	.*
 +[0-9a-f]+:	00000000 	.*
 +[0-9a-f]+:	02000000 	.*

Disassembly of section .bss:

#check: FOO_LONG_POS format %016x 0x$FOOPOS
#check: BAR_LONG_POS format %016x 0x$BARPOS
#check: BAZ_LONG_POS format %016x 0x$BAZPOS
BAZ_LONG_POS <baz>:
   [0-9a-f]+:	00000000 	.*

BAR_LONG_POS <bar>:
   [0-9a-f]+:	00000000 	.*

FOO_LONG_POS <foo>:
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	00000000 	.*
