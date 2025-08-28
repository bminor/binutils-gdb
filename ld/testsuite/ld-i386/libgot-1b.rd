ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 \(current\)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN \(Shared object file\)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          52 \(bytes into file\)
  Start of section headers:          768 \(bytes into file\)
  Flags:                             0x0
  Size of this header:               52 \(bytes\)
  Size of program headers:           32 \(bytes\)
  Number of program headers:         4
  Size of section headers:           40 \(bytes\)
  Number of section headers:         14
  Section header string table index: 13

Section Headers:
  \[Nr\] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            00000000 000000 000000 00      0   0  0
  \[ 1\] .hash             HASH            000000b4 0000b4 000024 04   A  2   0  4
  \[ 2\] .dynsym           DYNSYM          000000d8 0000d8 000040 10   A  3   1  4
  \[ 3\] .dynstr           STRTAB          00000118 000118 00000e 00   A  0   0  1
  \[ 4\] .rel.dyn          REL             00000128 000128 000008 08   A  2   0  4
  \[ 5\] .rel.plt          REL             00000130 000130 000008 08  AI  2  10  4
  \[ 6\] .plt              PROGBITS        00000140 000140 000020 04  AX  0   0 16
  \[ 7\] .text             PROGBITS        00000160 000160 00000b 00  AX  0   0  1
  \[ 8\] .dynamic          DYNAMIC         0020016c 00016c 000090 08  WA  3   0  4
  \[ 9\] .got              PROGBITS        002001fc 0001fc 000004 04  WA  0   0  4
  \[10\] .got.plt          PROGBITS        00200200 000200 000010 04  WA  0   0  4
  \[11\] .symtab           SYMTAB          00000000 000210 000060 10     12   3  4
  \[12\] .strtab           STRTAB          00000000 000270 00002d 00      0   0  1
  \[13\] .shstrtab         STRTAB          00000000 00029d 000060 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\), I \(info\),
  L \(link order\), O \(extra OS processing required\), G \(group\), T \(TLS\),
  C \(compressed\), x \(unknown\), o \(OS specific\), E \(exclude\),
  D \(mbind\), p \(processor specific\)

There are no section groups in this file.

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000000 0x00000000 0x00000000 0x0016b 0x0016b R E 0x200000
  LOAD           0x00016c 0x0020016c 0x0020016c 0x000a4 0x000a4 RW  0x200000
  DYNAMIC        0x00016c 0x0020016c 0x0020016c 0x00090 0x00090 RW  0x4
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x10

 Section to Segment mapping:
  Segment Sections...
   00     .hash .dynsym .dynstr .rel.dyn .rel.plt .plt .text 
   01     .dynamic .got .got.plt 
   02     .dynamic 
   03     

Dynamic section at offset 0x16c contains 13 entries:
  Tag        Type                         Name/Value
 0x00000004 \(HASH\)                       0xb4
 0x00000005 \(STRTAB\)                     0x118
 0x00000006 \(SYMTAB\)                     0xd8
 0x0000000a \(STRSZ\)                      14 \(bytes\)
 0x0000000b \(SYMENT\)                     16 \(bytes\)
 0x00000003 \(PLTGOT\)                     0x200200
 0x00000002 \(PLTRELSZ\)                   8 \(bytes\)
 0x00000014 \(PLTREL\)                     REL
 0x00000017 \(JMPREL\)                     0x130
 0x00000011 \(REL\)                        0x128
 0x00000012 \(RELSZ\)                      8 \(bytes\)
 0x00000013 \(RELENT\)                     8 \(bytes\)
 0x00000000 \(NULL\)                       0x0

Relocation section '.rel.dyn' at offset 0x128 contains 1 entry:
 Offset     Info    Type                Sym. Value  Symbol's Name
0+2001fc  00000206 R_386_GLOB_DAT         00000000   foo

Relocation section '.rel.plt' at offset 0x130 contains 1 entry:
 Offset     Info    Type                Sym. Value  Symbol's Name
0+20020c  00000307 R_386_JUMP_SLOT        00000000   bar
No processor specific unwind information to decode

Symbol table '.dynsym' contains 4 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
 +[a-f0-9]+: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
 +[a-f0-9]+: 00000160     0 FUNC    GLOBAL DEFAULT    7 func
 +[a-f0-9]+: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND foo
 +[a-f0-9]+: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND bar

Symbol table '.symtab' contains 6 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
 +[a-f0-9]+: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
 +[a-f0-9]+: 0020016c     0 OBJECT  LOCAL  DEFAULT    8 _DYNAMIC
 +[a-f0-9]+: 00200200     0 OBJECT  LOCAL  DEFAULT   10 _GLOBAL_OFFSET_TABLE_
 +[a-f0-9]+: 00000160     0 FUNC    GLOBAL DEFAULT    7 func
 +[a-f0-9]+: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND foo
 +[a-f0-9]+: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND bar

Histogram for bucket list length \(total of 3 buckets\):
 Length  Number     % of total  Coverage
      0  1          \( 33.3%\)
      1  1          \( 33.3%\)     33.3%
      2  1          \( 33.3%\)    100.0%

No version information found in this file.

Global Offset Table '.got' contains 1 entry:
   Index:  Address      Reloc             Sym. Name \+ Addend/Value
 +[0-9]+: 002001fc R_386_GLOB_DAT         foo \+ 0

Global Offset Table '.got.plt' contains 4 entries:
   Index:  Address      Reloc             Sym. Name \+ Addend/Value
 +[0-9]+: 00200200                        20016c
 +[0-9]+: 00200204                        0
 +[0-9]+: 00200208                        0
 +[0-9]+: 0020020c R_386_JUMP_SLOT        bar \+ 156
#pass
