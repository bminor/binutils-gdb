#target: [check_shared_lib_support]
#as: -march=morello+c64 --defsym C64MODE=1
#ld: -shared --hash-style=sysv
#objdump: -dw
#source: ifunc-2.s

#record: INDIRECT_LOC FOO_LOCATION
#...
0+([0-9a-f]{3}).*0x([0-9a-f]{3})@plt>:
#...
Disassembly of section \.text:

#check: FOO_LOC string tolower $FOO_LOCATION
#check: INDIRECT_POS format %x [expr "0x$INDIRECT_LOC + 1"]
0+FOO_LOC <foo>:
#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+0xFOO_LOC@plt>
[ \t0-9a-f]+:[ \t0-9a-f]+adrp[ \t]+c0, 0 <.*>
[ \t0-9a-f]+:[ \t0-9a-f]+add[ \t]+c0, c0, #0xINDIRECT_POS
#pass
