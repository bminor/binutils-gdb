#source: ifunc-3.s
#ld: -shared --hash-style=sysv
#objdump: -dw
#target: aarch64*-*-*

#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+(0x1e0|0x230)@plt>
#pass
