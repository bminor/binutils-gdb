
#
# Try to verify all operand modes for 68HC12
#
	sect .text
	globl start

start:
	anda	[12,x]
	ldaa	#10
	ldx	L1
L1:	ldy	,x
	addd	1,y
	subd	-1,y
	eora	15,y
	eora	-16,y
	eorb	16,y
	eorb	-17,y
	oraa	128,sp
	orab	-128,sp
	orab	255,x
	orab	-256,x
	anda	256,x
	andb	-257,x
	anda	[12,x]
	ldaa	[257,y]
	ldab	[32767,sp]
	ldd	[32768,pc]
	ldd	L1,pc
	std	a,x
	ldx	b,x
	stx	d,y
	addd	1,+x
	addd	2,+x
	addd	8,+x
	addd	1,sp+
	addd	2,sp+
	addd	8,sp+
	subd	1,-y
	subd	2,-y
	subd	8,-y
	addd	1,y-
	addd	2,y-
	addd	8,y-
	std	[d,x]
	std	[d,y]
	std	[d,sp]
	std	[d,pc]
	beq	L1
	lbeq	start
	lbcc	L2
	movb	start, 1,x
	movw	1,x, start
	movb	start, 1,+x
	movb	start, 1,-x
	movb	#23, 1,-sp
	movb	L1, L2
	movb	L1, a,x
	movw	L1, b,x
	movw	L1, d,x
	movw	d,x, a,x
	movw	b,sp, d,pc
	movw	b,sp, L1
	movw	b,sp, 1,x
	movw	d,x, a,y
	trap	#0x30
	trap	#0x39
	trap	#0x40
	trap	#0x80
	trap	#255
L2:	
	rts
