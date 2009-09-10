# asl test (BUG)

	asl r0,r1
	asl fp,sp

	asl r0,0
	asl r1,-1
	asl 0,r2
	# bug asl -1,r3
	asl r4,255
	# bug asl 255,r5
	asl r6,-256
	#bug asl -256,r7

	asl r8,256
	asl r9,-257
	asl r11,0x42424242

	# bug asl 255,256

	asl r0,foo

	asl r0,r1
	asl r3,r4
	asl r6,r7
	asl r9,r10
	asl r12,r13
	asl r15,r16
	asl r18,r19
	asl r21,r22
	asl r24,r25
	asl r27,r28
	asl r30,r31
	asl 0,r34
	asl 0,r37
	asl 0,r40
	asl 0,r43
	asl 0,r46
	asl 0,r49
	asl 0,r50
	asl 0,r55
	asl 0,r58
	# bug asl.gt r60,r60
	#asl.ge r0,0
	# bug asl.le 2,2
	# bug asl.hi r3,r3
	# bug asl.ls r4,r4
	# bug asl.pnz r5,r5

	asl.f r0,r1
	asl.f r2,1
	# bug asl.f 1,r3
	asl.f 0,r4
	asl.f r5,512
	# bug asl.f 512,512

	#asl.eq.f r0,r1
	#asl.ne.f r1,0
