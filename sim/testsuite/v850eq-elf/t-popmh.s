.include "t-macros.i"

	start

	load sp stackEI
	ldsr r0, psw    # ENSURE NP=EP=0
test_popmh_EI:
	popmh {eipc, eipsw}
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
test_popmh_FE:
	popmh {fepc, fepsw}
	# check FEPSW
	stsr fepsw, r3
	check1 r3 0xf0
	ldsr r0, fepsw
	# check FEPC
	stsr fepc, r3
	check1 r3 0xfc
	ldsr r0, fepc
	

	load sp stackR
test_popmh_R:
	popmh {r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31}
	check1 r16 16
	mov 0, r16
	check1 r17 17
	mov 0, r17
	check1 r18 18
	mov 0, r18
	check1 r19 19
	mov 0, r19
	check1 r20 20
	mov 0, r20
	check1 r21 21
	mov 0, r21
	check1 r22 22
	mov 0, r22
	check1 r23 23
	mov 0, r23
	check1 r24 24
	mov 0, r24
	check1 r25 25
	mov 0, r25
	check1 r26 26
	mov 0, r26
	check1 r27 27
	mov 0, r27
	check1 r28 28
	mov 0, r28
	check1 r29 29
	mov 0, r29
	check1 r30 30
	mov 0, r30
	check1 r31 31
	mov 0, r31

	exit0

	.align 2
stackEI:
	.long 0xe0 #EIPSW
	.long 0xec #EIPC


stackFE:
	.long 0xf0 #FEPSW
	.long 0xfc #FEPC


stackR:
	.long 31
	.long 30
	.long 29
	.long 28
	.long 27
	.long 26
	.long 25
	.long 24
	.long 23
	.long 22
	.long 21
	.long 20
	.long 19
	.long 18
	.long 17
	.long 16


stackPSW:
	.long 0xee
