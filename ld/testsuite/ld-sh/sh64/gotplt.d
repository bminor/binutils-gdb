#source: gotplt.s
#as: --abi=32 --isa=SHmedia
#ld: -shared -mshelf32 --version-script=$srcdir/$subdir/gotplt.map
#readelf: -r
#target: sh64-*-elf

# Make sure that gotplt relocations of forced local symbols
# use the GOT.

Relocation section '\.rela\.dyn' at offset 0x29c contains 1 entries:
 Offset     Info    Type            Sym\.Value  Sym\. Name \+ Addend
000003cc  000000a5 R_SH_RELATIVE                                000002a8
