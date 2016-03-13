#source: pr19818-1a.s
#as: --x32 -mrelax-relocations=no
#ld: -shared -m elf32_x86_64 --defsym foo=0xffffffff -z nocombreloc
#readelf: -r --wide

Relocation section '.rela.text' at offset 0x[0-9a-f]+ contains 1 entries:
 Offset     Info    Type                Sym. Value  Symbol's Name \+ Addend
[0-9a-f]+ +[0-9a-f]+ +R_X86_64_32 +ffffffff +foo \+ 0
