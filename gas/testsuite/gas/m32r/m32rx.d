#as: -m32rx --no-warn-explicit-parallel-conflicts --m32rx-enable-special
#objdump: -dr
#name: m32rx

.*: +file format .*

Disassembly of section .text:

00000000 <bcl>:
   0:	78 00 f0 00 	bcl 0 <bcl> || nop

00000004 <bncl>:
   4:	79 ff f0 00 	bncl 0 <bcl> || nop

00000008 <cmpz>:
   8:	00 7d f0 00 	cmpz fp || nop

0000000c <cmpeq>:
   c:	0d 6d f0 00 	cmpeq fp,fp || nop

00000010 <maclh1>:
  10:	5d cd f0 00 	maclh1 fp,fp || nop

00000014 <msblo>:
  14:	5d dd f0 00 	msblo fp,fp || nop

00000018 <mulwu1>:
  18:	5d ad f0 00 	mulwu1 fp,fp || nop

0000001c <macwu1>:
  1c:	5d bd f0 00 	macwu1 fp,fp || nop

00000020 <sadd>:
  20:	50 e4 f0 00 	sadd || nop

00000024 <satb>:
  24:	8d 6d 03 00 	satb fp,fp

00000028 <mulhi>:
  28:	3d 8d f0 00 	mulhi fp, fp, a1 || nop

0000002c <mullo>:
  2c:	3d 1d f0 00 	mullo fp,fp || nop

00000030 <divh>:
  30:	9d 0d f0 00 	divh fp, fp || nop

00000034 <machi>:
  34:	3d cd f0 00 	machi fp, fp, a1 || nop

00000038 <maclo>:
  38:	3d 5d f0 00 	maclo fp, fp || nop

0000003c <mvfachi>:
  3c:	5d f4 f0 00 	mvfachi fp,a1 || nop

00000040 <mvfacmi>:
  40:	5d f6 f0 00 	mvfacmi fp || nop

00000044 <mvfaclo>:
  44:	5d f5 f0 00 	mvfaclo fp || nop

00000048 <mvtachi>:
  48:	5d f0 f0 00 	mvfachi fp || nop

0000004c <mvtaclo>:
  4c:	5d 71 f0 00 	mvtaclo fp || nop

00000050 <rac>:
  50:	54 90 f0 00 	rac a1 || nop

00000054 <rac_ds>:
  54:	54 90 f0 00 	rac a1, a0 || nop

00000058 <rac_dsi>:
  58:	50 94 f0 00 	rac a0, a1, #1 || nop

0000005c <rach>:
  5c:	54 80 f0 00 	rach a1 || nop

00000060 <rach_ds>:
  60:	50 84 f0 00 	rach a0, a1 || nop

00000064 <rach_dsi>:
  64:	54 81 f0 00 	rach a1, a0, #2 || nop

00000068 <bc__add>:
  68:	7c e6 8d ad 	bc 0 <bcl> || add fp,fp
  6c:	7c e5 0d ad 	bc 0 <bcl> -> add fp,fp

00000070 <bcl__addi>:
  70:	78 e4 cd 4d 	bcl 0 <bcl> || addi fp,#77
  74:	78 e3 cd 4d 	bcl 0 <bcl> || addi fp,#77

00000078 <bl__addv>:
  78:	7e e2 8d 8d 	bl 0 <bcl> || addv fp,fp
  7c:	7e e1 8d 8d 	bl 0 <bcl> || addv fp,fp

00000080 <bnc__addx>:
  80:	7d e0 8d 9d 	bnc 0 <bnc> || addx fp,fp
  84:	7d df 0d 9d 	bnc 0 <bcl> -> addx fp,fp

00000088 <bncl__and>:
  88:	79 de 8d cd 	bncl 0 <bcl> || and fp,fp
  8c:	79 dd 0d cd 	bncl 0 <bcl> -> and fp,fp

00000090 <bra__cmp>:
  90:	7f dc 8d 4d 	bra 0 <bcl> || cmp fp,fp
  94:	7f db 8d 4d 	bra 0 <bcl> || cmp fp,fp

00000098 <jl__cmpeq>:
  98:	1e cd 8d 6d 	jl fp || cmpeq fp,fp
  9c:	1e cd 8d 6d 	jl fp || cmpeq fp,fp

000000a0 <jmp__cmpu>:
  a0:	1f cd 8d 5d 	jmp fp || cmpu fp,fp
  a4:	1f cd 8d 5d 	jmp fp || cmpu fp,fp

000000a8 <ld__cmpz>:
  a8:	2d cd 80 71 	ld fp,@fp || cmpz r1
  ac:	2d cd 80 71 	ld fp,@fp || cmpz r1

000000b0 <ld__ldi>:
  b0:	2d e1 e2 4d 	ld fp,@r1\+ || ldi r2,#77
  b4:	2d e1 e2 4d 	ld fp,@r1\+ || ldi r2,#77

000000b8 <ldb__mv>:
  b8:	2d 8d 92 8d 	ldb fp,@fp || mv r2,fp
  bc:	2d 8d 12 8d 	ldb fp,@fp -> mv r2,fp

000000c0 <ldh__neg>:
  c0:	2d ad 82 3d 	ldh fp,@fp || neg r2,fp
  c4:	2d ad 02 3d 	ldh fp,@fp -> neg r2,fp

000000c8 <ldub__nop>:
  c8:	2d 9d f0 00 	ldub fp,@fp || nop
  cc:	2d 9d f0 00 	ldub fp,@fp || nop

000000d0 <lduh__not>:
  d0:	2d bd 82 bd 	lduh fp,@fp || not r2,fp
  d4:	2d bd 02 bd 	lduh fp,@fp -> not r2,fp

000000d8 <lock__or>:
  d8:	2d dd 82 ed 	lock fp,@fp || or r2,fp
  dc:	2d dd 02 ed 	lock fp,@fp -> or r2,fp

000000e0 <mvfc__sub>:
  e0:	1d 91 82 2d 	mvfc fp,cbr || sub r2,fp
  e4:	1d 91 02 2d 	mvfc fp,cbr -> sub r2,fp

000000e8 <mvtc__subv>:
  e8:	10 ad 82 0d 	mvtc fp,cr2 || subv r2,fp
  ec:	10 ad 82 0d 	mvtc fp,cr2 || subv r2,fp

000000f0 <rte__subx>:
  f0:	10 d6 82 2d 	rte || sub r2,fp
  f4:	10 d6 02 1d 	rte -> subx r2,fp

000000f8 <sll__xor>:
  f8:	1d 41 82 dd 	sll fp,r1 || xor r2,fp
  fc:	1d 41 02 dd 	sll fp,r1 -> xor r2,fp

00000100 <slli__machi>:
 100:	5d 56 b2 4d 	slli fp,#0x16 || machi r2,fp
 104:	5d 56 32 4d 	slli fp,#0x16 -> machi r2,fp

00000108 <sra__maclh1>:
 108:	1d 2d d2 cd 	sra fp,fp || maclh1 r2,fp
 10c:	1d 2d 52 cd 	sra fp,fp -> maclh1 r2,fp

00000110 <srai__maclo>:
 110:	5d 36 b2 5d 	srai fp,#0x16 || maclo r2,fp
 114:	5d 36 32 5d 	srai fp,#0x16 -> maclo r2,fp

00000118 <srl__macwhi>:
 118:	1d 0d b2 6d 	srl fp,fp || macwhi r2,fp
 11c:	1d 0d 32 6d 	srl fp,fp -> macwhi r2,fp

00000120 <srli__macwlo>:
 120:	5d 16 b2 7d 	srli fp,#0x16 || macwlo r2,fp
 124:	5d 16 32 7d 	srli fp,#0x16 -> macwlo r2,fp

00000128 <st__macwu1>:
 128:	2d 4d d2 bd 	st fp,@fp || macwu1 r2,fp
 12c:	2d 4d d2 bd 	st fp,@fp || macwu1 r2,fp

00000130 <st__msblo>:
 130:	2d 6d d2 dd 	st fp,@+fp || msblo r2,fp
 134:	2d 6d d2 dd 	st fp,@+fp || msblo r2,fp

00000138 <st__mul>:
 138:	2d 7d 92 6d 	st fp,@-fp || mul r2,fp
 13c:	2d 7d 92 6d 	st fp,@-fp || mul r2,fp

00000140 <stb__mulhi>:
 140:	2d 0d b2 0d 	stb fp,@fp || mulhi r2,fp
 144:	2d 0d b2 0d 	stb fp,@fp || mulhi r2,fp

00000148 <sth__mullo>:
 148:	2d 2d b2 1d 	sth fp,@fp || mullo r2,fp
 14c:	2d 2d b2 1d 	sth fp,@fp || mullo r2,fp

00000150 <trap__mulwhi>:
 150:	10 f2 b2 2d 	trap #0x2 || mulwhi r2,fp
 154:	10 f2 f0 00 	trap #0x2 || nop
 158:	32 2d f0 00 	mulwhi r2,fp || nop

0000015c <unlock__mulwlo>:
 15c:	2d 5d b2 3d 	unlock fp,@fp || mulwlo r2,fp
 160:	2d 5d b2 3d 	unlock fp,@fp || mulwlo r2,fp

00000164 <add__mulwu1>:
 164:	0d ad d2 ad 	add fp,fp || mulwu1 r2,fp
 168:	0d ad 52 ad 	add fp,fp -> mulwu1 r2,fp

0000016c <addi__mvfachi>:
 16c:	4d 4d d2 f0 	addi fp,#77 || mvfachi r2
 170:	4d 4d d2 f0 	addi fp,#77 || mvfachi r2

00000174 <addv__mvfaclo>:
 174:	0d 8d d2 f5 	addv fp,fp || mvfaclo r2,a1
 178:	0d 8d d2 f5 	addv fp,fp || mvfaclo r2,a1

0000017c <addx__mvfacmi>:
 17c:	0d 9d d2 f2 	addx fp,fp || mvfacmi r2
 180:	0d 9d d2 f2 	addx fp,fp || mvfacmi r2

00000184 <and__mvtachi>:
 184:	0d cd d2 70 	and fp,fp || mvtachi r2
 188:	0d cd d2 70 	and fp,fp || mvtachi r2

0000018c <cmp__mvtaclo>:
 18c:	0d 4d d2 71 	cmp fp,fp || mvtaclo r2
 190:	0d 4d d2 71 	cmp fp,fp || mvtaclo r2

00000194 <cmpeq__rac>:
 194:	0d 6d d0 94 	cmpeq fp,fp || rac a1
 198:	0d 6d d0 94 	cmpeq fp,fp || rac a1

0000019c <cmpu__rach>:
 19c:	0d 5d d0 84 	cmpu fp,fp || rach a0, a1
 1a0:	0d 5d d4 86 	cmpu fp,fp || rach a1, a1, #2

000001a4 <cmpz__sadd>:
 1a4:	00 7d d0 e4 	cmpz fp || sadd
 1a8:	00 7d d0 e4 	cmpz fp || sadd

000001ac <sc>:
 1ac:	74 01 d0 e4 	sc || sadd

000001b0 <snc>:
 1b0:	75 01 d0 e4 	snc || sadd

000001b4 <jc>:
 1b4:	1c cd f0 00 	jc fp || nop

000001b8 <jnc>:
 1b8:	1d cd f0 00 	jnc fp || nop

000001bc <pcmpbz>:
 1bc:	03 7d f0 00 	pcmpbz fp || nop

000001c0 <sat>:
 1c0:	8d 6d 00 00 	sat fp,fp

000001c4 <sath>:
 1c4:	8d 6d 02 00 	sath fp,fp

000001c8 <jc__pcmpbz>:
 1c8:	1c cd 83 7d 	jc fp || pcmpbz fp
 1cc:	1c cd 03 7d 	jc fp -> pcmpbz fp

000001d0 <jnc__ldi>:
 1d0:	1d cd ed 4d 	jnc fp || ldi fp,#77
 1d4:	1d cd 6d 4d 	jnc fp -> ldi fp,#77

000001d8 <sc__mv>:
 1d8:	74 01 9d 82 	sc || mv fp,r2
 1dc:	74 01 9d 82 	sc || mv fp,r2

000001e0 <snc__neg>:
 1e0:	75 01 8d 32 	snc || neg fp,r2
 1e4:	75 01 8d 32 	snc || neg fp,r2

000001e8 <nop__sadd>:
 1e8:	70 00 d0 e4 	nop || sadd

000001ec <sadd__nop>:
 1ec:	70 00 d0 e4 	nop || sadd

000001f0 <sadd__nop_reverse>:
 1f0:	70 00 d0 e4 	nop || sadd

000001f4 <add__not>:
 1f4:	00 a1 83 b5 	add r0,r1 || not r3,r5

000001f8 <add__not_dest_clash>:
 1f8:	03 a4 03 b5 	add r3,r4 -> not r3,r5

000001fc <add__not__src_clash>:
 1fc:	03 a4 05 b3 	add r3,r4 -> not r5,r3

00000200 <add__not__no_clash>:
 200:	03 a4 84 b5 	add r3,r4 || not r4,r5

00000204 <mul__sra>:
 204:	13 24 91 62 	sra r3,r4 || mul r1,r2

00000208 <mul__sra__reverse_src_clash>:
 208:	13 24 91 63 	sra r3,r4 || mul r1,r3

0000020c <bc__add_>:
 20c:	7c 04 01 a2 	bc 21c <label> -> add r1,r2

00000210 <add__bc>:
 210:	7c 03 83 a4 	bc 21c <label> || add r3,r4

00000214 <bc__add__forced_parallel>:
 214:	7c 02 85 a6 	bc 21c <label> || add r5,r6

00000218 <add__bc__forced_parallel>:
 218:	7c 01 87 a8 	bc 21c <label> || add r7,r8


