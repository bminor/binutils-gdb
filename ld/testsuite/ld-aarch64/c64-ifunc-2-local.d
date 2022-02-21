#target: [check_shared_lib_support]
#as: -march=morello+c64 --defsym C64MODE=1
#ld: -shared --hash-style=sysv
#objdump: -dw
#source: ifunc-2-local.s

#...
0+(110|180|1a0|1b8) <__GI_foo>:
#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+0x(110|180|1a0|1b8)@plt>
[ \t0-9a-f]+:[ \t0-9a-f]+adrp[ \t]+c0, 0 <.*>
[ \t0-9a-f]+:[ \t0-9a-f]+add[ \t]+c0, c0, #0x(101|171|191|1a9)
#pass
