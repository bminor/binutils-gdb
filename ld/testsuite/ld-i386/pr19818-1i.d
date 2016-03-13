#source: pr19818-1a.s
#as: --32
#ld: -shared -m elf_i386 --defsym foo=0xffffffff -z nocombreloc
#readelf: -r --wide

Relocation section '.rel.text' at offset 0x[0-9a-f]+ contains 1 entries:
 Offset     Info    Type                Sym. Value  Symbol's Name
[0-9a-f]+ +[0-9a-f]+ +R_386_32 +ffffffff +foo
