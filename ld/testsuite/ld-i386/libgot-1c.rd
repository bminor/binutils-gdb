
Global Offset Table '.got' contains 1 entry:
 Index:  Address      Reloc        Sym. Name \+ Addend/Value
 +[0-9]+: 002001fc R_386_GLOB_DAT    foo \+ 0

Global Offset Table '.got.plt' contains 4 entries:
 Index:  Address      Reloc        Sym. Name \+ Addend/Value
 +[0-9]+: 00200200                   20016c
 +[0-9]+: 00200204                   0
 +[0-9]+: 00200208                   0
 +[0-9]+: 0020020c R_386_JUMP_SLOT   bar \+ 156
#pass
