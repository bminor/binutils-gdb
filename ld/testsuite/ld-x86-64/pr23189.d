#as: --64  -mrelax-relocations=yes
#ld: -shared -melf_x86_64 -T pr23189.t
#readelf: -r --wide

Relocation section '.rela.dyn' at offset 0x[0-9a-f]+ contains 1 entry:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
[0-9a-f]+ +[0-9a-f]+ +R_X86_64_RELATIVE +[0-9a-f]+
