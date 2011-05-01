#name: PR ld/12718
#as: --32
#ld: -melf_i386
#readelf: -S

There are 5 section headers, starting at offset 0x7c:

Section Headers:
  \[Nr\] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  \[ 0\]                   NULL            00000000 000000 000000 00      0   0  0
  \[ 1\] .text             PROGBITS        08048054 000054 000006 00  AX  0   0  4
  \[ 2\] .shstrtab         STRTAB          00000000 00005a 000021 00      0   0  1
  \[ 3\] .symtab           SYMTAB          00000000 000144 000070 10      4   2  4
  \[ 4\] .strtab           STRTAB          00000000 0001b4 000024 00      0   0  1
Key to Flags:
  W \(write\), A \(alloc\), X \(execute\), M \(merge\), S \(strings\)
  I \(info\), L \(link order\), G \(group\), T \(TLS\), E \(exclude\), x \(unknown\)
  O \(extra OS processing required\) o \(OS specific\), p \(processor specific\)
#pass
