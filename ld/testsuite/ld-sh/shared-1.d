#source: ldr1.s
#source: ldr2.s
#as: -little
#ld: -shared -EL -z nocombreloc
#readelf: -r -x4 -x5
#target: sh*-*-elf

# Make sure relocations against global and local symbols with relative and
# absolute 32-bit relocs don't come out wrong after ld -r.  Remember that
# SH uses partial_inplace (sort-of REL within RELA) with related confusion
# about how, when, where and which addends to use.  A DSO must have the
# same value in the addend as in the data, so either can be used.

Relocation section '\.rela\.text' at offset 0x[0-9a-f]+ contains 1 entries:
.*
00000220  000000a5 R_SH_RELATIVE +00000224

Hex dump of section '\.rela\.text':
  0x0000020c          00000224 000000a5 00000220 .*

Hex dump of section '\.text':
  0x00000218          00000224 00090009 00090009 .*
