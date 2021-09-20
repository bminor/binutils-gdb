#source: emit-relocs-morello-4.s
#as: -march=morello+c64
#ld: -static
#readelf: --relocs --syms


#...
Relocation section[^\n]*contains 1 entry:
#...
Symbol table[^\n]*contains[^\n]*:
   Num:    Value          Size.*
#...
    [0-9]+: [0-9a-f]+0 64016 [^\n]* bigarray
#pass
