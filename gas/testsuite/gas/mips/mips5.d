#objdump: -dr --prefix-addresses --show-raw-insn -M reg-names=numeric
#name: MIPS mips5 instructions
#stderr: mips5.l

# Check MIPS V instruction assembly

.*: +file format .*mips.*

Disassembly of section \.text:
0+0000 <[^>]*> 46c01045 	abs\.ps	\$f1,\$f2
0+0004 <[^>]*> 46c520c0 	add\.ps	\$f3,\$f4,\$f5
0+0008 <[^>]*> 4c68399e 	alnv\.ps	\$f6,\$f7,\$f8,\$3
0+000c <[^>]*> 46ca4832 	c\.eq\.ps	\$f9,\$f10
0+0010 <[^>]*> 46cc5a32 	c\.eq\.ps	\$fcc2,\$f11,\$f12
0+0014 <[^>]*> 46ca4830 	c\.f\.ps	\$f9,\$f10
0+0018 <[^>]*> 46cc5a30 	c\.f\.ps	\$fcc2,\$f11,\$f12
0+001c <[^>]*> 46ca483e 	c\.le\.ps	\$f9,\$f10
0+0020 <[^>]*> 46cc5a3e 	c\.le\.ps	\$fcc2,\$f11,\$f12
0+0024 <[^>]*> 46ca483c 	c\.lt\.ps	\$f9,\$f10
0+0028 <[^>]*> 46cc5a3c 	c\.lt\.ps	\$fcc2,\$f11,\$f12
0+002c <[^>]*> 46ca483d 	c\.nge\.ps	\$f9,\$f10
0+0030 <[^>]*> 46cc5a3d 	c\.nge\.ps	\$fcc2,\$f11,\$f12
0+0034 <[^>]*> 46ca483b 	c\.ngl\.ps	\$f9,\$f10
0+0038 <[^>]*> 46cc5a3b 	c\.ngl\.ps	\$fcc2,\$f11,\$f12
0+003c <[^>]*> 46ca4839 	c\.ngle\.ps	\$f9,\$f10
0+0040 <[^>]*> 46cc5a39 	c\.ngle\.ps	\$fcc2,\$f11,\$f12
0+0044 <[^>]*> 46ca483f 	c\.ngt\.ps	\$f9,\$f10
0+0048 <[^>]*> 46cc5a3f 	c\.ngt\.ps	\$fcc2,\$f11,\$f12
0+004c <[^>]*> 46ca4836 	c\.ole\.ps	\$f9,\$f10
0+0050 <[^>]*> 46cc5a36 	c\.ole\.ps	\$fcc2,\$f11,\$f12
0+0054 <[^>]*> 46ca4834 	c\.olt\.ps	\$f9,\$f10
0+0058 <[^>]*> 46cc5a34 	c\.olt\.ps	\$fcc2,\$f11,\$f12
0+005c <[^>]*> 46ca483a 	c\.seq\.ps	\$f9,\$f10
0+0060 <[^>]*> 46cc5a3a 	c\.seq\.ps	\$fcc2,\$f11,\$f12
0+0064 <[^>]*> 46ca4838 	c\.sf\.ps	\$f9,\$f10
0+0068 <[^>]*> 46cc5a38 	c\.sf\.ps	\$fcc2,\$f11,\$f12
0+006c <[^>]*> 46ca4833 	c\.ueq\.ps	\$f9,\$f10
0+0070 <[^>]*> 46cc5a33 	c\.ueq\.ps	\$fcc2,\$f11,\$f12
0+0074 <[^>]*> 46ca4837 	c\.ule\.ps	\$f9,\$f10
0+0078 <[^>]*> 46cc5a37 	c\.ule\.ps	\$fcc2,\$f11,\$f12
0+007c <[^>]*> 46ca4835 	c\.ult\.ps	\$f9,\$f10
0+0080 <[^>]*> 46cc5a35 	c\.ult\.ps	\$fcc2,\$f11,\$f12
0+0084 <[^>]*> 46ca4831 	c\.un\.ps	\$f9,\$f10
0+0088 <[^>]*> 46cc5a31 	c\.un\.ps	\$fcc2,\$f11,\$f12
0+008c <[^>]*> 460f7366 	cvt\.ps\.s	\$f13,\$f14,\$f15
0+0090 <[^>]*> 46c08c28 	cvt\.s\.pl	\$f16,\$f17
0+0094 <[^>]*> 46c09ca0 	cvt\.s\.pu	\$f18,\$f19
0+0098 <[^>]*> 4ca40505 	luxc1	\$f20,\$4\(\$5\)
0+009c <[^>]*> 4ed8bd66 	madd\.ps	\$f21,\$f22,\$f23,\$f24
0+00a0 <[^>]*> 46c0d646 	mov\.ps	\$f25,\$f26
0+00a4 <[^>]*> 46c8e6d1 	movf\.ps	\$f27,\$f28,\$fcc2
0+00a8 <[^>]*> 46c3e6d3 	movn\.ps	\$f27,\$f28,\$3
0+00ac <[^>]*> 46d1f751 	movt\.ps	\$f29,\$f30,\$fcc4
0+00b0 <[^>]*> 46c5f752 	movz\.ps	\$f29,\$f30,\$5
0+00b4 <[^>]*> 4c020fee 	msub\.ps	\$f31,\$f0,\$f1,\$f2
0+00b8 <[^>]*> 46c520c2 	mul\.ps	\$f3,\$f4,\$f5
0+00bc <[^>]*> 46c03987 	neg\.ps	\$f6,\$f7
0+00c0 <[^>]*> 4d2b5236 	nmadd\.ps	\$f8,\$f9,\$f10,\$f11
0+00c4 <[^>]*> 4d2b523e 	nmsub\.ps	\$f8,\$f9,\$f10,\$f11
0+00c8 <[^>]*> 46ce6b2c 	pll\.ps	\$f12,\$f13,\$f14
0+00cc <[^>]*> 46d183ed 	plu\.ps	\$f15,\$f16,\$f17
0+00d0 <[^>]*> 46d49cae 	pul\.ps	\$f18,\$f19,\$f20
0+00d4 <[^>]*> 46d7b56f 	puu\.ps	\$f21,\$f22,\$f23
0+00d8 <[^>]*> 46dace01 	sub\.ps	\$f24,\$f25,\$f26
0+00dc <[^>]*> 4ce6d80d 	suxc1	\$f27,\$6\(\$7\)
0+00e0 <[^>]*> 46cc5b32 	c\.eq\.ps	\$fcc3,\$f11,\$f12
0+00e4 <[^>]*> 46cce6d1 	movf\.ps	\$f27,\$f28,\$fcc3
	\.\.\.
