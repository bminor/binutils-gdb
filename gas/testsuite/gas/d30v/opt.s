	.text

	abs	r1,r2
	abs	r3,r4

	notfg	f0,f4
	notfg	f1,f2
		
	abs	r1,r2
	notfg	f1,f2

# both change C flag
	add	r1,r2,r3
	notfg	C,f0

# one uses and one changes C flag	
	add	r1,r2,r3
	notfg	f0,C

	bra	.
	abs	r1,r2

	abs	r1,r2	
	bra	.

	bsr	.
	abs	r1,r2
	
	abs	r1,r2	
	abs	r1,r2	
	bsr	.
	

	ldb	r1,@(r2,r3)
	stb	r7,@(r8,r9)

	stb	r7,@(r8,r9)	
	ldb	r1,@(r2,r3)

	ldb	r7,@(r8,r9)	
	ldb	r1,@(r2,r3)

	stb	r7,@(r8,r9)	
	stb	r1,@(r2,r3)
					