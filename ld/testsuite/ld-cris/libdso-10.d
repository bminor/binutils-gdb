#source: dso-1.s
#as: --pic --no-underscore --march=v32 --em=criself
#ld: --shared -m crislinux
#objdump: -p -h

# Sanity check; just an empty GOT.

.*:     file format elf32-cris

Program Header:
    LOAD off    0x0+ vaddr 0x0+ paddr 0x0+ align 2\*\*13
         filesz 0x0+160 memsz 0x0+160 flags r-x
    LOAD off    0x0+160 vaddr 0x0+2160 paddr 0x0+2160 align 2\*\*13
         filesz 0x0+64 memsz 0x0+64 flags rw-
 DYNAMIC off    0x0+160 vaddr 0x0+2160 paddr 0x0+2160 align 2\*\*2
         filesz 0x0+58 memsz 0x0+58 flags rw-
Dynamic Section:
  HASH        0x94
  STRTAB      0x134
  SYMTAB      0xc4
  STRSZ       0x28
  SYMENT      0x10
private flags = 2: \[v32\]
Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 \.hash         0+30  0+94  0+94  0+94  2\*\*2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 \.dynsym       0+70  0+c4  0+c4  0+c4  2\*\*2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 \.dynstr       0+28  0+134  0+134  0+134  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  3 \.text         0+4  0+15c  0+15c  0+15c  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  4 \.dynamic      0+58  0+2160  0+2160  0+160  2\*\*2
                  CONTENTS, ALLOC, LOAD, DATA
  5 \.got          0+c  0+21b8  0+21b8  0+1b8  2\*\*2
                  CONTENTS, ALLOC, LOAD, DATA
