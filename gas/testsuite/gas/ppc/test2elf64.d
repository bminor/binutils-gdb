#objdump: -Drx -Mpower4
#as: -mpower4
#name: PowerPC Test 2, 64 bit elf, power4 instructions

.*: +file format elf64-powerpc
.*
architecture: powerpc:common64, flags 0x0+11:
HAS_RELOC, HAS_SYMS
start address 0x0+

Sections:
Idx Name +Size +VMA +LMA +File off +Algn
 +0 \.text +0+68 +0+ +0+ +.*
 +CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
 +1 \.data +0+10 +0+ +0+ +.*
 +CONTENTS, ALLOC, LOAD, DATA
 +2 \.bss +0+ +0+ +0+ +.*
 +ALLOC
 +3 \.toc +0+30 +0+ +0+ +.*
 +CONTENTS, ALLOC, LOAD, RELOC, DATA
SYMBOL TABLE:
0+ l +d +\.text	0+ 
0+ l +d +\.data	0+ 
0+ l +d +\.bss	0+ 
0+ l +\.data	0+ dsym0
0+8 l +\.data	0+ dsym1
0+ l +d +\.toc	0+ 
0+8 l +\.data	0+ usym0
0+10 l +\.data	0+ usym1
0+ +\*UND\*	0+ esym0
0+ +\*UND\*	0+ esym1


Disassembly of section \.text:

0+ <\.text>:
 +0:	e0 83 00 00 	lq	r4,0\(r3\)
			2: R_PPC64_ADDR16_LO_DS	dsym0
 +4:	e0 83 00 00 	lq	r4,0\(r3\)
			6: R_PPC64_ADDR16_LO_DS	dsym1
 +8:	e0 83 00 00 	lq	r4,0\(r3\)
			a: R_PPC64_ADDR16_LO_DS	usym0
 +c:	e0 83 00 00 	lq	r4,0\(r3\)
			e: R_PPC64_ADDR16_LO_DS	usym1
 +10:	e0 83 00 00 	lq	r4,0\(r3\)
			12: R_PPC64_ADDR16_LO_DS	esym0
 +14:	e0 83 00 00 	lq	r4,0\(r3\)
			16: R_PPC64_ADDR16_LO_DS	esym1
 +18:	e0 82 00 00 	lq	r4,0\(r2\)
			1a: R_PPC64_TOC16_DS	\.toc
 +1c:	e0 82 00 00 	lq	r4,0\(r2\)
			1e: R_PPC64_TOC16_DS	\.toc\+0x8
 +20:	e0 82 00 10 	lq	r4,16\(r2\)
			22: R_PPC64_TOC16_DS	\.toc\+0x10
 +24:	e0 82 00 10 	lq	r4,16\(r2\)
			26: R_PPC64_TOC16_DS	\.toc\+0x18
 +28:	e0 82 00 20 	lq	r4,32\(r2\)
			2a: R_PPC64_TOC16_DS	\.toc\+0x20
 +2c:	e0 82 00 20 	lq	r4,32\(r2\)
			2e: R_PPC64_TOC16_DS	\.toc\+0x28
 +30:	e0 c2 00 20 	lq	r6,32\(r2\)
			32: R_PPC64_TOC16_LO_DS	\.toc\+0x28
 +34:	e0 80 00 00 	lq	r4,0\(r0\)
			36: R_PPC64_ADDR16_LO_DS	\.text
 +38:	e0 c3 00 00 	lq	r6,0\(r3\)
			3a: R_PPC64_GOT16_DS	dsym0
 +3c:	e0 c3 00 00 	lq	r6,0\(r3\)
			3e: R_PPC64_GOT16_LO_DS	dsym0
 +40:	e0 c3 00 00 	lq	r6,0\(r3\)
			42: R_PPC64_PLT16_LO_DS	dsym0
 +44:	e0 c3 00 00 	lq	r6,0\(r3\)
			46: R_PPC64_SECTOFF_DS	dsym1
 +48:	e0 c3 00 00 	lq	r6,0\(r3\)
			4a: R_PPC64_SECTOFF_LO_DS	dsym1
 +4c:	e0 c4 00 10 	lq	r6,16\(r4\)
 +50:	f8 c7 00 02 	stq	r6,0\(r7\)
 +54:	f8 c7 00 12 	stq	r6,16\(r7\)
 +58:	f8 c7 ff f2 	stq	r6,-16\(r7\)
 +5c:	f8 c7 80 02 	stq	r6,-32768\(r7\)
 +60:	f8 c7 7f f2 	stq	r6,32752\(r7\)
 +64:	00 00 02 00 	attn
Disassembly of section \.data:

0+ <dsym0>:
 +0:	00 00 00 00 	\.long 0x0
 +4:	de ad be ef 	stfdu	f21,-16657\(r13\)

0+8 <dsym1>:
 +8:	00 00 00 00 	\.long 0x0
 +c:	ca fe ba be 	lfd	f23,-17730\(r30\)

Disassembly of section \.toc:

0+ <\.toc>:
	\.\.\.
			0: R_PPC64_ADDR64	dsym0
			8: R_PPC64_ADDR64	dsym1
			10: R_PPC64_ADDR64	usym0
			18: R_PPC64_ADDR64	usym1
			20: R_PPC64_ADDR64	esym0
			28: R_PPC64_ADDR64	esym1
