#source: ifunc-3.s
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared --hash-style=sysv
#objdump: -dw

#...
Disassembly of section \.text:

#record: INDIRECT_FOO
([0-9a-f]+) <__GI_foo>:
#check: FOO_POS format %x 0x$INDIRECT_FOO
#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+0xFOO_POS@plt>
#pass
