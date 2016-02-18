#source: relocs-258-dyn.s
#ld: --defsym dummy=0x45034 -T aarch64.ld   -shared
#readelf: -Wr

Relocation section '\.rela\.dyn' at offset 0x8080 contains 2 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
0000000000008010  0000000000000403 R_AARCH64_RELATIVE                        800c
000000000000800c  0000000200000102 R_AARCH64_ABS32        0000000000045034 dummy \+ 0
