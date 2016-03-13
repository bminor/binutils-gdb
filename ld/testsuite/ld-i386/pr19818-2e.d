#source: pr19818-2.s
#as: --32 -mrelax-relocations=no
#ld: -shared -Bsymbolic -m elf_i386 --defsym foo=0xffffffff
#readelf: -r --wide

There are no relocations in this file.
