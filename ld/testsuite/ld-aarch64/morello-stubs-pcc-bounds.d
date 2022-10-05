# The purpose of this test is to check the interaction between the code
# to ensure exact PCC bounds and the code that inserts stubs. We want to
# make sure we still get tight PCC bounds even when stubs are requried.
#
# For the PCC bounds, we now add padding *after* the end of the last
# section in PCC bounds (instead of padding the last section itself).
# Hence we check that the PCC span ends where the .tail_relro section
# starts, rather than checking where .text_high ends.
#source: morello-stubs-pcc-bounds.s
#as: -march=morello+c64
#ld: -static -T morello-stubs-pcc-bounds.ld
#objdump: -Dr --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 \.text_low     00000030  00000000000000c0  00000000000000c0  000000c0  2\*\*5
#...
[0-9a-f]+ <__baz_a64c64_veneer>:
#...
[0-9a-f]+ <__foo_c64a64_veneer>:
#...
Disassembly of section \.tail_relro:

#record: TAIL_RELRO_START
([0-9a-f]+) <ptr>:
#check: PCC_SIZE format %08x [expr "0x$TAIL_RELRO_START - 0xc0"]
   20040:	000000c0 	.*
   20044:	00000000 	.*
   20048:	PCC_SIZE 	.*
   2004c:	04000000 	.*
