#name: FRV uClinux PIC relocs to hidden symbols, shared linking
#source: fdpic3.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -shared

.*:     file format elf.*frv.*

Disassembly of section \.text:

0000042c <F3>:
 42c:	80 3c 00 01 	call 430 <HF0>

00000430 <HF0>:
 430:	80 40 f0 0c 	addi gr15,12,gr0
 434:	80 fc 00 18 	setlos 0x18,gr0
 438:	80 f4 00 1c 	setlo 0x1c,gr0
 43c:	80 f8 00 00 	sethi hi\(0x0\),gr0
 440:	80 40 f0 10 	addi gr15,16,gr0
 444:	80 fc 00 20 	setlos 0x20,gr0
 448:	80 f4 00 14 	setlo 0x14,gr0
 44c:	80 f8 00 00 	sethi hi\(0x0\),gr0
 450:	80 40 ff f8 	addi gr15,-8,gr0
 454:	80 fc ff e8 	setlos 0xffffffe8,gr0
 458:	80 f4 ff d8 	setlo 0xffd8,gr0
 45c:	80 f8 ff ff 	sethi 0xffff,gr0
 460:	80 40 ff 48 	addi gr15,-184,gr0
 464:	80 fc ff 48 	setlos 0xffffff48,gr0
 468:	80 f4 ff 48 	setlo 0xff48,gr0
 46c:	80 f8 ff ff 	sethi 0xffff,gr0
 470:	80 f4 00 24 	setlo 0x24,gr0
 474:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.data:

0001047c <D3>:
   1047c:	00 00 00 04 	add\.p gr0,gr4,gr0
			1047c: R_FRV_32	\.data

00010480 <HD0>:
   10480:	00 00 00 08 	add\.p gr0,gr8,gr0
			10480: R_FRV_32	\.got
   10484:	00 00 00 04 	add\.p gr0,gr4,gr0
			10484: R_FRV_32	\.text
Disassembly of section \.got:

00010500 <_GLOBAL_OFFSET_TABLE_-0x38>:
   10500:	00 00 00 04 	add\.p gr0,gr4,gr0
			10500: R_FRV_FUNCDESC_VALUE	\.text
   10504:	00 00 00 00 	add\.p gr0,gr0,gr0
   10508:	00 00 00 04 	add\.p gr0,gr4,gr0
			10508: R_FRV_FUNCDESC_VALUE	\.text
   1050c:	00 00 00 00 	add\.p gr0,gr0,gr0
   10510:	00 00 00 04 	add\.p gr0,gr4,gr0
			10510: R_FRV_FUNCDESC_VALUE	\.text
   10514:	00 00 00 00 	add\.p gr0,gr0,gr0
   10518:	00 00 00 04 	add\.p gr0,gr4,gr0
			10518: R_FRV_FUNCDESC_VALUE	\.text
   1051c:	00 00 00 00 	add\.p gr0,gr0,gr0
   10520:	00 00 00 04 	add\.p gr0,gr4,gr0
			10520: R_FRV_FUNCDESC_VALUE	\.text
   10524:	00 00 00 00 	add\.p gr0,gr0,gr0
   10528:	00 00 00 04 	add\.p gr0,gr4,gr0
			10528: R_FRV_FUNCDESC_VALUE	\.text
   1052c:	00 00 00 00 	add\.p gr0,gr0,gr0
   10530:	00 00 00 04 	add\.p gr0,gr4,gr0
			10530: R_FRV_FUNCDESC_VALUE	\.text
   10534:	00 00 00 00 	add\.p gr0,gr0,gr0

00010538 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   10544:	00 00 00 04 	add\.p gr0,gr4,gr0
			10544: R_FRV_32	\.text
   10548:	00 00 00 00 	add\.p gr0,gr0,gr0
			10548: R_FRV_32	\.got
   1054c:	00 00 00 28 	add\.p gr0,gr40,gr0
			1054c: R_FRV_32	\.got
   10550:	00 00 00 04 	add\.p gr0,gr4,gr0
			10550: R_FRV_32	\.text
   10554:	00 00 00 04 	add\.p gr0,gr4,gr0
			10554: R_FRV_32	\.text
   10558:	00 00 00 18 	add\.p gr0,gr24,gr0
			10558: R_FRV_32	\.got
   1055c:	00 00 00 04 	add\.p gr0,gr4,gr0
			1055c: R_FRV_32	\.data
