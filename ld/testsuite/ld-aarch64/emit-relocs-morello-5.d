#source: emit-relocs-morello-5.s
#as: -march=morello+c64
#ld: -static
#error: .*: capability range may exceed object bounds
#error: .*: in function `_start':.*
#error: .*: dangerous relocation: unsupported relocation
