#target: [check_shared_lib_support]
#as: -march=morello+c64 --defsym C64MODE=1
#ld: -shared --hash-style=sysv
#objdump: -dw
#source: ifunc-2.s

#...
0+(130|1a0|1c8|1e0) <foo>:
#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+0x(130|1a0|1c8|1e0)@plt>
[ \t0-9a-f]+:[ \t0-9a-f]+adrp[ \t]+c0, 0 <.*>
[ \t0-9a-f]+:[ \t0-9a-f]+add[ \t]+c0, c0, #0x(120|190|1b8|1d0)
#pass
