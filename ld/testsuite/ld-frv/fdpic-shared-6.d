#name: FRV uClinux PIC relocs to weak undefined symbols, shared linking
#source: fdpic6.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -shared --defsym WD1=D6 --version-script fdpic6.ldv

.*:     file format elf.*frv.*

Disassembly of section \.plt:

0000041c <\.plt>:
 41c:	00 00 00 08 	add\.p gr0,gr8,gr0
 420:	c0 1a 00 06 	bra 438 <F6-0x10>
 424:	00 00 00 00 	add\.p gr0,gr0,gr0
 428:	c0 1a 00 04 	bra 438 <F6-0x10>
 42c:	00 00 00 10 	add\.p gr0,gr16,gr0
 430:	c0 1a 00 02 	bra 438 <F6-0x10>
 434:	00 00 00 18 	add\.p gr0,gr24,gr0
 438:	88 08 f1 40 	ldd @\(gr15,gr0\),gr4
 43c:	80 30 40 00 	jmpl @\(gr4,gr0\)
 440:	9c cc ff f0 	lddi @\(gr15,-16\),gr14
 444:	80 30 e0 00 	jmpl @\(gr14,gr0\)
Disassembly of section \.text:

00000448 <F6>:
 448:	fe 3f ff fe 	call 440 <F6-0x8>
 44c:	80 40 f0 0c 	addi gr15,12,gr0
 450:	80 fc 00 24 	setlos 0x24,gr0
 454:	80 f4 00 20 	setlo 0x20,gr0
 458:	80 f8 00 00 	sethi hi\(0x0\),gr0
 45c:	80 40 f0 10 	addi gr15,16,gr0
 460:	80 fc 00 18 	setlos 0x18,gr0
 464:	80 f4 00 1c 	setlo 0x1c,gr0
 468:	80 f8 00 00 	sethi hi\(0x0\),gr0
 46c:	80 40 ff f8 	addi gr15,-8,gr0
 470:	80 fc ff e8 	setlos 0xffffffe8,gr0
 474:	80 f4 ff e0 	setlo 0xffe0,gr0
 478:	80 f8 ff ff 	sethi 0xffff,gr0
 47c:	80 f4 ff 40 	setlo 0xff40,gr0
 480:	80 f8 ff ff 	sethi 0xffff,gr0
 484:	80 f4 00 14 	setlo 0x14,gr0
 488:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.data:

00010490 <D6>:
	\.\.\.
			10490: R_FRV_32	WD0
			10494: R_FRV_FUNCDESC	WFb
			10498: R_FRV_32	WFb
Disassembly of section \.got:

00010530 <_GLOBAL_OFFSET_TABLE_-0x20>:
   10530:	00 00 04 38 	\*unknown\*
			10530: R_FRV_FUNCDESC_VALUE	WF9
   10534:	00 00 00 00 	add\.p gr0,gr0,gr0
   10538:	00 00 04 30 	\*unknown\*
			10538: R_FRV_FUNCDESC_VALUE	WF8
   1053c:	00 00 00 00 	add\.p gr0,gr0,gr0
   10540:	00 00 04 28 	\*unknown\*
			10540: R_FRV_FUNCDESC_VALUE	WF0
   10544:	00 00 00 00 	add\.p gr0,gr0,gr0
   10548:	00 00 04 20 	\*unknown\*
			10548: R_FRV_FUNCDESC_VALUE	WF7
   1054c:	00 00 00 00 	add\.p gr0,gr0,gr0

00010550 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
			1055c: R_FRV_32	WF1
			10560: R_FRV_FUNCDESC	WF4
			10564: R_FRV_32	WD2
			10568: R_FRV_FUNCDESC	WF5
			1056c: R_FRV_FUNCDESC	WF6
			10570: R_FRV_32	WF3
			10574: R_FRV_32	WF2
