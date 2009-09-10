# add test

	add r0,r1,r2
	add r26,fp,sp
	add ilink1,ilink2,blink
	#add r56,r59,lp_count

	add r0,r1,0
	add r0,0,r2
	add 0,r1,r2
	add r0,r1,-1
	add r0,-1,r2
	#add -1,r1,r2
	add r0,r1,255
	add r0,255,r2
	#add 255,r1,r2
	add r0,r1,-256
	add r0,-256,r2
	#add -256,r1,r2

	add r0,r1,256
	add r0,-257,r2

	#add r0,255,256
	#add r0,256,255

	add r0,r1,foo

	add r0,r1,r2
	add r3,r4,r5
	add r6,r7,r8
	add r9,r10,r11
	add r12,r13,r14
	add r15,r16,r17
	add r18,r19,r20
	add r21,r22,r23
	add r24,r25,r26
	add r27,r28,r29
	add r30,r31,r32
	
	
	add.f r0,r1,r2
	add.f r0,r1,1
	add.f r0,1,r2
	add.f 0,r1,r2
	add.f r0,r1,512
	add.f r0,512,r2

	add.f r0,r1,r2
	add.f r0,r1,0
	add.f r0,0,r2
	add.f 0,r1,r2
	add.f r0,r1,512
	add.f r0,512,r2
