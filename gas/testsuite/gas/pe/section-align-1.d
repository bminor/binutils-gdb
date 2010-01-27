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
  3 \.none         0*0000000  0*0000000  0*0000000  0*0000000  2\*\*0
                  ALLOC, LOAD, READONLY, DATA
  4 \.zero         0*0000000  0*0000000  0*0000000  0*0000000  2\*\*0
                  ALLOC, LOAD, READONLY, DATA
  5 \.one          0*0000001  0*0000000  0*0000000  0*00001a4  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  6 \.two          0*0000002  0*0000000  0*0000000  0*00001a5  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 \.three        0*0000003  0*0000000  0*0000000  0*00001a7  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  8 \.four         0*0000004  0*0000000  0*0000000  0*00001aa  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 \.five         0*0000005  0*0000000  0*0000000  0*00001ae  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
