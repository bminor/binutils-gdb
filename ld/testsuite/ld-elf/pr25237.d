#ld:
#readelf: -l -W
# Address bits of these targets don't support the .bss section alignment,
# 1 << 16.
#notarget: avr-*-* h8300-*-*

#...
 +LOAD +0x0+ +0x[0-9a-f]+ +0x[0-9a-f]+ +0x0+ +0xb60000+ +RW +0x10000
#pass
