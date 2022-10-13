#source: ifunc-3.s
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared --hash-style=sysv
#objdump: -dw --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn  Flags
#record: PCC_START
  0 \.[^ ]* *[0-9a-f]+  ([0-9a-f]+)  [0-9a-f]+  [0-9a-f]+  2\*\*[0-9]  CONTENTS, ALLOC, LOAD, READONLY, DATA
#...
Disassembly of section \.plt:

#record: INDIRECT_LOC FOO_LOCATION
#...
0+([0-9a-f]{3}).*0x([0-9a-f]+)@plt>:
#...
Disassembly of section \.text:

#check: FOO_LOC string tolower $FOO_LOCATION
#check: FOO_ADDR format %016x [expr "0x$PCC_START + 0x$FOO_LOCATION & ~1"]
#check: INDIRECT_POS string tolower $INDIRECT_LOC
FOO_ADDR <__GI_foo>:
#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl\tINDIRECT_POS <\*ABS\*\+0xFOO_LOC@plt>
#pass
