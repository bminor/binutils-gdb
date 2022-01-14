#as: --64
#ld: -melf_x86_64 -shared -z relro -z now --hash-style=sysv -z max-page-size=0x1000 -z noseparate-code $NO_DT_RELR_LDFLAGS
#readelf: -S -l --wide
#target: x86_64-*-linux*

There are 17 section headers, starting at offset 0x101228:

Section Headers:
  \[Nr\] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            0000000000000000 000000 000000 00      0   0  0
  \[ 1\] .hash             HASH            0000000000000120 000120 000018 04   A  2   0  8
  \[ 2\] .dynsym           DYNSYM          0000000000000138 000138 000048 18   A  3   1  8
  \[ 3\] .dynstr           STRTAB          0000000000000180 000180 00000a 00   A  0   0  1
  \[ 4\] .rela.dyn         RELA            0000000000000190 000190 000018 18   A  2   0  8
  \[ 5\] .plt              PROGBITS        00000000000001b0 0001b0 000010 10  AX  0   0 16
  \[ 6\] .plt.got          PROGBITS        00000000000001c0 0001c0 000008 08  AX  0   0  8
  \[ 7\] .text             PROGBITS        00000000000001c8 0001c8 00000c 00  AX  0   0  1
  \[ 8\] .eh_frame         PROGBITS        00000000000001d8 0001d8 00006c 00   A  0   0  8
  \[ 9\] .init_array       INIT_ARRAY      00000000000ffff8 0ffff8 000004 08  WA  0   0  8
  \[10\] .fini_array       FINI_ARRAY      0000000000100000 100000 000100 08  WA  0   0 1048576
  \[11\] .dynamic          DYNAMIC         0000000000100100 100100 000150 10  WA  3   0  8
  \[12\] .got              PROGBITS        0000000000100250 100250 000020 08  WA  0   0  8
  \[13\] .data             PROGBITS        0000000000101000 101000 000100 00  WA  0   0  1
  \[14\] .symtab           SYMTAB          0000000000000000 101100 000078 18     15   3  8
  \[15\] .strtab           STRTAB          0000000000000000 101178 000029 00      0   0  1
  \[16\] .shstrtab         STRTAB          0000000000000000 1011a1 000080 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\), I \(info\),
  L \(link order\), O \(extra OS processing required\), G \(group\), T \(TLS\),
  C \(compressed\), x \(unknown\), o \(OS specific\), E \(exclude\),
  D \(mbind\), l \(large\), p \(processor specific\)

Elf file type is DYN \(Shared object file\)
Entry point 0x0
There are 4 program headers, starting at offset 64

Program Headers:
  Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align
  LOAD           0x000000 0x0000000000000000 0x0000000000000000 0x000244 0x000244 R E 0x1000
  LOAD           0x0ffff8 0x00000000000ffff8 0x00000000000ffff8 0x001108 0x001108 RW  0x100000
  DYNAMIC        0x100100 0x0000000000100100 0x0000000000100100 0x000150 0x000150 RW  0x8
  GNU_RELRO      0x0ffff8 0x00000000000ffff8 0x00000000000ffff8 0x001008 0x001008 R   0x1

 Section to Segment mapping:
  Segment Sections...
   00     .hash .dynsym .dynstr .rela.dyn .plt .plt.got .text .eh_frame 
   01     .init_array .fini_array .dynamic .got .data 
   02     .dynamic 
   03     .init_array .fini_array .dynamic .got 
#pass
