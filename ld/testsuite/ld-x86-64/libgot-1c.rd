
Global Offset Table '.got' contains 1 entry:
 Index:    Address       Reloc         Sym. Name \+ Addend/Value
 +[0-9]+: 000000200340 R_X86_64_GLOB_DAT foo \+ 0

Global Offset Table '.got.plt' contains 4 entries:
 Index:    Address       Reloc         Sym. Name \+ Addend/Value
 +[0-9]+: 000000200348                   200220
 +[0-9]+: 000000200350                   0
 +[0-9]+: 000000200358                   0
 +[0-9]+: 000000200360 R_X86_64_JUMP_SLO bar \+ 0
#pass
