#readelf: -Ssr -x1 -x4

There are 9 section headers, starting at offset 0xa0:

Section Headers:
  \[Nr\] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  \[ 0\]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  \[ 1\] \.text             PROGBITS         0000000000000000  00000040
       0000000000000004  0000000000000000  AX       0     0     4
  \[ 2\] \.data             PROGBITS         0000000000000000  00000044
       0000000000000000  0000000000000000  WA       0     0     1
  \[ 3\] \.bss              NOBITS           0000000000000000  00000044
       0000000000000000  0000000000000000  WA       0     0     1
  \[ 4\] \.MMIX\.spec_data\.2 PROGBITS         0000000000000000  00000044
       0000000000000004  0000000000000000           0     0     4
  \[ 5\] \.rela\.MMIX\.spec_d RELA             0000000000000000  00000378
       0000000000000018  0000000000000018           7     4     8
  \[ 6\] \.shstrtab         STRTAB           0000000000000000  00000048
       0000000000000055  0000000000000000           0     0     1
  \[ 7\] \.symtab           SYMTAB           0000000000000000  000002e0
       0000000000000090  0000000000000018           8     5     8
  \[ 8\] \.strtab           STRTAB           0000000000000000  00000370
       0000000000000006  0000000000000000           0     0     1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\)
  I \(info\), L \(link order\), G \(group\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)

Relocation section '\.rela\.MMIX\.spec_data\.2' at offset 0x378 contains 1 entries:
[ 	]+Offset[ 	]+Info[ 	]+Type[ 	]+Symbol's Value[ 	]+Symbol's Name[ 	]+Addend
0+  0+500000004 R_MMIX_32             0+  forw                      \+ 0

Symbol table '\.symtab' contains 6 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0+     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0+     0 SECTION LOCAL  DEFAULT    1 
     2: 0+     0 SECTION LOCAL  DEFAULT    2 
     3: 0+     0 SECTION LOCAL  DEFAULT    3 
     4: 0+     0 SECTION LOCAL  DEFAULT    4 
     5: 0+     0 NOTYPE  GLOBAL DEFAULT  UND forw

Hex dump of section '\.text':
  0x00000000 fd010203                            .*

Hex dump of section '\.MMIX\.spec_data\.2':
  0x00000000 00000000                            .*
