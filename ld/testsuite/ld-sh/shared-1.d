#source: ldr1.s
#source: ldr2.s
#as: -little
#ld: -shared -EL
#readelf: -r -x4 -x5
#target: sh*-*-elf sh*-*-linux*

# Make sure relocations against global and local symbols with relative and
# absolute 32-bit relocs don't come out wrong after ld -r.  Remember that
# SH uses partial_inplace (sort-of REL within RELA) with related confusion
# about how, when, where and which addends to use.  A DSO must have the
# same value in the addend as in the data, so either can be used.

Relocation section '\.rela\.text' at offset 0x32c contains 1 entries:
 Offset     Info    Type            Symbol's Value  Symbol's Name          Addend
00000340  000000a5 R_SH_RELATIVE                                          00000344

Hex dump of section '\.rela\.text':
  0x0000032c          00000344 000000a5 00000340 .*

Hex dump of section '\.text':
  0x00000338          00000344 00090009 00090009 .*
