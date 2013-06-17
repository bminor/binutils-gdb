#source: ifunc-3.s
#ld: -shared
#objdump: -dw
#target: aarch64*-*-*

#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+0x[0-9a-f]+@plt>
#pass
