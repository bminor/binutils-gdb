
	.section	".data"
dsym0:	.llong	0xdeadbeef
dsym1:


	.section	".toc"
.L_tsym0:
	.tc	ignored0[TC],dsym0
.L_tsym1:
	.tc	ignored1[TC],dsym1
.L_tsym2:
	.tc	ignored2[TC],usym0
.L_tsym3:
	.tc	ignored3[TC],usym1
.L_tsym4:
	.tc	ignored4[TC],esym0
.L_tsym5:
	.tc	ignored5[TC],esym1


	.section	".text"
	lq	4,dsym0@l(3)
	lq	4,dsym1@l(3)
	lq	4,usym0@l(3)
	lq	4,usym1@l(3)
	lq	4,esym0@l(3)
	lq	4,esym1@l(3)
	lq	4,.L_tsym0@toc(2)
	lq	4,.L_tsym1@toc(2)
	lq	4,.L_tsym2@toc(2)
	lq	4,.L_tsym3@toc(2)
	lq	4,.L_tsym4@toc(2)
	lq	4,.L_tsym5@toc(2)
	lq	6,.L_tsym5@toc@l(2)
	lq	4,.text@l(0)	
	lq	6,dsym0@got(3)
	lq	6,dsym0@got@l(3)
	lq	6,dsym0@plt@l(3)
	lq	6,dsym1@sectoff(3)
	lq	6,dsym1@sectoff@l(3)
	lq	6,usym1-dsym0@l(4)
	stq 6,	0(7)
	stq 6,	16(7)
	stq 6,	-16(7)
	stq 6,	-32768(7)
	stq 6,	32752(7)
	attn

	.section	".data"
usym0:	.llong	0xcafebabe
usym1:

