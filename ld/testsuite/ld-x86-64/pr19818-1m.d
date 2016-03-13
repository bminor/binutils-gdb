#source: pr19818-1a.s
#as: --x32
#ld: -shared -Bsymbolic -m elf32_x86_64 --defsym foo=0xffffffff
#readelf: -r --wide

There are no relocations in this file.
