#as: -m32rx
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

00000010 <machl1>:
  10:	5d cd f0 00 	machl1 fp,fp || nop

00000014 <msblo>:
  14:	5d dd f0 00 	msblo fp,fp || nop

00000018 <mulwu1>:
  18:	5d ad f0 00 	mulwu1 fp,fp || nop

0000001c <macwu1>:
  1c:	5d bd f0 00 	macwu1 fp,fp || nop

00000020 <sadd>:
  20:	50 e4 f0 00 	sadd || nop

00000024 <satb>:
  24:	8d 0d 01 00 	satb fp,fp

00000028 <mulhi>:
  28:	3d 8d f0 00 	 || nop

0000002c <mullo>:
  2c:	3d 1d f0 00 	mullo fp,fp || nop

00000030 <mulwhi>:
  30:	3d ad f0 00 	 || nop

00000034 <mulwlo>:
  34:	3d 3d f0 00 	mulwlo fp,fp || nop

00000038 <machi>:
  38:	3d cd f0 00 	 || nop

0000003c <maclo>:
  3c:	3d 5d f0 00 	maclo fp,fp || nop

00000040 <macwhi>:
  40:	3d ed f0 00 	 || nop

00000044 <macwlo>:
  44:	3d 7d f0 00 	macwlo fp,fp || nop

00000048 <mvfachi>:
  48:	5d f0 f0 00 	mvfachi fp || nop

0000004c <mvfacmi>:
  4c:	5d f2 f0 00 	mvfacmi fp || nop

00000050 <mvfaclo>:
  50:	5d f1 f0 00 	mvfaclo fp || nop

00000054 <mvtachi>:
  54:	5d f0 f0 00 	mvfachi fp || nop

00000058 <mvtaclo>:
  58:	5d 71 f0 00 	mvtaclo fp || nop

0000005c <rac>:
  5c:	50 90 f0 00 	rac || nop

00000060 <rach>:
  60:	50 80 f0 00 	rach || nop

00000064 <bc__add>:
  64:	7c e7 8d ad 	bc 0 <bcl> || add fp,fp
  68:	7c e6 0d ad 	bc 0 <bcl> -> add fp,fp

0000006c <bcl__addi>:
  6c:	78 e5 cd 4d 	bcl 0 <bcl> || addi fp,#77
  70:	78 e4 cd 4d 	bcl 0 <bcl> || addi fp,#77

00000074 <bl__addv>:
  74:	7e e3 8d 8d 	bl 0 <bcl> || addv fp,fp
  78:	7e e2 8d 8d 	bl 0 <bcl> || addv fp,fp

0000007c <bnc__addx>:
  7c:	7d e1 8d 9d 	bnc 0 <bnc> || addx fp,fp
  80:	7d e0 0d 9d 	bnc 0 <bcl> -> addx fp,fp

00000084 <bncl__and>:
  84:	79 df 8d cd 	bncl 0 <bcl> || and fp,fp
  88:	79 de 0d cd 	bncl 0 <bcl> -> and fp,fp

0000008c <bra__cmp>:
  8c:	7f dd 8d 4d 	bra 0 <bcl> || cmp fp,fp
  90:	7f dc 8d 4d 	bra 0 <bcl> || cmp fp,fp

00000094 <jl__cmpeq>:
  94:	1e cd 8d 6d 	jl fp || cmpeq fp,fp
  98:	1e cd 8d 6d 	jl fp || cmpeq fp,fp

0000009c <jmp__cmpu>:
  9c:	1f cd 8d 5d 	jmp fp || cmpu fp,fp
  a0:	1f cd 8d 5d 	jmp fp || cmpu fp,fp

000000a4 <ld__cmpz>:
  a4:	2d cd 80 71 	ld fp,@fp || cmpz r1
  a8:	2d cd 80 71 	ld fp,@fp || cmpz r1

000000ac <ld__ldi>:
  ac:	2d e1 e2 4d 	ld fp,@r1\+ || ldi r2,#77
  b0:	2d e1 e2 4d 	ld fp,@r1\+ || ldi r2,#77

000000b4 <ldb__mv>:
  b4:	2d 8d 92 8d 	ldb fp,@fp || mv r2,fp
  b8:	2d 8d 12 8d 	ldb fp,@fp -> mv r2,fp

000000bc <ldh__neg>:
  bc:	2d ad 82 3d 	ldh fp,@fp || neg r2,fp
  c0:	2d ad 02 3d 	ldh fp,@fp -> neg r2,fp

000000c4 <ldub__nop>:
  c4:	2d 9d f0 00 	ldub fp,@fp || nop
  c8:	2d 9d f0 00 	ldub fp,@fp || nop

000000cc <lduh__not>:
  cc:	2d bd 82 bd 	lduh fp,@fp || not r2,fp
  d0:	2d bd 02 bd 	lduh fp,@fp -> not r2,fp

000000d4 <lock__or>:
  d4:	2d dd 82 ed 	lock fp,@fp || or r2,fp
  d8:	2d dd 02 ed 	lock fp,@fp -> or r2,fp

000000dc <mvfc__sub>:
  dc:	1d 91 82 2d 	mvfc fp,cbr || sub r2,fp
  e0:	1d 91 02 2d 	mvfc fp,cbr -> sub r2,fp

000000e4 <mvtc__subv>:
  e4:	10 ad 82 0d 	mvtc fp,psw || subv r2,fp
  e8:	10 ad 82 0d 	mvtc fp,psw || subv r2,fp

000000ec <rte__subx>:
  ec:	10 d6 82 1d 	rte || subx r2,fp
  f0:	10 d6 02 1d 	rte -> subx r2,fp

000000f4 <sll__xor>:
  f4:	1d 41 82 dd 	sll fp,r1 || xor r2,fp
  f8:	1d 41 02 dd 	sll fp,r1 -> xor r2,fp

000000fc <slli__machi>:
  fc:	5d 56 b2 4d 	slli fp,#0x16 || machi r2,fp
 100:	5d 56 32 4d 	slli fp,#0x16 -> machi r2,fp

00000104 <sra__machl1>:
 104:	1d 2d d2 cd 	sra fp,fp || machl1 r2,fp
 108:	1d 2d 52 cd 	sra fp,fp -> machl1 r2,fp

0000010c <srai__maclo>:
 10c:	5d 36 b2 5d 	srai fp,#0x16 || maclo r2,fp
 110:	5d 36 32 5d 	srai fp,#0x16 -> maclo r2,fp

00000114 <srl__macwhi>:
 114:	1d 0d b2 6d 	srl fp,fp || macwhi r2,fp
 118:	1d 0d 32 6d 	srl fp,fp -> macwhi r2,fp

0000011c <srli__macwlo>:
 11c:	5d 16 b2 7d 	srli fp,#0x16 || macwlo r2,fp
 120:	5d 16 32 7d 	srli fp,#0x16 -> macwlo r2,fp

00000124 <st__macwu1>:
 124:	2d 4d d2 bd 	st fp,@fp || macwu1 r2,fp
 128:	2d 4d d2 bd 	st fp,@fp || macwu1 r2,fp

0000012c <st__msblo>:
 12c:	2d 6d d2 dd 	st fp,@+fp || msblo r2,fp
 130:	2d 6d d2 dd 	st fp,@+fp || msblo r2,fp

00000134 <st__mul>:
 134:	2d 7d 92 6d 	st fp,@-fp || mul r2,fp
 138:	2d 7d 92 6d 	st fp,@-fp || mul r2,fp

0000013c <stb__mulhi>:
 13c:	2d 0d b2 0d 	stb fp,@fp || mulhi r2,fp
 140:	2d 0d b2 0d 	stb fp,@fp || mulhi r2,fp

00000144 <sth__mullo>:
 144:	2d 2d b2 1d 	sth fp,@fp || mullo r2,fp
 148:	2d 2d b2 1d 	sth fp,@fp || mullo r2,fp

0000014c <trap__mulwhi>:
 14c:	10 f2 b2 2d 	trap #0x2 || mulwhi r2,fp
 150:	10 f2 f0 00 	trap #0x2 || nop
 154:	32 2d f0 00 	mulwhi r2,fp || nop

00000158 <unlock__mulwlo>:
 158:	2d 5d b2 3d 	unlock fp,@fp || mulwlo r2,fp
 15c:	2d 5d b2 3d 	unlock fp,@fp || mulwlo r2,fp

00000160 <add__mulwu1>:
 160:	0d ad d2 ad 	add fp,fp || mulwu1 r2,fp
 164:	0d ad 52 ad 	add fp,fp -> mulwu1 r2,fp

00000168 <addi__mvfachi>:
 168:	4d 4d d2 f0 	addi fp,#77 || mvfachi r2
 16c:	4d 4d d2 f0 	addi fp,#77 || mvfachi r2

00000170 <addv__mvfaclo>:
 170:	0d 8d d2 f5 	addv fp,fp || mvfaclo r2,a1
 174:	0d 8d d2 f5 	addv fp,fp || mvfaclo r2,a1

00000178 <addx__mvfacmi>:
 178:	0d 9d d2 f2 	addx fp,fp || mvfacmi r2
 17c:	0d 9d d2 f2 	addx fp,fp || mvfacmi r2

00000180 <and__mvtachi>:
 180:	0d cd d2 70 	and fp,fp || mvtachi r2
 184:	0d cd d2 70 	and fp,fp || mvtachi r2

00000188 <cmp__mvtaclo>:
 188:	0d 4d d2 71 	cmp fp,fp || mvtaclo r2
 18c:	0d 4d d2 71 	cmp fp,fp || mvtaclo r2

00000190 <cmpeq__rac>:
 190:	0d 6d d0 94 	cmpeq fp,fp || rac a1
 194:	0d 6d d0 94 	cmpeq fp,fp || rac a1

00000198 <cmpu__rach>:
 198:	0d 5d d0 80 	cmpu fp,fp || rach
 19c:	0d 5d d0 80 	cmpu fp,fp || rach

000001a0 <cmpz__sadd>:
 1a0:	00 7d d0 e4 	cmpz fp || sadd
 1a4:	00 7d d0 e4 	cmpz fp || sadd

000001a8 <sc>:
 1a8:	74 01 d0 e4 	sc || sadd

000001ac <snc>:
 1ac:	75 01 d0 e4 	snc || sadd

000001b0 <jc>:
 1b0:	1c cd f0 00 	jc fp || nop

000001b4 <jnc>:
 1b4:	1d cd f0 00 	*unknown* || nop

000001b8 <pcmpbz>:
 1b8:	03 7d f0 00 	pcmpbz fp || nop

000001bc <sat>:
 1bc:	8d 0d 00 00 	sat fp,fp

000001c0 <sath>:
 1c0:	8d 0d 02 00 	sath fp,fp

000001c4 <jc__pcmpbz>:
 1c4:	1c cd 83 7d 	jc fp || pcmpbz fp
 1c8:	1c cd 03 7d 	jc fp -> pcmpbz fp

000001cc <jnc__ldi>:
 1cc:	1d cd ed 4d 	\*unknown\* || ldi fp,#77
 1d0:	1d cd 6d 4d 	\*unknown\* -> ldi fp,#77

000001d4 <sc__mv>:
 1d4:	74 01 9d 82 	sc || mv fp,r2
 1d8:	74 01 9d 82 	sc || mv fp,r2

000001dc <snc__neg>:
 1dc:	75 01 8d 32 	snc || neg fp,r2
 1e0:	75 01 8d 32 	snc || neg fp,r2

000001e4 <nop__sadd>:
 1e4:	70 00 d0 e4 	nop || sadd

000001e8 <sadd__nop>:
 1e8:	70 00 d0 e4 	nop || sadd

000001ec <sadd__nop_reverse>:
 1ec:	70 00 d0 e4 	nop || sadd

000001f0 <add__not>:
 1f0:	00 a1 83 b5 	add r0,r1 || not r3,r5

000001f4 <add__not_dest_clash>:
 1f4:	03 a4 03 b5 	add r3,r4 -> not r3,r5

000001f8 <add__not__src_clash>:
 1f8:	03 a4 05 b3 	add r3,r4 -> not r5,r3

000001fc <add__not__no_clash>:
 1fc:	03 a4 84 b5 	add r3,r4 || not r4,r5

00000200 <mul__sra>:
 200:	13 24 91 62 	sra r3,r4 || mul r1,r2

00000204 <mul__sra__reverse_src_clash>:
 204:	13 24 91 63 	sra r3,r4 || mul r1,r3

00000208 <bc__add_>:
 208:	7c f7 01 a2 	bc 1e4 <nop__sadd> -> add r1,r2

0000020c <add__bc>:
 20c:	7c f6 83 a4 	bc 1e4 <nop__sadd> || add r3,r4

00000210 <bc__add__forced_parallel>:
 210:	7c f5 85 a6 	bc 1e4 <nop__sadd> || add r5,r6

00000214 <add__bc__forced_parallel>:
 214:	7c f4 87 a8 	bc 1e4 <nop__sadd> || add r7,r8


