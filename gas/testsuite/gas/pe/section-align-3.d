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
  3 \.none         00000000  00000000  00000000  00000000  2\*\*3
                  ALLOC, LOAD, READONLY, DATA
  4 \.zero         00000000  00000000  00000000  00000000  2\*\*3
                  ALLOC, LOAD, READONLY, DATA
  5 \.one          00000008  00000000  00000000  0000026c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  6 \.two          00000008  00000000  00000000  00000274  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 \.three        00000008  00000000  00000000  0000027c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  8 \.four         00000008  00000000  00000000  00000284  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 \.five         00000008  00000000  00000000  0000028c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 10 \.six          00000008  00000000  00000000  00000294  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 11 \.seven        00000008  00000000  00000000  0000029c  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 12 \.eight        00000008  00000000  00000000  000002a4  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 13 \.nine         00000010  00000000  00000000  000002ac  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 14 \.ten          00000010  00000000  00000000  000002bc  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
