#as: -march=morello+c64
#ld: -static -T morello-sec-round-adjust.ld
#objdump: -x
#...
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#...
  0 \.text         00000101  0000000000000078  [0-9a-f]+  [0-9a-f]+  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 \.data\.rel\.ro  00100200  [0-9a-f]+  [0-9a-f]+  [0-9a-f]+  2\*\*9
                  CONTENTS, ALLOC, LOAD, DATA
#...
  3 \.rela\.dyn     00000018  [0-9a-f]+  [0-9a-f]+  [0-9a-f]+  2\*\*3
#pass

