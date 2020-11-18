#name: Merge SHF_GNU_RETAIN for non-unique sections (check no unmerged)
#notarget: ![supports_gnu_osabi]
#source: section24.s
#readelf: -S --wide

#failif
#...
  \[..\] .(text|data|bss|rodata)[ 	]+PROGBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00 [^R] .*
#pass

