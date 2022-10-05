# The purpose of this test is to check the interaction between the code
# to ensure that an individual section gets precise, representable
# bounds and the code that inserts stubs.
#
# In the case of this test, .text_low should get tight bounds, and
# the .tail_relro section should start exactly where the PCC bounds
# end.
#source: morello-stubs-sec-bounds.s
#as: -march=morello+c64
#ld: -static -T morello-stubs-sec-bounds.ld
#objdump: -Dr --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 \.text_low     00100200  0000000000000400  0000000000000400  00000400  2\*\*10
#record: TAIL_RELRO_VMA
#...
  5 \.tail_relro   00000010  ([0-9a-f]+)  .*
#...
[0-9a-f]+ <__baz_a64c64_veneer>:
#...
[0-9a-f]+ <__foo_c64a64_veneer>:
#...
Disassembly of section \.tail_relro:

#check: PCC_SIZE format %08x [expr "0x$TAIL_RELRO_VMA - 0x400"]
[0-9a-f]+ <ptr>:
  [0-9a-f]+:	00000400 	.*
  [0-9a-f]+:	00000000 	.*
  [0-9a-f]+:	PCC_SIZE 	.*
  [0-9a-f]+:	04000000 	.*
