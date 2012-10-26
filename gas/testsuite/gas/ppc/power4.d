#objdump: -drx -Mpower4
#as: -mpower4
#name: Power4 instructions

.*: +file format elf64-powerpc
.*
architecture: powerpc:common64, flags 0x0+11:
HAS_RELOC, HAS_SYMS
start address 0x0+

Sections:
Idx Name +Size +VMA +LMA +File off +Algn
 +0 \.text +0+c8 +0+ +0+ +.*
 +CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
 +1 \.data +0+20 +0+ +0+ +.*
 +CONTENTS, ALLOC, LOAD, DATA
 +2 \.bss +0+ +0+ +0+ +.*
 +ALLOC
 +3 \.toc +0+20 +0+ +0+ +.*
 +CONTENTS, ALLOC, LOAD, RELOC, DATA
SYMBOL TABLE:
0+ l +d +\.text	0+ (|\.text)
0+ l +d +\.data	0+ (|\.data)
0+ l +d +\.bss	0+ (|\.bss)
0+ l +\.data	0+ dsym0
0+10 l +\.data	0+ dsym1
0+ l +d +\.toc	0+ (|\.toc)
0+10 l +\.data	0+ usym0
0+20 l +\.data	0+ usym1
0+ +\*UND\*	0+ esym0
0+ +\*UND\*	0+ esym1


Disassembly of section \.text:

0+ <\.text>:
.*:	e0 83 00 00 	lq      r4,0\(r3\)
.*: R_PPC64_ADDR16_LO_DS	\.data
.*:	e0 83 00 10 	lq      r4,16\(r3\)
.*: R_PPC64_ADDR16_LO_DS	\.data\+0x10
.*:	e0 83 00 10 	lq      r4,16\(r3\)
.*: R_PPC64_ADDR16_LO_DS	\.data\+0x10
.*:	e0 83 00 20 	lq      r4,32\(r3\)
.*: R_PPC64_ADDR16_LO_DS	\.data\+0x20
.*:	e0 83 00 00 	lq      r4,0\(r3\)
.*: R_PPC64_ADDR16_LO_DS	esym0
.*:	e0 83 00 00 	lq      r4,0\(r3\)
.*: R_PPC64_ADDR16_LO_DS	esym1
.*:	e0 82 00 00 	lq      r4,0\(r2\)
.*: R_PPC64_TOC16_DS	\.toc
.*:	e0 82 00 10 	lq      r4,16\(r2\)
.*: R_PPC64_TOC16_DS	\.toc\+0x10
.*:	e0 80 00 00 	lq      r4,0\(0\)
.*: R_PPC64_ADDR16_LO_DS	\.text
.*:	e0 c3 00 00 	lq      r6,0\(r3\)
.*: R_PPC64_GOT16_DS	dsym0
.*:	e0 c3 00 00 	lq      r6,0\(r3\)
.*: R_PPC64_GOT16_LO_DS	dsym0
.*:	e0 c3 00 00 	lq      r6,0\(r3\)
.*: R_PPC64_PLT16_LO_DS	\.data
.*:	e0 c3 00 10 	lq      r6,16\(r3\)
.*: R_PPC64_SECTOFF_DS	\.data\+0x10
.*:	e0 c3 00 10 	lq      r6,16\(r3\)
.*: R_PPC64_SECTOFF_LO_DS	\.data\+0x10
.*:	e0 c4 00 20 	lq      r6,32\(r4\)
.*:	f8 c7 00 02 	stq     r6,0\(r7\)
.*:	f8 c7 00 12 	stq     r6,16\(r7\)
.*:	f8 c7 ff f2 	stq     r6,-16\(r7\)
.*:	f8 c7 80 02 	stq     r6,-32768\(r7\)
.*:	f8 c7 7f f2 	stq     r6,32752\(r7\)
.*:	00 00 02 00 	attn
.*:	7c 6f f1 20 	mtcr    r3
.*:	7c 6f f1 20 	mtcr    r3
.*:	7c 68 11 20 	mtcrf   129,r3
.*:	7c 70 11 20 	mtocrf  1,r3
.*:	7c 70 21 20 	mtocrf  2,r3
.*:	7c 70 41 20 	mtocrf  4,r3
.*:	7c 70 81 20 	mtocrf  8,r3
.*:	7c 71 01 20 	mtocrf  16,r3
.*:	7c 72 01 20 	mtocrf  32,r3
.*:	7c 74 01 20 	mtocrf  64,r3
.*:	7c 78 01 20 	mtocrf  128,r3
.*:	7c 60 00 26 	mfcr    r3
.*:	7c 70 10 26 	mfocrf  r3,1
.*:	7c 70 20 26 	mfocrf  r3,2
.*:	7c 70 40 26 	mfocrf  r3,4
.*:	7c 70 80 26 	mfocrf  r3,8
.*:	7c 71 00 26 	mfocrf  r3,16
.*:	7c 72 00 26 	mfocrf  r3,32
.*:	7c 74 00 26 	mfocrf  r3,64
.*:	7c 78 00 26 	mfocrf  r3,128
.*:	7c 01 17 ec 	dcbz    r1,r2
.*:	7c 23 27 ec 	dcbzl   r3,r4
.*:	7c 05 37 ec 	dcbz    r5,r6
.*:	e0 40 00 10 	lq      r2,16\(0\)
.*:	e0 05 00 10 	lq      r0,16\(r5\)
.*:	e0 45 00 10 	lq      r2,16\(r5\)
.*:	f8 40 00 12 	stq     r2,16\(0\)
.*:	f8 05 00 12 	stq     r0,16\(r5\)
.*:	f8 45 00 12 	stq     r2,16\(r5\)
