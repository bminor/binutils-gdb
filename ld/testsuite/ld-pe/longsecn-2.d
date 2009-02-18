#name: PE-COFF Long section names (enabled)
#ld: --enable-long-section-names
#objdump: -h
#source: longsecn.s

.*:     file format .*

Sections:
Idx Name          Size      VMA +LMA +File off  Algn
  0 \.text         [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 \.text\.very\.long\.section\.name [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, READONLY, CODE, DATA
  2 \.data         [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  3 \.rodata       [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  4 \.data\.very\.long\.section [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  5 \.rodata\.very.long\.section [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
  6 \.idata        [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  [0-9a-fA-F]+  2\*\*[0-9]
                  CONTENTS, ALLOC, LOAD, DATA
