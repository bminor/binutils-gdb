#source: relbrlt.s
#as: -a64
#ld: -melf64ppc --emit-relocs
#objdump: -dr

.*:     file format elf64-powerpc

Disassembly of section \.text:

0*10000078 <_start>:
    10000078:	49 bf 00 31 	bl      11bf00a8 .*
			10000078: R_PPC64_REL24	\.text\+0x37e0044
    1000007c:	60 00 00 00 	nop
    10000080:	49 bf 00 19 	bl      11bf0098 .*
			10000080: R_PPC64_REL24	\.text\+0x3bf0020
    10000084:	60 00 00 00 	nop
    10000088:	49 bf 00 25 	bl      11bf00ac .*
			10000088: R_PPC64_REL24	\.text\+0x57e0024
    1000008c:	60 00 00 00 	nop
    10000090:	00 00 00 00 	\.long 0x0
    10000094:	4b ff ff e4 	b       10000078 <_start>
	\.\.\.

0*11bf0098 <.*plt_branch.*>:
    11bf0098:	3d 82 05 7e 	addis   r12,r2,1406
    11bf009c:	e9 6c 80 28 	ld      r11,-32728\(r12\)
    11bf00a0:	7d 69 03 a6 	mtctr   r11
    11bf00a4:	4e 80 04 20 	bctr

0*11bf00a8 <.*long_branch.*>:
    11bf00a8:	49 bf 00 14 	b       137e00bc <far>
			11bf00a8: R_PPC64_REL24	\*ABS\*\+0x137e00bc

0*11bf00ac <.*plt_branch.*>:
    11bf00ac:	3d 82 05 7e 	addis   r12,r2,1406
    11bf00b0:	e9 6c 80 30 	ld      r11,-32720\(r12\)
    11bf00b4:	7d 69 03 a6 	mtctr   r11
    11bf00b8:	4e 80 04 20 	bctr
	\.\.\.

0*137e00bc <far>:
    137e00bc:	4e 80 00 20 	blr
	\.\.\.

0*13bf0098 <far2far>:
    13bf0098:	4e 80 00 20 	blr
	\.\.\.

0*157e009c <huge>:
    157e009c:	4e 80 00 20 	blr
