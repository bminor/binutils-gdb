	.text
	clr d2
	inc d1
	inc a2
	inc4 a3
	jmp a2
	jmp 256
	jmp 65536
	call 256,5,9
	call 65536,0,0
	calls a2
	calls 256
	calls 65536
	ret 15,7
	retf 9,5
	rets
	rti
	trap
	nop
	rtm
