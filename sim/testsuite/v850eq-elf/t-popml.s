.include "t-macros.i"

	start

	load sp stackEI
	ldsr r0, psw    # ENSURE NP=EP=0
test_popml_EI:
	popml {eipc, eipsw}
	# check EIPSW
	stsr eipsw, r3
	check1 r3 0xe0
	ldsr r0, eipsw
	# check EIPC
	stsr eipc, r3
	check1 r3 0xec
	ldsr r0, eipc


	
	mov PSW_NP, r1
	ldsr r1, psw   # ENSURE NP=1, EP=0
	load sp stackFE
test_popml_FE:
	popml {fepc, fepsw}
	# check FEPSW
	stsr fepsw, r3
	check1 r3 0xf0
	ldsr r0, fepsw
	# check FEPC
	stsr fepc, r3
	check1 r3 0xfc
	ldsr r0, fepc
	

	load sp stackR
test_popml_R:
	popml {r1,r2,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15}
	mov r1, r3
	check1 r3 1
	mov 0, r3
	check1 r2 2
	mov 0, r2
	#check1 r3 3
	check1 r4 4
	mov 0, r4
	check1 r5 5
	mov 0, r5
	check1 r6 6
	mov 0, r6
	check1 r7 7
	mov 0, r7
	check1 r8 8
	mov 0, r8
	check1 r9 9
	mov 0, r9
	check1 r10 10
	mov 0, r10
	check1 r11 11
	mov 0, r11
	check1 r12 12
	mov 0, r12
	check1 r13 13
	mov 0, r13
	check1 r14 14
	mov 0, r14
	check1 r15 15
	mov 0, r15


	load sp, stackPSW
test_popml_PSW:
	popml {psw}
	stsr psw, r2
	check1 r2, 0xee


	exit0

	.align 2
stackEI:
	.long 0xe0 #EIPSW
	.long 0xec #EIPC


stackFE:
	.long 0xf0 #FEPSW
	.long 0xfc #FEPC


stackR:
	.long 15
	.long 14
	.long 13
	.long 12
	.long 11
	.long 10
	.long  9
	.long  8
	.long  7
	.long  6
	.long  5
	.long  4
	#.long  3 - sp
	.long  2
	.long  1


stackPSW:
	.long 0xee
