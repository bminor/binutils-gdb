#as: -march=morello+c64
#ld: -static -T morello-sec-round-adjust.ld
#objdump: -x
#...
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#...
  0 \.text         00000100  0000000000000080  [0-9a-f]+  [0-9a-f]+  2\*\*4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 \.data\.rel\.ro  00008000  0000000000000180  [0-9a-f]+  [0-9a-f]+  2\*\*4
                  CONTENTS, ALLOC, LOAD, DATA
  2 \.got          00000030  0000000000008180  [0-9a-f]+  [0-9a-f]+  2\*\*4
                  CONTENTS, ALLOC, LOAD, DATA
  3 \.got\.plt      00000030  00000000000081b0  [0-9a-f]+  [0-9a-f]+  2\*\*4
                  CONTENTS, ALLOC, LOAD, DATA
  4 \.rela\.dyn     000000b8  00000000000081e0  [0-9a-f]+  [0-9a-f]+  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#pass


