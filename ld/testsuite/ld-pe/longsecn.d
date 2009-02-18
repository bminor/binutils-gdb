#name: PE-COFF Long section names (default)
#ld:
#objdump: -h
#source: longsecn.s

.*:     file format .*

Sections:
Idx Name          Size      VMA +LMA +File off  Algn
  0 \.text         [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 \.text\.ve      [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, READONLY, CODE, DATA
  2 \.data         [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  3 \.rodata       [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  4 \.data\.ve      [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  5 \.rodata\.      [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  6 \.idata        [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA

