#source: start.s
#as: --x32
#ld: -m elf32_x86_64 -shared --no-ld-generated-unwind-info
#readelf: -d -S --wide

There are 10 section headers, starting at offset 0x22c:

Section Headers:
  \[Nr\] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            00000000 000000 000000 00      0   0  0
  \[ 1\] .hash             HASH            00000094 000094 000030 04   A  2   0  4
  \[ 2\] .dynsym           DYNSYM          000000c4 0000c4 000070 10   A  3   2  4
  \[ 3\] .dynstr           STRTAB          00000134 000134 00001d 00   A  0   0  1
  \[ 4\] .rela.dyn         RELA            00000154 000154 00000c 0c   A  2   0  4
  \[ 5\] .text             PROGBITS        00000160 000160 000005 00  AX  0   0  4
  \[ 6\] .dynamic          DYNAMIC         00200168 000168 000078 08  WA  3   0  4
  \[ 7\] .shstrtab         STRTAB          00000000 0001e0 00004a 00      0   0  1
  \[ 8\] .symtab           SYMTAB          00000000 0003bc 0000e0 10      9   9  4
  \[ 9\] .strtab           STRTAB          00000000 00049c 000043 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\), l \(large\)
  I \(info\), L \(link order\), G \(group\), T \(TLS\), E \(exclude\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)

Dynamic section at offset 0x168 contains 10 entries:
  Tag        Type                         Name/Value
 0x00000004 \(HASH\)                       0x94
 0x00000005 \(STRTAB\)                     0x134
 0x00000006 \(SYMTAB\)                     0xc4
 0x0000000a \(STRSZ\)                      29 \(bytes\)
 0x0000000b \(SYMENT\)                     16 \(bytes\)
 0x00000007 \(RELA\)                       0x154
 0x00000008 \(RELASZ\)                     12 \(bytes\)
 0x00000009 \(RELAENT\)                    12 \(bytes\)
 0x00000016 \(TEXTREL\)                    0x0
 0x00000000 \(NULL\)                       0x0
