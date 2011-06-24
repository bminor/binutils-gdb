#name: PR ld/12921
#as: --32
#ld: -melf_i386
#readelf: -S --wide

There are 7 section headers, starting at offset 0x204c:

Section Headers:
  \[Nr\] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            00000000 000000 000000 00      0   0  0
  \[ 1\] .text             PROGBITS        08049000 001000 000001 00  AX  0   0 4096
  \[ 2\] .data             PROGBITS        0804b000 002000 000020 00  WA  0   0 4096
  \[ 3\] .bss              NOBITS          0804c000 002020 010000 00  WA  0   0 4096
  \[ 4\] .shstrtab         STRTAB          00000000 002020 00002c 00      0   0  1
  \[ 5\] .symtab           SYMTAB          00000000 002164 0000c0 10      6   6  4
  \[ 6\] .strtab           STRTAB          00000000 002224 000037 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\)
  I \(info\), L \(link order\), G \(group\), T \(TLS\), E \(exclude\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)
#pass
