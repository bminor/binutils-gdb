#objdump: -h
#name: section alignment

# Test that alignment works in .section directives.

.*: .*

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 \.text         00000000  00000000  00000000  00000000  2\*\*2
                  ALLOC, LOAD, READONLY, CODE
  1 \.data         00000000  00000000  00000000  00000000  2\*\*2
                  ALLOC, LOAD, DATA
  2 \.bss          00000000  00000000  00000000  00000000  2\*\*2
                  ALLOC
  3 \.none         00000000  00000000  00000000  00000000  2\*\*0
                  ALLOC, LOAD, READONLY, DATA
  4 \.zero         00000000  00000000  00000000  00000000  2\*\*0
                  ALLOC, LOAD, READONLY, DATA
  5 \.one          00000001  00000000  00000000  000001a4  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  6 \.two          00000002  00000000  00000000  000001a5  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 \.three        00000003  00000000  00000000  000001a7  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  8 \.four         00000004  00000000  00000000  000001aa  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 \.five         00000005  00000000  00000000  000001ae  2\*\*0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
