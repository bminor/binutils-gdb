#as: -32
#objdump: -dr --prefix-addresses
#name: MIPS mul
#source: mul.s

# Test the mul macro.

.*: +file format .*mips.*

Disassembly of section \.text:
0+0000 <[^>]*> mul	a0,a0,a1
0+0004 <[^>]*> mul	a0,a1,a2
0+0008 <[^>]*> li	at,0
0+000c <[^>]*> mult	a1,at
0+0010 <[^>]*> mflo	a0
0+0014 <[^>]*> li	at,1
0+0018 <[^>]*> mult	a1,at
0+001c <[^>]*> mflo	a0
0+0020 <[^>]*> li	at,0x8000
0+0024 <[^>]*> mult	a1,at
0+0028 <[^>]*> mflo	a0
0+002c <[^>]*> li	at,-32768
0+0030 <[^>]*> mult	a1,at
0+0034 <[^>]*> mflo	a0
0+0038 <[^>]*> lui	at,0x1
0+003c <[^>]*> mult	a1,at
0+0040 <[^>]*> mflo	a0
0+0044 <[^>]*> lui	at,0x1
0+0048 <[^>]*> ori	at,at,0xa5a5
0+004c <[^>]*> mult	a1,at
0+0050 <[^>]*> mflo	a0
0+0054 <[^>]*> mult	a0,a1
0+0058 <[^>]*> mflo	a0
0+005c <[^>]*> sra	a0,a0,0x1f
0+0060 <[^>]*> mfhi	at
0+0064 <[^>]*> beq	a0,at,0+70 <foo\+(0x|)70>
0+0068 <[^>]*> nop
0+006c <[^>]*> break	(0x0,0x6|0x6)
0+0070 <[^>]*> mflo	a0
0+0074 <[^>]*> mult	a1,a2
0+0078 <[^>]*> mflo	a0
0+007c <[^>]*> sra	a0,a0,0x1f
0+0080 <[^>]*> mfhi	at
0+0084 <[^>]*> beq	a0,at,0+90 <foo\+(0x|)90>
0+0088 <[^>]*> nop
0+008c <[^>]*> break	(0x0,0x6|0x6)
0+0090 <[^>]*> mflo	a0
0+0094 <[^>]*> multu	a0,a1
0+0098 <[^>]*> mfhi	at
0+009c <[^>]*> mflo	a0
0+00a0 <[^>]*> beqz	at,0+ac <foo\+(0x|)ac>
0+00a4 <[^>]*> nop
0+00a8 <[^>]*> break	(0x0,0x6|0x6)
0+00ac <[^>]*> multu	a1,a2
0+00b0 <[^>]*> mfhi	at
0+00b4 <[^>]*> mflo	a0
0+00b8 <[^>]*> beqz	at,0+c4 <foo\+(0x|)c4>
0+00bc <[^>]*> nop
0+00c0 <[^>]*> break	(0x0,0x6|0x6)
0+00c4 <[^>]*> dmul	a0,a1,a2
0+00c8 <[^>]*> li	at,1
0+00cc <[^>]*> dmult	a1,at
0+00d0 <[^>]*> mflo	a0
0+00d4 <[^>]*> dmult	a1,a2
0+00d8 <[^>]*> mflo	a0
0+00dc <[^>]*> dsra32	a0,a0,0x1f
0+00e0 <[^>]*> mfhi	at
0+00e4 <[^>]*> beq	a0,at,0+f0 <foo\+(0x|)f0>
0+00e8 <[^>]*> nop
0+00ec <[^>]*> break	(0x0,0x6|0x6)
0+00f0 <[^>]*> mflo	a0
0+00f4 <[^>]*> dmultu	a1,a2
0+00f8 <[^>]*> mfhi	at
0+00fc <[^>]*> mflo	a0
0+0100 <[^>]*> beqz	at,0+10c <foo\+(0x|)10c>
0+0104 <[^>]*> nop
0+0108 <[^>]*> break	(0x0,0x6|0x6)
	\.\.\.
