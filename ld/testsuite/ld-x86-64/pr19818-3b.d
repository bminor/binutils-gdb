#source: pr19818-3.s
#source: pr19818-1b.s
#as: --64 -mrelax-relocations=no
#ld: -shared -m elf_x86_64 -z nocombreloc
#readelf: -r --wide

Relocation section '.rela.text' at offset 0x[0-9a-f]+ contains 1 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
[0-9a-f]+ +[0-9a-f]+ +R_X86_64_64 +0+ffffffff +foo \+ 0
