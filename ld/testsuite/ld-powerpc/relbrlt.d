#source: relbrlt.s
#as: -a64
#ld: -melf64ppc --emit-relocs
#objdump: -dr

.*:     file format elf64-powerpc

Disassembly of section \.text:

0*100000a8 <_start>:
    100000a8:	49 bf 00 31 	bl      11bf00d8 .*
			100000a8: R_PPC64_REL24	\.text\+0x37e0044
    100000ac:	60 00 00 00 	nop
    100000b0:	49 bf 00 19 	bl      11bf00c8 .*
			100000b0: R_PPC64_REL24	\.text\+0x3bf0020
    100000b4:	60 00 00 00 	nop
    100000b8:	49 bf 00 25 	bl      11bf00dc .*
			100000b8: R_PPC64_REL24	\.text\+0x57e0024
    100000bc:	60 00 00 00 	nop
    100000c0:	00 00 00 00 	\.long 0x0
    100000c4:	4b ff ff e4 	b       100000a8 <_start>
	\.\.\.

0*11bf00c8 <.*plt_branch.*>:
    11bf00c8:	3d 82 05 7e 	addis   r12,r2,1406
    11bf00cc:	e9 6c 80 58 	ld      r11,-32680\(r12\)
    11bf00d0:	7d 69 03 a6 	mtctr   r11
    11bf00d4:	4e 80 04 20 	bctr

0*11bf00d8 <.*long_branch.*>:
    11bf00d8:	49 bf 00 14 	b       137e00ec <far>
			11bf00d8: R_PPC64_REL24	\*ABS\*\+0x137e00ec

0*11bf00dc <.*plt_branch.*>:
    11bf00dc:	3d 82 05 7e 	addis   r12,r2,1406
    11bf00e0:	e9 6c 80 60 	ld      r11,-32672\(r12\)
    11bf00e4:	7d 69 03 a6 	mtctr   r11
    11bf00e8:	4e 80 04 20 	bctr
	\.\.\.

0*137e00ec <far>:
    137e00ec:	4e 80 00 20 	blr
	\.\.\.

0*13bf00c8 <far2far>:
    13bf00c8:	4e 80 00 20 	blr
	\.\.\.

0*157e00cc <huge>:
    157e00cc:	4e 80 00 20 	blr
