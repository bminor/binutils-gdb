#objdump: -dr --prefix-addresses -mmips:4000
#as: -march=r4000 -mtune=r4000
#name: MIPS R4000 drol

# Test the drol and dror macros.

.*: +file format .*mips.*

Disassembly of section .text:
0+0000 <[^>]*> dnegu	at,a1
0+0004 <[^>]*> dsrlv	at,a0,at
0+0008 <[^>]*> dsllv	a0,a0,a1
0+000c <[^>]*> or	a0,a0,at
0+0010 <[^>]*> dnegu	at,a2
0+0014 <[^>]*> dsrlv	at,a1,at
0+0018 <[^>]*> dsllv	a0,a1,a2
0+001c <[^>]*> or	a0,a0,at
0+0020 <[^>]*> dsll	at,a0,0x1
0+0024 <[^>]*> dsrl32	a0,a0,0x1f
0+0028 <[^>]*> or	a0,a0,at
0+002c <[^>]*> dsll	at,a1,0x1
0+0030 <[^>]*> dsrl32	a0,a1,0x1f
0+0034 <[^>]*> or	a0,a0,at
0+0038 <[^>]*> dsll	at,a1,0x1f
0+003c <[^>]*> dsrl32	a0,a1,0x1
0+0040 <[^>]*> or	a0,a0,at
0+0044 <[^>]*> dsll32	at,a1,0x0
0+0048 <[^>]*> dsrl32	a0,a1,0x0
0+004c <[^>]*> or	a0,a0,at
0+0050 <[^>]*> dsll32	at,a1,0x1
0+0054 <[^>]*> dsrl	a0,a1,0x1f
0+0058 <[^>]*> or	a0,a0,at
0+005c <[^>]*> dsll32	at,a1,0x1f
0+0060 <[^>]*> dsrl	a0,a1,0x1
0+0064 <[^>]*> or	a0,a0,at
0+0068 <[^>]*> dnegu	at,a1
0+006c <[^>]*> dsllv	at,a0,at
0+0070 <[^>]*> dsrlv	a0,a0,a1
0+0074 <[^>]*> or	a0,a0,at
0+0078 <[^>]*> dnegu	at,a2
0+007c <[^>]*> dsllv	at,a1,at
0+0080 <[^>]*> dsrlv	a0,a1,a2
0+0084 <[^>]*> or	a0,a0,at
0+0088 <[^>]*> dsrl	at,a0,0x1
0+008c <[^>]*> dsll32	a0,a0,0x1f
0+0090 <[^>]*> or	a0,a0,at
0+0094 <[^>]*> dsrl	at,a1,0x1
0+0098 <[^>]*> dsll32	a0,a1,0x1f
0+009c <[^>]*> or	a0,a0,at
0+00a0 <[^>]*> dsrl	at,a1,0x1f
0+00a4 <[^>]*> dsll32	a0,a1,0x1
0+00a8 <[^>]*> or	a0,a0,at
0+00ac <[^>]*> dsrl32	at,a1,0x0
0+00b0 <[^>]*> dsll32	a0,a1,0x0
0+00b4 <[^>]*> or	a0,a0,at
0+00b8 <[^>]*> dsrl32	at,a1,0x1
0+00bc <[^>]*> dsll	a0,a1,0x1f
0+00c0 <[^>]*> or	a0,a0,at
0+00c4 <[^>]*> dsrl32	at,a1,0x1f
0+00c8 <[^>]*> dsll	a0,a1,0x1
0+00cc <[^>]*> or	a0,a0,at
	...
