#readelf: -Sr -x1 -x4
There are 11 section headers, starting at offset 0xe0:

Section Headers:
  \[Nr\] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  \[ 0\]                   NULL             0+  0+
       0+  0+           0     0     0
  \[ 1\] \.text             PROGBITS         0+  0+40
       0+4  0+  AX       0     0     4
  \[ 2\] \.data             PROGBITS         0+  0+44
       0+  0+  WA       0     0     1
  \[ 3\] \.bss              NOBITS           0+  0+44
       0+  0+  WA       0     0     1
  \[ 4\] \.MMIX\.spec_data\.2 PROGBITS         0+  0+48
       0+10  0+           0     0     8
  \[ 5\] \.rela\.MMIX\.spec_d RELA             0+  0+470
       0+30  0+18           9     4     8
  \[ 6\] \.MMIX\.spec_data\.3 PROGBITS         0+  0+58
       0+8  0+           0     0     8
  \[ 7\] \.rela\.MMIX\.spec_d RELA             0+  0+4a0
       0+18  0+18           9     6     8
  \[ 8\] \.shstrtab         STRTAB           0+  0+60
       0+7e  0+           0     0     1
  \[ 9\] \.symtab           SYMTAB           0+  0+3a0
       0+c0  0+18          10     6     8
  \[10\] \.strtab           STRTAB           0+  0+460
       0+c  0+           0     0     1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\)
  I \(info\), L \(link order\), G \(group\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)

Relocation section '\.rela\.MMIX\.spec_data\.2' at offset 0x470 contains 2 entries:
[ 	]+Offset[ 	]+Info[ 	]+Type[ 	]+Symbol's Value[ 	]+Symbol's Name[ 	]+Addend
0+  0+600000004 R_MMIX_32             0+  forw                      \+ 0
0+8  0+700000005 R_MMIX_64             0+  other                     \+ 0

Relocation section '\.rela\.MMIX\.spec_data\.3' at offset 0x4a0 contains 1 entries:
[ 	]+Offset[ 	]+Info[ 	]+Type[ 	]+Symbol's Value[ 	]+Symbol's Name[ 	]+Addend
0+  0+700000005 R_MMIX_64             0+  other                     \+ 0

Hex dump of section '\.text':
  0x00000000 fd010203                            .*

Hex dump of section '\.MMIX\.spec_data\.2':
  0x00000000 00000000 0000002a 00000000 00000000 .*
