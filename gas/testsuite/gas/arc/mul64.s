# mul64 test
	.cpu host

# reg,reg
	mul64 r1,r2
	mulu64 r3,r4

# shimm values
	mul64 r1,0
	mul64 0,r2
	mul64 r1,-1
	mul64 -1,r2
	mul64 r1,255
	mul64 255,r2
	mul64 r1,-256
	mul64 -256,r2
	mulu64 r1,256
	mulu64 511,r2

# limm values
	mul64 r1,256
	mul64 -257,r2
	mulu64 r3,512
	mulu64 r4,-1
	mul64 0x42424242,r5
	mul64 0x12345678,0x12345678

# symbols
	mul64 r0,foo
	mulu64 r32,foo

# conditional execution
	mul64.al r0,r1
	mul64.eq r6,r7
	mulu64.ne r12,r13

# flag setting
	mul64.f r0,r1
	mul64.f r2,1
	mul64.f 1,r3
	mul64.f 0,r4
	mulu64.f r5,512
	mulu64.f r6,512
	mulu64.f r7,r8

# conditional execution + flag setting
	mulu64.eq.f r0,r1
	mulu64.ne.f r2,0
	mulu64.lt.f 0,r3
	mulu64.gt.f r4,512
	mulu64.le.f 512,r5
	mulu64.ge.f 512,512
