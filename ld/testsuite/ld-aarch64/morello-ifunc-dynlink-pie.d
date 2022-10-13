# Checking that the sharedfoo symbol is a FUNC and UNDEFINED symbol.
# It should also have 0 as its value (so that we don't make the PLT stub the
# canonical address).
# Also checking that there is a CAPINIT relocation against the sharedfoo symbol
# (i.e. that it has not been relaxed to some other relocation).
#source: morello-ifunc-dynlink.s
#as: -march=morello+c64
#ld: -pie tmpdir/morello-ifunc-shared.so
#readelf: --relocs --dyn-syms

Relocation section '\.rela\.dyn' at offset 0x.* contains 1 entry:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
[0-9a-f]+  00030000e800 R_MORELLO_CAPINIT 0000000000000000 sharedfoo \+ 0

#...
Symbol table '\.dynsym' contains [0-9]+ entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
#...
 +[0-9]+: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND sharedfoo
#pass
