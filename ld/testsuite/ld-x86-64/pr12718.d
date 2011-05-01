#name: PR ld/12718
#as: --64
#ld: -melf_x86_64
#readelf: -S --wide

There are 5 section headers, starting at offset 0xa0:

Section Headers:
  \[Nr\] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            0000000000000000 000000 000000 00      0   0  0
  \[ 1\] .text             PROGBITS        0000000000400078 000078 000006 00  AX  0   0  4
  \[ 2\] .shstrtab         STRTAB          0000000000000000 00007e 000021 00      0   0  1
  \[ 3\] .symtab           SYMTAB          0000000000000000 0001e0 0000a8 18      4   2  8
  \[ 4\] .strtab           STRTAB          0000000000000000 000288 000024 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\), l \(large\)
  I \(info\), L \(link order\), G \(group\), T \(TLS\), E \(exclude\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)
#pass
