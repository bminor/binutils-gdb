# sh testcase for movua
# mach:	 all
# as(sh):	-defsym sim_cpu=0
# as(shdsp):	-defsym sim_cpu=1 -dsp 

	.include "testutils.inc"

	start
movua_1:
	set_grs_a5a5
	mov.l	srcp, r1
	movua.l	@r1, r0
	assertreg0	0x00010203

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x01020304

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x02030405

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x03040506

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x04050607

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x05060708

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x06070809

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x0708090a

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x08090a0b

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x090a0b0c

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x0a0b0c0d

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x0b0c0d0e

	add	#1, r1
	movua.l	@r1, r0
	assertreg0	0x0c0d0e0f

	assertreg	src+12, r1
	test_gr_a5a5	r2
	test_gr_a5a5	r3
	test_gr_a5a5	r4
	test_gr_a5a5	r5
	test_gr_a5a5	r6
	test_gr_a5a5	r7
	test_gr_a5a5	r8
	test_gr_a5a5	r9
	test_gr_a5a5	r10
	test_gr_a5a5	r11
	test_gr_a5a5	r12
	test_gr_a5a5	r13
	test_gr_a5a5	r14

	bra	movua_4:
	nop
	
	.align 0
src:	.byte	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
	.align 2
srcp:	.long	src

movua_4:
	set_grs_a5a5
	mov.l	srcp2, r1
	movua.l	@r1+, r0
	assertreg0	0x00010203
	assertreg	src+4, r1

	mov.l	srcp2, r1
	add	#1, r1
	movua.l	@r1+, r0
	assertreg0	0x01020304
	assertreg	src+5, r1

	mov.l	srcp2, r1
	add	#2, r1
	movua.l	@r1+, r0
	assertreg0	0x02030405
	assertreg	src+6, r1

	mov.l	srcp2, r1
	add	#3, r1
	movua.l	@r1+, r0
	assertreg0	0x03040506
	assertreg	src+7, r1

	test_gr_a5a5	r2
	test_gr_a5a5	r3
	test_gr_a5a5	r4
	test_gr_a5a5	r5
	test_gr_a5a5	r6
	test_gr_a5a5	r7
	test_gr_a5a5	r8
	test_gr_a5a5	r9
	test_gr_a5a5	r10
	test_gr_a5a5	r11
	test_gr_a5a5	r12
	test_gr_a5a5	r13
	test_gr_a5a5	r14

	pass
	exit 0

srcp2:	.long	src

