#ld: -shared
#objdump: -dw
#target: aarch64-*-*

#...
[ \t0-9a-f]+:[ \t0-9a-f]+bl[ \t0-9a-f]+<\*ABS\*\+0x[0-9a-f]+@plt>
[ \t0-9a-f]+:[ \t0-9a-f]+adrp[ \t]+x0, 0 <_GLOBAL_OFFSET_TABLE_>
[ \t0-9a-f]+:[ \t0-9a-f]+add[ \t]+x0, x0, #0x[0-9a-f]+
#pass
