#as: -march=morello+c64
#ld: -static
#objdump: -x
#...
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#...
  0 \.text         00003000  [0-9a-f]+  [0-9a-f]+  [0-9a-f]+  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 \.alt          00003000  [0-9a-f]+  [0-9a-f]+  [0-9a-f]+  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
#pass


