ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 \(current\)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN \(Shared object file\)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          64 \(bytes into file\)
  Start of section headers:          1160 \(bytes into file\)
  Flags:                             0x0
  Size of this header:               64 \(bytes\)
  Size of program headers:           56 \(bytes\)
  Number of program headers:         4
  Size of section headers:           64 \(bytes\)
  Number of section headers:         14
  Section header string table index: 13

Section Headers:
  \[Nr\] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            0000000000000000 000000 000000 00      0   0  0
  \[ 1\] .hash             HASH            0000000000000120 000120 000024 04   A  2   0  8
  \[ 2\] .dynsym           DYNSYM          0000000000000148 000148 000060 18   A  3   1  8
  \[ 3\] .dynstr           STRTAB          00000000000001a8 0001a8 00000e 00   A  0   0  1
  \[ 4\] .rela.dyn         RELA            00000000000001b8 0001b8 000018 18   A  2   0  8
  \[ 5\] .rela.plt         RELA            00000000000001d0 0001d0 000018 18  AI  2  10  8
  \[ 6\] .plt              PROGBITS        00000000000001f0 0001f0 000020 10  AX  0   0 16
  \[ 7\] .text             PROGBITS        0000000000000210 000210 00000b 00  AX  0   0  1
  \[ 8\] .dynamic          DYNAMIC         0000000000200220 000220 000120 10  WA  3   0  8
  \[ 9\] .got              PROGBITS        0000000000200340 000340 000008 08  WA  0   0  8
  \[10\] .got.plt          PROGBITS        0000000000200348 000348 000020 08  WA  0   0  8
  \[11\] .symtab           SYMTAB          0000000000000000 000368 000090 18     12   3  8
  \[12\] .strtab           STRTAB          0000000000000000 0003f8 00002d 00      0   0  1
  \[13\] .shstrtab         STRTAB          0000000000000000 000425 000062 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\), I \(info\),
  L \(link order\), O \(extra OS processing required\), G \(group\), T \(TLS\),
  C \(compressed\), x \(unknown\), o \(OS specific\), E \(exclude\),
  D \(mbind\), l \(large\), p \(processor specific\)

There are no section groups in this file.

Program Headers:
  Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align
  LOAD           0x000000 0x0000000000000000 0x0000000000000000 0x00021b 0x00021b R E 0x200000
  LOAD           0x000220 0x0000000000200220 0x0000000000200220 0x000148 0x000148 RW  0x200000
  DYNAMIC        0x000220 0x0000000000200220 0x0000000000200220 0x000120 0x000120 RW  0x8
  GNU_STACK      0x000000 0x0000000000000000 0x0000000000000000 0x000000 0x000000 RW  0x10

 Section to Segment mapping:
  Segment Sections...
   00     .hash .dynsym .dynstr .rela.dyn .rela.plt .plt .text 
   01     .dynamic .got .got.plt 
   02     .dynamic 
   03     

Dynamic section at offset 0x220 contains 13 entries:
  Tag        Type                         Name/Value
 0x0000000000000004 \(HASH\)               0x120
 0x0000000000000005 \(STRTAB\)             0x1a8
 0x0000000000000006 \(SYMTAB\)             0x148
 0x000000000000000a \(STRSZ\)              14 \(bytes\)
 0x000000000000000b \(SYMENT\)             24 \(bytes\)
 0x0000000000000003 \(PLTGOT\)             0x200348
 0x0000000000000002 \(PLTRELSZ\)           24 \(bytes\)
 0x0000000000000014 \(PLTREL\)             RELA
 0x0000000000000017 \(JMPREL\)             0x1d0
 0x0000000000000007 \(RELA\)               0x1b8
 0x0000000000000008 \(RELASZ\)             24 \(bytes\)
 0x0000000000000009 \(RELAENT\)            24 \(bytes\)
 0x0000000000000000 \(NULL\)               0x0

Relocation section '.rela.dyn' at offset 0x1b8 contains 1 entry:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
0+200340  0000000200000006 R_X86_64_GLOB_DAT      0000000000000000 foo \+ 0

Relocation section '.rela.plt' at offset 0x1d0 contains 1 entry:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
0+200360  0000000300000007 R_X86_64_JUMP_SLOT     0000000000000000 bar \+ 0
No processor specific unwind information to decode

Symbol table '.dynsym' contains 4 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
 +[a-f0-9]+: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
 +[a-f0-9]+: 0000000000000210     0 FUNC    GLOBAL DEFAULT    7 func
 +[a-f0-9]+: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND foo
 +[a-f0-9]+: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND bar

Symbol table '.symtab' contains 6 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
 +[a-f0-9]+: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
 +[a-f0-9]+: 0000000000200220     0 OBJECT  LOCAL  DEFAULT    8 _DYNAMIC
 +[a-f0-9]+: 0000000000200348     0 OBJECT  LOCAL  DEFAULT   10 _GLOBAL_OFFSET_TABLE_
 +[a-f0-9]+: 0000000000000210     0 FUNC    GLOBAL DEFAULT    7 func
 +[a-f0-9]+: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND foo
 +[a-f0-9]+: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND bar

Histogram for bucket list length \(total of 3 buckets\):
 Length  Number     % of total  Coverage
      0  1          \( 33.3%\)
      1  1          \( 33.3%\)     33.3%
      2  1          \( 33.3%\)    100.0%

No version information found in this file.

Global Offset Table '.got' contains 1 entry:
   Index:      Address          Reloc             Sym. Name \+ Addend/Value
 +[0-9]+: 0000000000200340 R_X86_64_GLOB_DAT      foo \+ 0

Global Offset Table '.got.plt' contains 4 entries:
   Index:      Address          Reloc             Sym. Name \+ Addend/Value
 +[0-9]+: 0000000000200348                        200220
 +[0-9]+: 0000000000200350                        0
 +[0-9]+: 0000000000200358                        0
 +[0-9]+: 0000000000200360 R_X86_64_JUMP_SLOT     bar \+ 0
#pass
