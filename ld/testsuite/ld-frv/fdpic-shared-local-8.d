#name: FRV uClinux PIC relocs to forced-local symbols with addends, shared linking
#source: fdpic8.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -shared --version-script fdpic8.ldv

.*:     file format elf.*frv.*

Disassembly of section \.text:

00000300 <F8>:
 300:	80 3c 00 02 	call 308 <GF0\+0x4>

00000304 <GF0>:
 304:	80 40 f0 10 	addi gr15,16,gr0
 308:	80 fc 00 14 	setlos 0x14,gr0
 30c:	80 f4 00 24 	setlo 0x24,gr0
 310:	80 f8 00 00 	sethi hi\(0x0\),gr0
 314:	80 40 f0 0c 	addi gr15,12,gr0
 318:	80 fc 00 1c 	setlos 0x1c,gr0
 31c:	80 f4 00 18 	setlo 0x18,gr0
 320:	80 f8 00 00 	sethi hi\(0x0\),gr0
 324:	80 40 ff f8 	addi gr15,-8,gr0
 328:	80 fc ff f0 	setlos 0xfffffff0,gr0
 32c:	80 f4 ff c8 	setlo 0xffc8,gr0
 330:	80 f8 ff ff 	sethi 0xffff,gr0
 334:	80 40 ff 48 	addi gr15,-184,gr0
 338:	80 fc ff 48 	setlos 0xffffff48,gr0
 33c:	80 f4 ff 48 	setlo 0xff48,gr0
 340:	80 f8 ff ff 	sethi 0xffff,gr0
 344:	80 f4 00 20 	setlo 0x20,gr0
 348:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.data:

00010350 <D8>:
   10350:	00 00 00 08 	add\.p gr0,gr8,gr0
			10350: R_FRV_32	\.data

00010354 <GD0>:
   10354:	00 00 00 10 	add\.p gr0,gr16,gr0
			10354: R_FRV_32	\.got
   10358:	00 00 00 08 	add\.p gr0,gr8,gr0
			10358: R_FRV_32	\.text
Disassembly of section \.got:

000103d8 <_GLOBAL_OFFSET_TABLE_-0x38>:
   103d8:	00 00 00 08 	add\.p gr0,gr8,gr0
			103d8: R_FRV_FUNCDESC_VALUE	\.text
   103dc:	00 00 00 00 	add\.p gr0,gr0,gr0
   103e0:	00 00 00 08 	add\.p gr0,gr8,gr0
			103e0: R_FRV_FUNCDESC_VALUE	\.text
   103e4:	00 00 00 00 	add\.p gr0,gr0,gr0
   103e8:	00 00 00 08 	add\.p gr0,gr8,gr0
			103e8: R_FRV_FUNCDESC_VALUE	\.text
   103ec:	00 00 00 00 	add\.p gr0,gr0,gr0
   103f0:	00 00 00 08 	add\.p gr0,gr8,gr0
			103f0: R_FRV_FUNCDESC_VALUE	\.text
   103f4:	00 00 00 00 	add\.p gr0,gr0,gr0
   103f8:	00 00 00 08 	add\.p gr0,gr8,gr0
			103f8: R_FRV_FUNCDESC_VALUE	\.text
   103fc:	00 00 00 00 	add\.p gr0,gr0,gr0
   10400:	00 00 00 08 	add\.p gr0,gr8,gr0
			10400: R_FRV_FUNCDESC_VALUE	\.text
   10404:	00 00 00 00 	add\.p gr0,gr0,gr0
   10408:	00 00 00 08 	add\.p gr0,gr8,gr0
			10408: R_FRV_FUNCDESC_VALUE	\.text
   1040c:	00 00 00 00 	add\.p gr0,gr0,gr0

00010410 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   1041c:	00 00 00 08 	add\.p gr0,gr8,gr0
			1041c: R_FRV_32	\.got
   10420:	00 00 00 08 	add\.p gr0,gr8,gr0
			10420: R_FRV_32	\.text
   10424:	00 00 00 08 	add\.p gr0,gr8,gr0
			10424: R_FRV_32	\.text
   10428:	00 00 00 20 	add\.p gr0,gr32,gr0
			10428: R_FRV_32	\.got
   1042c:	00 00 00 18 	add\.p gr0,gr24,gr0
			1042c: R_FRV_32	\.got
   10430:	00 00 00 08 	add\.p gr0,gr8,gr0
			10430: R_FRV_32	\.data
   10434:	00 00 00 08 	add\.p gr0,gr8,gr0
			10434: R_FRV_32	\.text
