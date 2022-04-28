# This testcase ensures that we have the expected number and type of
# relocations in our resultant binary.
#source: morello-dynamic-link-rela-dyn.s
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#readelf: --relocs --dyn-sym

Relocation section '\.rela\.dyn' at offset .* contains 6 entries:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00010000e801 R_MORELLO_GLOB_DA 0000000000000000 undefweakval \+ 0
.*  00020000e801 R_MORELLO_GLOB_DA 0000000000000000 var \+ 0

Symbol table '\.dynsym' contains [34] entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND undefweakval
     2: 0000000000000000     0 OBJECT  GLOBAL DEFAULT  UND var
#pass
