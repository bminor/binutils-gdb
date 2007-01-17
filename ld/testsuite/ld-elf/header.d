# target: *-*-linux*
# ld: -T header.t -z max-page-size=0x10000
# objdump: -hpw

#...
Program Header:
    LOAD off    0x0*0000000 vaddr 0x0*0010000 paddr 0x0*0010000 align 2..16
         filesz 0x0*00100[23][048] memsz 0x0*00100[23][048] flags rwx
#...
Sections:
Idx Name          Size      VMA       *LMA       *File off  Algn  Flags
  0 .text         0*000ffac  0*00100[78][048]  0*00100[78][048]  0*00000[78][048]  2...  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         0*0000004  0*002002[04c]  0*002002[04c]  0*001002[04c]  2...  CONTENTS, ALLOC, LOAD, DATA
