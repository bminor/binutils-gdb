#source: relr-data.s
#ld: -shared -z pack-relative-relocs -T relocs.ld
#readelf: -rW

Relocation section '\.rela\.dyn' at offset 0x10150 contains 6 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
0000000012340000  0000000000000403 R_AARCH64_RELATIVE                        10004
0000000012340008  0000000000000403 R_AARCH64_RELATIVE                        10008
0000000012340018  0000000000000403 R_AARCH64_RELATIVE                        12340050
0000000012340010  0000000400000101 R_AARCH64_ABS64        000000000001000c sym_global \+ 0
0000000012340030  0000000400000101 R_AARCH64_ABS64        000000000001000c sym_global \+ 0
0000000012340040  0000000300000101 R_AARCH64_ABS64        0000000000000000 sym_weak_undef \+ 0

Relocation section '\.relr\.dyn' at offset 0x101e0 contains 2 entries which relocate 3 locations:
Index: Entry            Address           Symbolic Address
0000:  0000000012340020 0000000012340020  aligned_local
0001:  0000000000000023 0000000012340028  aligned_hidden
                        0000000012340048  aligned_DYNAMIC
