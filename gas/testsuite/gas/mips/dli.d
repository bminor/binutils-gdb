#objdump: -dr
#name: MIPS dli
#as: -mips3

# Test the dli macro.

.*: +file format .*mips.*

No symbols in .*
Disassembly of section .text:
0+0000 li \$a0,0
0+0004 li \$a0,1
0+0008 li \$a0,-1
0+000c li \$a0,0x8000
0+0010 li \$a0,-32768
0+0014 lui \$a0,0x1
0+0018 lui \$a0,0x1
0+001c ori \$a0,\$a0,0xa5a5
0+0020 li \$a0,0x8000
0+0024 dsll \$a0,\$a0,0x10
0+0028 ori \$a0,\$a0,0x1234
0+002c lui \$a0,0xffff
0+0030 dsrl32 \$a1,\$a0,0x0
0+0034 lui \$a0,0xffff
0+0038 dsrl32 \$a1,\$a0,0x0
0+003c li \$a0,-1
0+0040 lui \$a0,0xf
0+0044 ori \$a0,\$a0,0xffff
0+0048 dsll \$a0,\$a0,0x10
0+004c ori \$a0,\$a0,0xffff
0+0050 dsll \$a0,\$a0,0x10
0+0054 ori \$a0,\$a0,0xffff
0+0058 lui \$a0,0x8000
0+005c ori \$a0,\$a0,0x1234
0+0060 li \$a0,-32768
0+0064 dsll \$a0,\$a0,0x10
0+0068 ori \$a0,\$a0,0x1234
0+006c dsll \$a0,\$a0,0x10
0+0070 ori \$a0,\$a0,0x5678
0+0074 lui \$a0,0x8000
0+0078 ori \$a0,\$a0,0x1234
0+007c dsll \$a0,\$a0,0x10
0+0080 ori \$a0,\$a0,0x5678
0+0084 dsll \$a0,\$a0,0x10
0+0088 li \$a0,-30875
0+008c lui \$a0,0xffff
0+0090 ori \$a0,\$a0,0x4321
...
