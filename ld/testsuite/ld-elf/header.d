# target: *-*-linux*
# ld: -T header.t -z max-page-size=0x10000
# objdump: -hpw

#...
Program Header:
    LOAD off    0x0*0000000 vaddr 0x0*0010000 paddr 0x0*0010000 align 2..16
         filesz 0x0*001002[48] memsz 0x0*001002[48] flags rwx
#...
Sections:
Idx Name          Size      VMA       *LMA       *File off  Algn  Flags
  0 .text         0*000ffac  0*001007[48]  0*001007[48]  0*000007[48]  2...  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         0*0000004  0*002002[04]  0*002002[04]  0*001002[04]  2...  CONTENTS, ALLOC, LOAD, DATA
