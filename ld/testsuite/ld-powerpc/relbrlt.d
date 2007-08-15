#source: relbrlt.s
#as: -a64
#ld: -melf64ppc --emit-relocs
#objdump: -dr

.*:     file format elf64-powerpc

Disassembly of section \.text:

0*100000b0 <_start>:
[0-9a-f	 ]*:	49 bf 00 2d 	bl      .*
[0-9a-f	 ]*: R_PPC64_REL24	\.text\+0x37e003c
[0-9a-f	 ]*:	60 00 00 00 	nop
[0-9a-f	 ]*:	49 bf 00 19 	bl      .*
[0-9a-f	 ]*: R_PPC64_REL24	\.text\+0x3bf0020
[0-9a-f	 ]*:	60 00 00 00 	nop
[0-9a-f	 ]*:	49 bf 00 21 	bl      .*
[0-9a-f	 ]*: R_PPC64_REL24	\.text\+0x57e0024
[0-9a-f	 ]*:	60 00 00 00 	nop
[0-9a-f	 ]*:	00 00 00 00 	\.long 0x0
[0-9a-f	 ]*:	4b ff ff e4 	b       .* <_start>
	\.\.\.

[0-9a-f	 ]*<.*plt_branch.*>:
[0-9a-f	 ]*:	e9 62 80 00 	ld      r11,-32768\(r2\)
[0-9a-f	 ]*:	7d 69 03 a6 	mtctr   r11
[0-9a-f	 ]*:	4e 80 04 20 	bctr

[0-9a-f	 ]*<.*long_branch.*>:
[0-9a-f	 ]*:	49 bf 00 10 	b       .* <far>
[0-9a-f	 ]*: R_PPC64_REL24	\*ABS\*\+0x137e00ec

[0-9a-f	 ]*<.*plt_branch.*>:
[0-9a-f	 ]*:	e9 62 80 08 	ld      r11,-32760\(r2\)
[0-9a-f	 ]*:	7d 69 03 a6 	mtctr   r11
[0-9a-f	 ]*:	4e 80 04 20 	bctr
	\.\.\.

0*137e00ec <far>:
[0-9a-f	 ]*:	4e 80 00 20 	blr
	\.\.\.

[0-9a-f	 ]*<far2far>:
[0-9a-f	 ]*:	4e 80 00 20 	blr
	\.\.\.

[0-9a-f	 ]*<huge>:
[0-9a-f	 ]*:	4e 80 00 20 	blr
