#as: -march=morello+c64
#ld: -static -T morello-sec-round-adjust.ld
#objdump: -x
#...
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#...
  0 \.text         00000109  0000000000000200  [0-9a-f]+  [0-9a-f]+  2\*\*9
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 \.data\.rel\.ro  00100200  0000000000000400  0000000000000400  00000400  2\*\*9
                  CONTENTS, ALLOC, LOAD, DATA
  2 \.got          00000030  0000000000100600  0000000000100600  00100600  2\*\*4
                  CONTENTS, ALLOC, LOAD, DATA
  3 \.got\.plt      00000030  0000000000100630  0000000000100630  00100630  2\*\*4
                  CONTENTS, ALLOC, LOAD, DATA
  4 \.rela\.dyn     000001a0  0000000000100660  0000000000100660  00100660  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#pass

