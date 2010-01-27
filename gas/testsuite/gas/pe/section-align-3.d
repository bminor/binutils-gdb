#objdump: -h
#name: section alignment

# Test that alignment works in .section directives.

.*: .*

Sections:
Idx Name          *Size      *VMA       *LMA       *File off  *Algn
  0 \.text         0*0000000  0*0000000  0*0000000  0*0000000  2\*\*[24]
                  ALLOC, LOAD, READONLY, CODE
  1 \.data         0*0000000  0*0000000  0*0000000  0*0000000  2\*\*[24]
                  ALLOC, LOAD, DATA
  2 \.bss          0*0000000  0*0000000  0*0000000  0*0000000  2\*\*[24]
                  ALLOC
  3 \.none         0*0000000  0*0000000  0*0000000  0*0000000  2\*\*3
                  ALLOC, LOAD, READONLY, DATA
  4 \.zero         0*0000000  0*0000000  0*0000000  0*0000000  2\*\*3
                  ALLOC, LOAD, READONLY, DATA
  5 \.one          0*0000008  0*0000000  0*0000000  0*000026c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  6 \.two          0*0000008  0*0000000  0*0000000  0*0000274  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 \.three        0*0000008  0*0000000  0*0000000  0*000027c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  8 \.four         0*0000008  0*0000000  0*0000000  0*0000284  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 \.five         0*0000008  0*0000000  0*0000000  0*000028c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 10 \.six          0*0000008  0*0000000  0*0000000  0*0000294  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 11 \.seven        0*0000008  0*0000000  0*0000000  0*000029c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 12 \.eight        0*0000008  0*0000000  0*0000000  0*00002a4  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 13 \.nine         0*0000010  0*0000000  0*0000000  0*00002ac  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 14 \.ten          0*0000010  0*0000000  0*0000000  0*00002bc  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
