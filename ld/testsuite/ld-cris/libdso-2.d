#source: dso-1.s
#source: dso-2.s
#as: --pic --no-underscore
#ld: --shared -m crislinux --version-script $srcdir/$subdir/hide1
#readelf: -S -s -r

# Use "dsofn" from dso-1 in a GOTPLT reloc, but hide it in a
# version script.  This will change the incoming GOTPLT reloc to
# instead be a (local) GOT reloc.  There are no other .rela.got
# entries.  This formerly SEGV:ed because .rela.got was created
# too late to have it mapped to an output section.

There are 15 section headers.*
#...
  \[ 1\] \.hash             HASH            [0-9a-f]+ [0-9a-f]+ 00004c 04   A  2   0  4
  \[ 2\] \.dynsym           DYNSYM          [0-9a-f]+ [0-9a-f]+ 0000e0 10   A  3   c  4
  \[ 3\] \.dynstr           STRTAB          [0-9a-f]+ [0-9a-f]+ 000014 00   A  0   0  1
  \[ 4\] \.gnu\.version      VERSYM          [0-9a-f]+ [0-9a-f]+ 00001c 02   A  2   0  2
  \[ 5\] \.gnu\.version_d    VERDEF          [0-9a-f]+ [0-9a-f]+ 000038 00   A  3   2  4
  \[ 6\] \.rela\.got         RELA            [0-9a-f]+ [0-9a-f]+ 00000c 0c   A  2   a  4
  \[ 7\] \.text             PROGBITS        [0-9a-f]+ [0-9a-f]+ 000014 00  AX  0   0  1
  \[ 8\] \.data             PROGBITS        [0-9a-f]+ [0-9a-f]+ 000000 00  WA  0   0  1
  \[ 9\] \.dynamic          DYNAMIC         [0-9a-f]+ [0-9a-f]+ 000088 08  WA  3   0  4
  \[10\] \.got              PROGBITS        [0-9a-f]+ [0-9a-f]+ 000010 04  WA  0   0  4
  \[11\] \.bss              NOBITS          [0-9a-f]+ [0-9a-f]+ 000008 00  WA  0   0  1
  \[12\] \.shstrtab         STRTAB          [0-9a-f]+ [0-9a-f]+ 000071 00      0   0  1
  \[13\] \.symtab           SYMTAB          [0-9a-f]+ [0-9a-f]+ 000170 10     14  15  4
  \[14\] \.strtab           STRTAB          [0-9a-f]+ [0-9a-f]+ 00004c 00      0   0  1
#...
Relocation section '\.rela\.got' at offset 0x[0-9a-f]+ contains 1 entries:
#...
000022f4  0000000c R_CRIS_RELATIVE                              00000234
#...
Symbol table '\.dynsym' contains 14 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 0+     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    1 
     2: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    2 
     3: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    3 
     4: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    4 
     5: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    5 
     6: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    6 
     7: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    7 
     8: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    8 
     9: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    9 
    10: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   10 
    11: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   11 
    12: 0+     0 OBJECT  GLOBAL DEFAULT  ABS TST1
    13: 0+238     0 FUNC    GLOBAL DEFAULT    7 export_1@@TST1

Symbol table '\.symtab' contains 23 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 0+     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    1 
     2: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    2 
     3: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    3 
     4: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    4 
     5: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    5 
     6: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    6 
     7: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    7 
     8: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    8 
     9: [0-9a-f]+     0 SECTION LOCAL  DEFAULT    9 
    10: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   10 
    11: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   11 
    12: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   12 
    13: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   13 
    14: [0-9a-f]+     0 SECTION LOCAL  DEFAULT   14 
    15: 0+2260     0 OBJECT  LOCAL  DEFAULT  ABS _DYNAMIC
    16: 0+22f8     0 NOTYPE  LOCAL  DEFAULT  ABS __bss_start
    17: 0+22f8     0 NOTYPE  LOCAL  DEFAULT  ABS _edata
    18: 0+22e8     0 OBJECT  LOCAL  DEFAULT  ABS _GLOBAL_OFFSET_TABLE_
    19: 0+2300     0 NOTYPE  LOCAL  DEFAULT  ABS _end
    20: 0+234     0 FUNC    LOCAL  DEFAULT    7 dsofn
    21: 0+     0 OBJECT  GLOBAL DEFAULT  ABS TST1
    22: 0+238     0 FUNC    GLOBAL DEFAULT    7 export_1
