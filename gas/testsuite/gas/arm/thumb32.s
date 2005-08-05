	.text
	.thumb
	.syntax unified

encode_thumb32_immediate:
	orr	r0, r1, #0x00000000
	orr	r0, r1, #0x000000a5
	orr	r0, r1, #0x00a500a5
	orr	r0, r1, #0xa500a500
	orr	r0, r1, #0xa5a5a5a5

	orr	r0, r1, #0xa5 << 31
	orr	r0, r1, #0xa5 << 30
	orr	r0, r1, #0xa5 << 29
	orr	r0, r1, #0xa5 << 28
	orr	r0, r1, #0xa5 << 27
	orr	r0, r1, #0xa5 << 26
	orr	r0, r1, #0xa5 << 25
	orr	r0, r1, #0xa5 << 24
	orr	r0, r1, #0xa5 << 23
	orr	r0, r1, #0xa5 << 22
	orr	r0, r1, #0xa5 << 21
	orr	r0, r1, #0xa5 << 20
	orr	r0, r1, #0xa5 << 19
	orr	r0, r1, #0xa5 << 18
	orr	r0, r1, #0xa5 << 17
	orr	r0, r1, #0xa5 << 16
	orr	r0, r1, #0xa5 << 15
	orr	r0, r1, #0xa5 << 14
	orr	r0, r1, #0xa5 << 13
	orr	r0, r1, #0xa5 << 12
	orr	r0, r1, #0xa5 << 11
	orr	r0, r1, #0xa5 << 10
	orr	r0, r1, #0xa5 << 9
	orr	r0, r1, #0xa5 << 8
	orr	r0, r1, #0xa5 << 7
	orr	r0, r1, #0xa5 << 6
	orr	r0, r1, #0xa5 << 5
	orr	r0, r1, #0xa5 << 4
	orr	r0, r1, #0xa5 << 3
	orr	r0, r1, #0xa5 << 2
	orr	r0, r1, #0xa5 << 1

add_sub:
	adds	r0, r0, #0	@ format 1
	adds	r5, r0, #0
	adds	r0, r5, #0
	adds	r0, r0, #5

	adds	r0, #129	@ format 2
	adds	r0, r0, #129
	adds	r5, #126

	adds	r0, r0, r0	@ format 3
	adds	r5, r0, r0
	adds	r0, r5, r0
	adds	r0, r0, r5
	adds	r1, r2, r3

	add	r8, r0		@ format 4
	add	r0, r8
	add	r0, r8, r0
	add	r0, r0, r8
	add	r8, r0, r0	@ ... not this one

	add	r1, r0
	add	r0, r1

	add	r0, pc, #0	@ format 5
	add	r5, pc, #0
	add	r0, pc, #516

	add	r0, sp, #0	@ format 6
	add	r5, sp, #0
	add	r0, sp, #516

	add	sp, #0		@ format 7
	add	sp, sp, #0
	add	sp, #260

	add.w	r0, r0, #0	@ T32 format 1
	adds.w	r0, r0, #0
	add.w	r9, r0, #0
	add.w	r0, r9, #0
	add.w	r0, r0, #129

	add.w	r0, r0, r0	@ T32 format 2
	adds.w	r0, r0, r0
	add.w	r9, r0, r0
	add.w	r0, r9, r0
	add.w	r0, r0, r9

	add.w	r8, r9, r10
	add.w	r8, r9, r10, lsl #17
	add.w	r8, r8, r10, lsr #32
	add.w	r8, r8, r10, lsr #17
	add.w	r8, r9, r10, asr #32
	add.w	r8, r9, r10, asr #17
	add.w	r8, r9, r10, rrx
	add.w	r8, r9, r10, ror #17

	subs	r0, r0, #0	@ format 1
	subs	r5, r0, #0
	subs	r0, r5, #0
	subs	r0, r0, #5

	subs	r0, r0, #129
	subs	r5, #8

	subs	r0, r0, r0	@ format 3
	subs	r5, r0, r0
	subs	r0, r5, r0
	subs	r0, r0, r5

	sub	sp, #260	@ format 4
	sub	sp, sp, #260

	subs	r8, r0		@ T32 format 2
	subs	r0, r8
	subs	r0, #260	@ T32 format 1

arit3:
	.macro arit3 op ops opw opsw
	\ops	r0, r0
	\ops	r5, r0
	\ops	r0, r5
	\ops	r0, r0, r5
	\ops	r0, r5, r0
	\op	r0, r5, r0
	\op	r0, r1, r2
	\op	r9, r0, r0
	\op	r0, r9, r0
	\op	r0, r0, r9
	\opsw	r0, r0, r0
	\opw	r0, r1, r2, asr #17
	\opw	r0, r1, #129
	.endm

	arit3	adc adcs adc.w adcs.w
	arit3	and ands and.w ands.w
	arit3	bic bics bic.w bics.w
	arit3	eor eors eor.w eors.w
	arit3	orr orrs orr.w orrs.w
	arit3	rsb rsbs rsb.w rsbs.w
	arit3	sbc sbcs sbc.w sbcs.w

	.purgem arit3

bfc_bfi_bfx:
	bfc	r0, #0, #1
	bfc	r9, #0, #1
	bfi	r9, #0, #0, #1
	bfc	r0, #21, #1
	bfc	r0, #0, #18

	bfi	r0, r0, #0, #1
	bfi	r9, r0, #0, #1
	bfi	r0, r9, #0, #1
	bfi	r0, r0, #21, #1
	bfi	r0, r0, #0, #18

	sbfx	r0, r0, #0, #1
	ubfx	r9, r0, #0, #1
	sbfx	r0, r9, #0, #1
	ubfx	r0, r0, #21, #1
	sbfx	r0, r0, #0, #18

	.globl	branches
branches:
	.macro bra op
	\op	1b
	\op	1f
	.endm
1:
	bra	beq.n
	bra	bne.n
	bra	bcs.n
	bra	bhs.n
	bra	bcc.n
	bra	bul.n
	bra	blo.n
	bra	bmi.n
	bra	bpl.n
	bra	bvs.n
	bra	bvc.n
	bra	bhi.n
	bra	bls.n
	bra	bvc.n
	bra	bhi.n
	bra	bls.n
	bra	bge.n
	bra	blt.n
	bra	bgt.n
	bra	ble.n
	bra	bal.n
	bra	b.n
	@ bl, blx have no short form.
	.balign 4
1:
	bra	beq
	bra	bne
	bra	bcs
	bra	bhs
	bra	bcc
	bra	bul
	bra	blo
	bra	bmi
	bra	bpl
	bra	bvs
	bra	bvc
	bra	bhi
	bra	bls
	bra	bvc
	bra	bhi
	bra	bls
	bra	bge
	bra	blt
	bra	bgt
	bra	ble
	bra	b
	bra	bl
	bra	blx
	.balign 4
1:
	bx	r9
	blx	r0
	blx	r9
	bxj	r0
	bxj	r9
	.purgem bra

clz:
	clz	r0, r0
	clz	r9, r0
	clz	r0, r9

cps:
	cpsie	f
	cpsid	i
	cpsie	a
	cpsid.w	f
	cpsie.w	i
	cpsid.w	a
	cpsie	i, #0
	cpsid	i, #17
	cps	#0
	cps	#17

cpy:
	cpy	r0, r0
	cpy	r9, r0
	cpy	r0, r9
	cpy.w	r0, r0
	cpy.w	r9, r0
	cpy.w	r0, r9

czb:
	cbnz	r0, 2f
	cbz	r5, 1f

nop_hint:
	nop
1:	yield
2:	wfe
	wfi
	sev

	nop.w
	yield.w
	wfe.w
	wfi.w
	sev.w

	nop {9}
	nop {129}

it:
	.macro	itx opc cond n
	\opc	\cond
	.rept	\n
	nop
	.endr
	.endm

	itx	it eq 1
	itx	it ne 1
	itx	it cs 1
	itx	it hs 1
	itx	it cc 1
	itx	it ul 1
	itx	it lo 1
	itx	it mi 1
	itx	it pl 1
	itx	it vs 1
	itx	it vc 1
	itx	it hi 1
	itx	it ge 1
	itx	it lt 1
	itx	it gt 1
	itx	it le 1
	itx	it al 1

	itx	itt eq 2
	itx	ite eq 2
	itx	ittt eq 3
	itx	itet eq 3
	itx	itte eq 3
	itx	itee eq 3
	itx	itttt eq 4
	itx	itett eq 4
	itx	ittet eq 4
	itx	ittte eq 4
	itx	ittee eq 4
	itx	itete eq 4
	itx	iteet eq 4
	itx	iteee eq 4
	
	itx	itt ne 2
	itx	ite ne 2
	itx	ittt ne 3
	itx	itet ne 3
	itx	itte ne 3
	itx	itee ne 3
	itx	itttt ne 4
	itx	itett ne 4
	itx	ittet ne 4
	itx	ittte ne 4
	itx	ittee ne 4
	itx	itete ne 4
	itx	iteet ne 4
	itx	iteee ne 4

	.purgem itx

ldst:
	.macro	ls op
	\op	r1, [r5]
	\op	r1, [r5, #0x330]
	\op	r1, [r5, #-0x30]
	\op	r1, [r5], #0x30
	\op	r1, [r5], #-0x30
	\op	r1, [r5, #0x30]!
	\op	r1, [r5, #-0x30]!
	\op	r1, [r5, r4]
	\op	r1, [r9, ip]
	\op	r1, 1f
	\op	r1, 1b
	.endm
1:
	ls	ldrb
	ls	ldrsb
	ls	ldrh
	ls	ldrsh
	ls	ldr
1:
	ls	strb
	ls	strh
	ls	str

	pld	[r5]
	pld	[r5, #0x330]
	pld	[r5, #-0x30]
	pld	[r5], #0x30
	pld	[r5], #-0x30
	pld	[r5, #0x30]!
	pld	[r5, #-0x30]!
	pld	[r5, r4]
	pld	[r9, ip]
	pld	1f
	pld	1b
1:

	ldrd	r2, r3, [r5]
	ldrd	r2, [r5, #0x30]
	ldrd	r2, [r5, #-0x30]
	strd	r2, r3, [r5]
	strd	r2, [r5, #0x30]
	strd	r2, [r5, #-0x30]

	ldrbt	r1, [r5]
	ldrbt	r1, [r5, #0x30]
	ldrsbt	r1, [r5]
	ldrsbt	r1, [r5, #0x30]
	ldrht	r1, [r5]
	ldrht	r1, [r5, #0x30]
	ldrsht	r1, [r5]
	ldrsht	r1, [r5, #0x30]
	ldrt	r1, [r5]
	ldrt	r1, [r5, #0x30]

	.purgem ls

ldxstx:
	ldrexb	r1, [r4]
	ldrexh	r1, [r4]
	ldrex	r1, [r4]
	ldrexd	r1, r2, [r4]

	strexb	r1, r2, [r4]
	strexh	r1, r2, [r4]
	strex	r1, r2, [r4]
	strexd	r1, r2, r3, [r4]

	ldrex	r1, [r4,#516]
	strex	r1, r2, [r4,#516]

ldmstm:
	ldmia	r0!, {r1,r2,r3}
	ldmia	r2, {r0,r1,r2}
	ldmia.w	r2, {r0,r1,r2}
	ldmia	r9, {r0,r1,r2}
	ldmia	r0, {r7,r8,r10}
	ldmia	r0!, {r7,r8,r10}
	
	stmia	r0!, {r1,r2,r3}
	stmia	r2!, {r0,r1,r3}
	stmia.w	r2!, {r0,r1,r3}
	stmia	r9, {r0,r1,r2}
	stmia	r0, {r7,r8,r10}
	stmia	r0!, {r7,r8,r10}

	ldmdb	r0, {r7,r8,r10}
	stmdb	r0, {r7,r8,r10}

mlas:
	mla	r0, r0, r0, r0
	mls	r0, r0, r0, r0
	mla	r9, r0, r0, r0
	mla	r0, r9, r0, r0
	mla	r0, r0, r9, r0
	mla	r0, r0, r0, r9

tst_teq_cmp_cmn_mov_mvn:
	.macro	mt op ops opw opsw
	\ops	r0, r0
	\op	r0, r0
	\ops	r5, r0
	\op	r0, r5
	\op	r0, r5, asr #17
	\opw	r0, r0
	\ops	r9, r0
	\opsw	r0, r9
	\op	r0, #129
	\op	r5, #129
	.endm

	mt	tst tsts tst.w tsts.w
	mt	teq teqs teq.w teqs.w
	mt	cmp cmps cmp.w cmps.w
	mt	cmn cmns cmn.w cmns.w
	mt	mov movs mov.w movs.w
	mt	mvn mvns mvn.w mvns.w
	.purgem mt

mov16:
	movw	r0, #0
	movt	r0, #0
	movw	r9, #0
	movw	r0, #0x9000
	movw	r0, #0x0800
	movw	r0, #0x0500
	movw	r0, #0x0081
	movw	r0, #0xffff

mrs_msr:
	mrs	r0, CPSR
	mrs	r0, SPSR
	mrs	r9, CPSR_all
	mrs	r9, SPSR_all

	msr	CPSR_c, r0
	msr	SPSR_c, r0
	msr	CPSR_c, r9
	msr	CPSR_x, r0
	msr	CPSR_s, r0
	msr	CPSR_f, r0

mul:
	mul	r0, r0, r0
	mul	r0, r9, r0
	mul	r0, r0, r9
	mul	r0, r0
	mul	r9, r0
	muls	r5, r0
	muls	r5, r0, r5
	muls	r0, r5

mull:
	smull	r0, r1, r0, r0
	umull	r0, r1, r0, r0
	smlal	r0, r1, r0, r0
	umlal	r0, r1, r0, r0
	smull	r9, r0, r0, r0
	smull	r0, r9, r0, r0
	smull	r0, r1, r9, r0
	smull	r0, r1, r0, r9

neg:
	negs	r0, r0
	negs	r0, r5
	negs	r5, r0
	negs.w	r0, r0
	negs.w	r5, r0
	negs.w	r0, r5

	neg	r0, r9
	neg	r9, r0
	negs	r0, r9
	negs	r9, r0

pkh:
	pkhbt	r0, r0, r0
	pkhbt	r9, r0, r0
	pkhbt	r0, r9, r0
	pkhbt	r0, r0, r9
	pkhbt	r0, r0, r0, lsl #0x14
	pkhbt	r0, r0, r0, lsl #3
	pkhtb	r1, r2, r3
	pkhtb	r1, r2, r3, asr #0x11

push_pop:
	push	{r0}
	pop	{r0}
	push	{r1,lr}
	pop	{r1,pc}
	push	{r8,r9,r10,r11,r12}
	pop	{r8,r9,r10,r11,r12}

qadd:
	qadd16		r1, r2, r3
	qadd8		r1, r2, r3
	qaddsubx	r1, r2, r3
	qsub16		r1, r2, r3
	qsub8		r1, r2, r3
	qsubaddx	r1, r2, r3
	sadd16		r1, r2, r3
	sadd8		r1, r2, r3
	saddsubx	r1, r2, r3
	ssub16		r1, r2, r3
	ssub8		r1, r2, r3
	ssubaddx	r1, r2, r3
	shadd16		r1, r2, r3
	shadd8		r1, r2, r3
	shaddsubx	r1, r2, r3
	shsub16		r1, r2, r3
	shsub8		r1, r2, r3
	shsubaddx	r1, r2, r3
	uadd16		r1, r2, r3
	uadd8		r1, r2, r3
	uaddsubx	r1, r2, r3
	usub16		r1, r2, r3
	usub8		r1, r2, r3
	usubaddx	r1, r2, r3
	uhadd16		r1, r2, r3
	uhadd8		r1, r2, r3
	uhaddsubx	r1, r2, r3
	uhsub16		r1, r2, r3
	uhsub8		r1, r2, r3
	uhsubaddx	r1, r2, r3
	uqadd16		r1, r2, r3
	uqadd8		r1, r2, r3
	uqaddsubx	r1, r2, r3
	uqsub16		r1, r2, r3
	uqsub8		r1, r2, r3
	uqsubaddx	r1, r2, r3
	sel		r1, r2, r3

rbit_rev:
	.macro	rx op opw
	\op	r0, r0
	\opw	r0, r0
	\op	r0, r5
	\op	r5, r0
	\op	r0, r9
	\op	r9, r0
	.endm

	rx	rev rev.w
	rx	rev16 rev16.w
	rx	revsh revsh.w
	rx	rbit rbit.w

	.purgem rx

shift:
	.macro	sh op ops opw opsw
	\ops	r0, #17		@ 16-bit format 1
	\ops	r0, r0, #14
	\ops	r5, r0, #17
	\ops	r0, r5, #14
	\ops	r0, r0		@ 16-bit format 2
	\ops	r0, r5
	\ops	r0, r0, r5
	\op	r9, #17		@ 32-bit format 1
	\op	r9, r9, #14
	\ops	r0, r9, #17
	\op	r9, r0, #14
	\opw	r0, r0, r0	@ 32-bit format 2
	\op	r9, r9
	\ops	r9, r0
	\op	r0, r9
	\op	r0, r5
	\ops	r0, r1, r2
	.endm

	sh	lsl lsls lsl.w lsls.w
	sh	lsr lsrs lsr.w lsrs.w
	sh	asr asrs asr.w asrs.w
	sh	ror rors ror.w rors.w

	.purgem sh

smi:
	smi	#0
	smi	#0xabcd

smla:
	smlabb	r0, r0, r0, r0
	smlabb	r9, r0, r0, r0
	smlabb	r0, r9, r0, r0
	smlabb	r0, r0, r9, r0
	smlabb	r0, r0, r0, r9

	smlatb	r0, r0, r0, r0
	smlabt	r0, r0, r0, r0
	smlatt	r0, r0, r0, r0
	smlawb	r0, r0, r0, r0
	smlawt	r0, r0, r0, r0
	smlad	r0, r0, r0, r0
	smladx	r0, r0, r0, r0
	smlsd	r0, r0, r0, r0
	smlsdx	r0, r0, r0, r0
	smmla	r0, r0, r0, r0
	smmlar	r0, r0, r0, r0
	smmls	r0, r0, r0, r0
	smmlsr	r0, r0, r0, r0
	usada8	r0, r0, r0, r0

smlal:
	smlalbb	r0, r0, r0, r0
	smlalbb	r9, r0, r0, r0
	smlalbb	r0, r9, r0, r0
	smlalbb	r0, r0, r9, r0
	smlalbb	r0, r0, r0, r9

	smlaltb	r0, r0, r0, r0
	smlalbt	r0, r0, r0, r0
	smlaltt	r0, r0, r0, r0
	smlald	r0, r0, r0, r0
	smlaldx	r0, r0, r0, r0
	smlsld	r0, r0, r0, r0
	smlsldx	r0, r0, r0, r0
	umaal	r0, r0, r0, r0

smul:
	smulbb	r0, r0, r0
	smulbb	r9, r0, r0
	smulbb	r0, r9, r0
	smulbb	r0, r0, r9

	smultb	r0, r0, r0
	smulbt	r0, r0, r0
	smultt	r0, r0, r0
	smulwb	r0, r0, r0
	smulwt	r0, r0, r0
	smmul	r0, r0, r0
	smmulr	r0, r0, r0
	smuad	r0, r0, r0
	smuadx	r0, r0, r0
	smusd	r0, r0, r0
	smusdx	r0, r0, r0
	usad8	r0, r0, r0

sat:
	ssat	r0, #1, r0
	ssat	r0, #1, r0, lsl #0
	ssat	r0, #1, r0, asr #0
	ssat	r9, #1, r0
	ssat	r0, #18, r0
	ssat	r0, #1, r9
	ssat	r0, #1, r0, lsl #0x1c
	ssat	r0, #1, r0, asr #0x03

	ssat16	r0, #1, r0
	ssat16	r9, #1, r0
	ssat16	r0, #10, r0
	ssat16	r0, #1, r9

	usat	r0, #0, r0
	usat	r0, #0, r0, lsl #0
	usat	r0, #0, r0, asr #0
	usat	r9, #0, r0
	usat	r0, #17, r0
	usat	r0, #0, r9
	usat	r0, #0, r0, lsl #0x1c
	usat	r0, #0, r0, asr #0x03

	usat16	r0, #0, r0
	usat16	r9, #0, r0
	usat16	r0, #9, r0
	usat16	r0, #0, r9

xt:
	sxtb	r0, r0
	sxtb	r0, r0, ror #0
	sxtb	r5, r0
	sxtb	r0, r5
	sxtb.w	r1, r2
	sxtb	r1, r2, ror #8
	sxtb	r1, r2, ror #16
	sxtb	r1, r2, ror #24

	sxtb16	r1, r2
	sxtb16	r8, r9
	sxth	r1, r2
	sxth	r8, r9
	uxtb	r1, r2
	uxtb	r8, r9
	uxtb16	r1, r2
	uxtb16	r8, r9
	uxth	r1, r2
	uxth	r8, r9

xta:	
	sxtab	r0, r0, r0
	sxtab	r0, r0, r0, ror #0
	sxtab	r9, r0, r0, ror #8
	sxtab	r0, r9, r0, ror #16
	sxtab	r0, r0, r9, ror #24

	sxtab16	r1, r2, r3
	sxtah	r1, r2, r3
	uxtab	r1, r2, r3
	uxtab16	r1, r2, r3
	uxtah	r1, r2, r3

	.macro	ldpcimm op
	\op	r1, [pc, #0x2aa]
	\op	r1, [pc, #0x155]
	\op	r1, [pc, #-0x2aa]
	\op	r1, [pc, #-0x155]
	.endm
	ldpcimm	ldrb
	ldpcimm	ldrsb
	ldpcimm	ldrh
	ldpcimm	ldrsh
	ldpcimm	ldr
	addw r9, r0, #0
	addw r6, pc, #0xfff
	subw r6, r9, #0xa85
	subw r6, r9, #0x57a
	tbb [pc, r6]
	tbb [r0, r9]
	tbh [pc, r7, lsl #1]
	tbh [r0, r8, lsl #1]

	push	{r8}
	pop	{r8}

	ldmdb	r0!, {r7,r8,r10}
	stmdb	r0!, {r7,r8,r10}
