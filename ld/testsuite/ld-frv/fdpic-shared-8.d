#name: FRV uClinux PIC relocs to (mostly) global symbols with addends, shared linking
#source: fdpic8.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -shared --version-script fdpic8min.ldv

.*:     file format elf.*frv.*

Disassembly of section \.text:

000004d4 <F8>:
 4d4:	80 3c 00 02 	call 4dc <GF1\+0x4>

000004d8 <GF1>:
 4d8:	80 40 f0 10 	addi gr15,16,gr0
 4dc:	80 fc 00 14 	setlos 0x14,gr0
 4e0:	80 f4 00 24 	setlo 0x24,gr0
 4e4:	80 f8 00 00 	sethi hi\(0x0\),gr0
 4e8:	80 40 f0 0c 	addi gr15,12,gr0
 4ec:	80 fc 00 1c 	setlos 0x1c,gr0
 4f0:	80 f4 00 18 	setlo 0x18,gr0
 4f4:	80 f8 00 00 	sethi hi\(0x0\),gr0
 4f8:	80 40 ff f8 	addi gr15,-8,gr0
 4fc:	80 fc ff f0 	setlos 0xfffffff0,gr0
 500:	80 f4 ff c8 	setlo 0xffc8,gr0
 504:	80 f8 ff ff 	sethi 0xffff,gr0
 508:	80 40 ff 4c 	addi gr15,-180,gr0
 50c:	80 fc ff 4c 	setlos 0xffffff4c,gr0
 510:	80 f4 ff 4c 	setlo 0xff4c,gr0
 514:	80 f8 ff ff 	sethi 0xffff,gr0
 518:	80 f4 00 20 	setlo 0x20,gr0
 51c:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.data:

00010524 <D8>:
   10524:	00 00 00 04 	add\.p gr0,gr4,gr0
			10524: R_FRV_32	GD0

00010528 <GD0>:
   10528:	00 00 00 10 	add\.p gr0,gr16,gr0
			10528: R_FRV_32	\.got
   1052c:	00 00 00 08 	add\.p gr0,gr8,gr0
			1052c: R_FRV_32	\.text
Disassembly of section \.got:

000105a8 <_GLOBAL_OFFSET_TABLE_-0x38>:
   105a8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105a8: R_FRV_FUNCDESC_VALUE	\.text
   105ac:	00 00 00 00 	add\.p gr0,gr0,gr0
   105b0:	00 00 00 08 	add\.p gr0,gr8,gr0
			105b0: R_FRV_FUNCDESC_VALUE	\.text
   105b4:	00 00 00 00 	add\.p gr0,gr0,gr0
   105b8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105b8: R_FRV_FUNCDESC_VALUE	\.text
   105bc:	00 00 00 00 	add\.p gr0,gr0,gr0
   105c0:	00 00 00 08 	add\.p gr0,gr8,gr0
			105c0: R_FRV_FUNCDESC_VALUE	\.text
   105c4:	00 00 00 00 	add\.p gr0,gr0,gr0
   105c8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105c8: R_FRV_FUNCDESC_VALUE	\.text
   105cc:	00 00 00 00 	add\.p gr0,gr0,gr0
   105d0:	00 00 00 08 	add\.p gr0,gr8,gr0
			105d0: R_FRV_FUNCDESC_VALUE	\.text
   105d4:	00 00 00 00 	add\.p gr0,gr0,gr0
   105d8:	00 00 00 08 	add\.p gr0,gr8,gr0
			105d8: R_FRV_FUNCDESC_VALUE	\.text
   105dc:	00 00 00 00 	add\.p gr0,gr0,gr0

000105e0 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   105ec:	00 00 00 08 	add\.p gr0,gr8,gr0
			105ec: R_FRV_32	\.got
   105f0:	00 00 00 04 	add\.p gr0,gr4,gr0
			105f0: R_FRV_32	GF1
   105f4:	00 00 00 04 	add\.p gr0,gr4,gr0
			105f4: R_FRV_32	GF2
   105f8:	00 00 00 20 	add\.p gr0,gr32,gr0
			105f8: R_FRV_32	\.got
   105fc:	00 00 00 18 	add\.p gr0,gr24,gr0
			105fc: R_FRV_32	\.got
   10600:	00 00 00 04 	add\.p gr0,gr4,gr0
			10600: R_FRV_32	GD4
   10604:	00 00 00 04 	add\.p gr0,gr4,gr0
			10604: R_FRV_32	GF3
