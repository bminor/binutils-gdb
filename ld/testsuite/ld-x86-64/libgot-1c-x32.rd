
Global Offset Table '.got' contains 1 entry:
 Index:  Address      Reloc        Sym. Name \+ Addend/Value
 +[0-9]+: 00200200 R_X86_64_GLOB_DAT foo \+ 0

Global Offset Table '.got.plt' contains 4 entries:
 Index:  Address      Reloc        Sym. Name \+ Addend/Value
 +[0-9]+: 00200208                   20016c
 +[0-9]+: 00200210                   0
 +[0-9]+: 00200218                   0
 +[0-9]+: 00200220 R_X86_64_JUMP_SLO bar \+ 0
#pass
