#PROG: objcopy
#objdump: -h
#objcopy: --set-section-flags .text=alloc,data
#name: copy with seting section flags 2
#source: bintest.s

.*: +file format .*

Sections:
Idx.*
#...
  [0-9]* .text.*
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
#...
