#readelf: -gr
#name: MIPS ELF o32 PIC comdat GOT16/LO16 relocation pairing
#as: -32 -mno-pdr -mips32r6
#source: comdat-reloc.s

# Make sure the orphan GOT16 relocation is paired with LO16 for a local
# symbol in a comdat section, i.e. rather than this:
#
# 00000014  00000509 R_MIPS_GOT16      00000000   foo
# 00000020  00000506 R_MIPS_LO16       00000000   foo
# 0000001c  00000509 R_MIPS_GOT16      00000000   foo
#
# we have this:
#
# 00000014  00000509 R_MIPS_GOT16      00000000   foo
# 00000024  00000509 R_MIPS_GOT16      00000000   foo
# 0000001c  00000506 R_MIPS_LO16       00000000   foo

#...
COMDAT group section \[.....\] `\.group' \[bar\] contains .+ sections:
   \[Index\]    Name
   \[.....\]   \.text\.foo
   \[.....\]   \.text\.bar
#...
Relocation section '\.rel\.text\.bar' at offset .+ contains .+ entries:
 Offset     Info    Type            Sym\.Value  Sym\. Name
00000000  ......05 R_MIPS_HI16       00000000   _gp_disp
00000004  ......06 R_MIPS_LO16       00000000   _gp_disp
0000000c  0000070a R_MIPS_PC16       00000020   \.L1\^B1
00000014  ......09 R_MIPS_GOT16      00000000   foo
00000024  ......09 R_MIPS_GOT16      00000000   foo
0000001c  ......06 R_MIPS_LO16       00000000   foo
00000020  0000080a R_MIPS_PC16       00000018   \.L0\^B1
#pass
