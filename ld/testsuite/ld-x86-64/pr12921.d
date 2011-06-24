#name: PR ld/12921
#as: --64
#ld: -melf_x86_64
#readelf: -S --wide

There are 7 section headers, starting at offset 0x2058:

Section Headers:
  \[Nr\] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            0000000000000000 000000 000000 00      0   0  0
  \[ 1\] .text             PROGBITS        0000000000401000 001000 000001 00  AX  0   0 4096
  \[ 2\] .data             PROGBITS        0000000000602000 002000 000028 00  WA  0   0 4096
  \[ 3\] .bss              NOBITS          0000000000603000 002028 010000 00  WA  0   0 4096
  \[ 4\] .shstrtab         STRTAB          0000000000000000 002028 00002c 00      0   0  1
  \[ 5\] .symtab           SYMTAB          0000000000000000 002218 000120 18      6   6  8
  \[ 6\] .strtab           STRTAB          0000000000000000 002338 000037 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\), l \(large\)
  I \(info\), L \(link order\), G \(group\), T \(TLS\), E \(exclude\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)
#pass
