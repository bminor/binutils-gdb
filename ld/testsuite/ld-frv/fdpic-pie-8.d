#name: FRV uClinux PIC relocs to global symbols with addends, pie linking
#source: fdpic8.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -pie --version-script fdpic8min.ldv

.*:     file format elf.*frv.*

Disassembly of section \.text:

00000498 <F8>:
 498:	80 3c 00 02 	call 4a0 <GF1\+0x4>

0000049c <GF1>:
 49c:	80 40 f0 10 	addi gr15,16,gr0
 4a0:	80 fc 00 14 	setlos 0x14,gr0
 4a4:	80 f4 00 24 	setlo 0x24,gr0
 4a8:	80 f8 00 00 	sethi hi\(0x0\),gr0
 4ac:	80 40 f0 0c 	addi gr15,12,gr0
 4b0:	80 fc 00 1c 	setlos 0x1c,gr0
 4b4:	80 f4 00 18 	setlo 0x18,gr0
 4b8:	80 f8 00 00 	sethi hi\(0x0\),gr0
 4bc:	80 40 ff f8 	addi gr15,-8,gr0
 4c0:	80 fc ff f0 	setlos 0xfffffff0,gr0
 4c4:	80 f4 ff c8 	setlo 0xffc8,gr0
 4c8:	80 f8 ff ff 	sethi 0xffff,gr0
 4cc:	80 40 ff 48 	addi gr15,-184,gr0
 4d0:	80 fc ff 48 	setlos 0xffffff48,gr0
 4d4:	80 f4 ff 48 	setlo 0xff48,gr0
 4d8:	80 f8 ff ff 	sethi 0xffff,gr0
 4dc:	80 f4 00 20 	setlo 0x20,gr0
 4e0:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.data:

000104f8 <D8>:
   104f8:	00 00 00 08 	add\.p gr0,gr8,gr0
			104f8: R_FRV_32	\.data

000104fc <GD0>:
   104fc:	00 00 00 10 	add\.p gr0,gr16,gr0
			104fc: R_FRV_32	\.got
   10500:	00 00 00 08 	add\.p gr0,gr8,gr0
			10500: R_FRV_32	\.text
Disassembly of section \.got:

00010580 <_GLOBAL_OFFSET_TABLE_-0x38>:
   10580:	00 00 00 08 	add\.p gr0,gr8,gr0
			10580: R_FRV_FUNCDESC_VALUE	\.text
   10584:	00 00 00 02 	add\.p gr0,fp,gr0
   10588:	00 00 00 08 	add\.p gr0,gr8,gr0
			10588: R_FRV_FUNCDESC_VALUE	\.text
   1058c:	00 00 00 02 	add\.p gr0,fp,gr0
   10590:	00 00 00 08 	add\.p gr0,gr8,gr0
			10590: R_FRV_FUNCDESC_VALUE	\.text
   10594:	00 00 00 02 	add\.p gr0,fp,gr0
   10598:	00 00 00 08 	add\.p gr0,gr8,gr0
			10598: R_FRV_FUNCDESC_VALUE	\.text
   1059c:	00 00 00 02 	add\.p gr0,fp,gr0
   105a0:	00 00 00 08 	add\.p gr0,gr8,gr0
			105a0: R_FRV_FUNCDESC_VALUE	\.text
   105a4:	00 00 00 02 	add\.p gr0,fp,gr0
   105a8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105a8: R_FRV_FUNCDESC_VALUE	\.text
   105ac:	00 00 00 02 	add\.p gr0,fp,gr0
   105b0:	00 00 00 08 	add\.p gr0,gr8,gr0
			105b0: R_FRV_FUNCDESC_VALUE	\.text
   105b4:	00 00 00 02 	add\.p gr0,fp,gr0

000105b8 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   105c4:	00 00 00 08 	add\.p gr0,gr8,gr0
			105c4: R_FRV_32	\.got
   105c8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105c8: R_FRV_32	\.text
   105cc:	00 00 00 08 	add\.p gr0,gr8,gr0
			105cc: R_FRV_32	\.text
   105d0:	00 00 00 20 	add\.p gr0,gr32,gr0
			105d0: R_FRV_32	\.got
   105d4:	00 00 00 18 	add\.p gr0,gr24,gr0
			105d4: R_FRV_32	\.got
   105d8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105d8: R_FRV_32	\.data
   105dc:	00 00 00 08 	add\.p gr0,gr8,gr0
			105dc: R_FRV_32	\.text
