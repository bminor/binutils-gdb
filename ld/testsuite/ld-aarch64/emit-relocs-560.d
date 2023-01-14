#source: emit-relocs-560.s
#ld: -shared
#readelf: -r

Relocation section '.rela.plt' at offset 0x[0-9a-f]+ contains 2 entries:
  Offset          Info           Type           Sym. Value    Sym. Name \+ Addend
[0-9a-f]+  000100000402 R_AARCH64_JUMP_SL 0000000000000000 baz \+ 0
[0-9a-f]+  000200000402 R_AARCH64_JUMP_SL 0000000000000000 bar \+ 0
