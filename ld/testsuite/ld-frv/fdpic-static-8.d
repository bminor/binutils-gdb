#name: FRV uClinux PIC relocs to global symbols with addends, static linking
#source: fdpic8.s
#objdump: -D
#as: -mfdpic
#ld: -static

.*:     file format elf.*frv.*

Disassembly of section \.text:

00010094 <F8>:
   10094:	80 3c 00 02 	call 1009c <GF0\+0x4>

00010098 <GF0>:
   10098:	80 40 f0 10 	addi gr15,16,gr0
   1009c:	80 fc 00 14 	setlos 0x14,gr0
   100a0:	80 f4 00 24 	setlo 0x24,gr0
   100a4:	80 f8 00 00 	sethi hi\(0x0\),gr0
   100a8:	80 40 f0 0c 	addi gr15,12,gr0
   100ac:	80 fc 00 1c 	setlos 0x1c,gr0
   100b0:	80 f4 00 18 	setlo 0x18,gr0
   100b4:	80 f8 00 00 	sethi hi\(0x0\),gr0
   100b8:	80 40 ff f8 	addi gr15,-8,gr0
   100bc:	80 fc ff f0 	setlos 0xfffffff0,gr0
   100c0:	80 f4 ff c8 	setlo 0xffc8,gr0
   100c4:	80 f8 ff ff 	sethi 0xffff,gr0
   100c8:	80 40 ff c4 	addi gr15,-60,gr0
   100cc:	80 fc ff c4 	setlos 0xffffffc4,gr0
   100d0:	80 f4 ff c4 	setlo 0xffc4,gr0
   100d4:	80 f8 ff ff 	sethi 0xffff,gr0
   100d8:	80 f4 00 20 	setlo 0x20,gr0
   100dc:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.rofixup:

000100e0 <__ROFIXUP_LIST__>:
   100e0:	00 02 01 98 	subx\.p gr32,gr24,gr0,icc0
   100e4:	00 02 01 9c 	subx\.p gr32,gr28,gr0,icc0
   100e8:	00 02 01 ac 	subx\.p gr32,gr44,gr0,icc0
   100ec:	00 02 01 94 	subx\.p gr32,gr20,gr0,icc0
   100f0:	00 02 01 58 	subcc\.p gr32,gr24,gr0,icc0
   100f4:	00 02 01 5c 	subcc\.p gr32,gr28,gr0,icc0
   100f8:	00 02 01 a4 	subx\.p gr32,gr36,gr0,icc0
   100fc:	00 02 01 68 	subcc\.p gr32,gr40,gr0,icc0
   10100:	00 02 01 6c 	subcc\.p gr32,gr44,gr0,icc0
   10104:	00 02 01 a0 	subx\.p gr32,gr32,gr0,icc0
   10108:	00 02 01 70 	subcc\.p gr32,gr48,gr0,icc0
   1010c:	00 02 01 74 	subcc\.p gr32,gr52,gr0,icc0
   10110:	00 02 01 80 	subx\.p gr32,gr0,gr0,icc0
   10114:	00 02 01 84 	subx\.p gr32,gr4,gr0,icc0
   10118:	00 02 01 78 	subcc\.p gr32,gr56,gr0,icc0
   1011c:	00 02 01 7c 	subcc\.p gr32,gr60,gr0,icc0
   10120:	00 02 01 50 	subcc\.p gr32,gr16,gr0,icc0
   10124:	00 02 01 54 	subcc\.p gr32,gr20,gr0,icc0
   10128:	00 02 01 a8 	subx\.p gr32,gr40,gr0,icc0
   1012c:	00 02 01 44 	subcc\.p gr32,gr4,gr0,icc0
   10130:	00 02 01 60 	subcc\.p gr32,gr32,gr0,icc0
   10134:	00 02 01 64 	subcc\.p gr32,gr36,gr0,icc0
   10138:	00 02 01 48 	subcc\.p gr32,gr8,gr0,icc0
   1013c:	00 02 01 4c 	subcc\.p gr32,gr12,gr0,icc0
   10140:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
Disassembly of section \.data:

00020144 <D8>:
   20144:	00 02 01 4c 	subcc\.p gr32,gr12,gr0,icc0

00020148 <GD0>:
   20148:	00 02 01 60 	subcc\.p gr32,gr32,gr0,icc0
   2014c:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
Disassembly of section \.got:

00020150 <_GLOBAL_OFFSET_TABLE_-0x38>:
   20150:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   20154:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
   20158:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   2015c:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
   20160:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   20164:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
   20168:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   2016c:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
   20170:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   20174:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
   20178:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   2017c:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0
   20180:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   20184:	00 02 01 88 	subx\.p gr32,gr8,gr0,icc0

00020188 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   20194:	00 02 01 58 	subcc\.p gr32,gr24,gr0,icc0
   20198:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   2019c:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
   201a0:	00 02 01 70 	subcc\.p gr32,gr48,gr0,icc0
   201a4:	00 02 01 68 	subcc\.p gr32,gr40,gr0,icc0
   201a8:	00 02 01 4c 	subcc\.p gr32,gr12,gr0,icc0
   201ac:	00 01 00 9c 	addx\.p gr16,gr28,gr0,icc0
