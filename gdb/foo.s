	file	"foo.c"
	data	1
	text
	def	main;	val	main;	scl	2;	type	044;	endef
	global	main
main:
	ln	1
	def	~bf;	val	~;	scl	101;	line	1;	endef
	link.l	%fp,&F%1
	movm.l	&M%1,(4,%sp)
	fmovm	&FPM%1,(FPO%1,%sp)
	def	i;	val	-4+S%1;	scl	1;	type	04;	endef
	ln	4
	clr.l	((S%1-4).w,%fp)
L%15:
	tst.l	((S%1-4).w,%fp)
	blt	L%14
	ln	5
	jsr	bar
L%13:
	add.l	&1,((S%1-4).w,%fp)
	bra	L%15
L%14:
L%12:
	def	~ef;	val	~;	scl	101;	line	6;	endef
	ln	6
	movm.l	(4,%sp),&M%1
	fmovm	(FPO%1,%sp),&FPM%1
	unlk	%fp
	rts
	def	main;	val	~;	scl	-1;	endef
	set	S%1,0
	set	T%1,0
	set	F%1,-8
	set	FPO%1,4
	set	FPM%1,0x0000
	set	M%1,0x0000
	data	1
	text
	def	bar;	val	bar;	scl	2;	type	044;	endef
	global	bar
bar:
	ln	1
	def	~bf;	val	~;	scl	101;	line	9;	endef
	link.l	%fp,&F%2
	movm.l	&M%2,(4,%sp)
	fmovm	&FPM%2,(FPO%2,%sp)
	def	i;	val	-4+S%2;	scl	1;	type	04;	endef
	ln	4
	mov.l	&10,((S%2-4).w,%fp)
L%17:
	def	~ef;	val	~;	scl	101;	line	5;	endef
	ln	5
	movm.l	(4,%sp),&M%2
	fmovm	(FPO%2,%sp),&FPM%2
	unlk	%fp
	rts
	def	bar;	val	~;	scl	-1;	endef
	set	S%2,0
	set	T%2,0
	set	F%2,-8
	set	FPO%2,4
	set	FPM%2,0x0000
	set	M%2,0x0000
	data	1
