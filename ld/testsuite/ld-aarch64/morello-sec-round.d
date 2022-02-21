#as: -march=morello+c64
#ld: -static -pie -T morello-sec-round.ld
#objdump: -x
#...
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#...
  1 .text_sec     00020040  ...........20000  [0-9a-f]+  [0-9a-f]+  2\*\*17
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 \..*       [0-9a-f]+  ...........40040  [0-9a-f]+  [0-9a-f]+  .*
#pass
