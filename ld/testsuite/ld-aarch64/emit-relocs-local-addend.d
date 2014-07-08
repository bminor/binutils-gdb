#source: emit-relocs-local-addend-foo.s
#source: emit-relocs-local-addend-bar.s
#ld: -e0 --emit-relocs
#readelf: -r

Relocation section '\.rela\.text' at offset 0x102f8 contains 4 entries:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
000000400000  000200000113 R_AARCH64_ADR_PRE 0000000000400018 \.rodata \+ 0
000000400004  000200000115 R_AARCH64_ADD_ABS 0000000000400018 \.rodata \+ 0
00000040000c  000200000113 R_AARCH64_ADR_PRE 0000000000400018 \.rodata \+ 10
000000400010  000200000115 R_AARCH64_ADD_ABS 0000000000400018 \.rodata \+ 10

Relocation section '\.rela\.rodata' at offset 0x10358 contains 2 entries:
  Offset          Info           Type           Sym\. Value    Sym. Name \+ Addend
000000400020  000200000101 R_AARCH64_ABS64   0000000000400018 \.rodata \+ 0
000000400030  000200000101 R_AARCH64_ABS64   0000000000400018 \.rodata \+ 10
