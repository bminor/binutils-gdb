	; Modular address postincrement test
	
start:	
	; set modular address limits: 0x18 bytes
	add  r1,r0,0x20000070    ; [start, ...
	nop || nop
	mvtsys mod_s,r1 || nop
	nop || nop
	add  r1,r0,0x20000088    ; ..., end)
	nop || nop
	mvtsys mod_e,r1 || nop

	;  program PSW for modular address mode
	add  r1,r0,0x81000000
	nop || nop
	mvtsys psw,r1 || nop
	nop || nop

	; modular autoincrement test
	add	r30,r0,0x20000070 ; base address = mod_s
	ld2w	r40,@(r30+,r0) || nop ; after:     r30 = ...078
	ld2w	r40,@(r30+,r0) || nop ; after:	r30 = ...080
	ld2w	r40,@(r30+,r0) || nop ; after:	r30 = ...070
	
	add	r29,r0,0x20000070 ; expected end address; wrapping around
	cmpeq	f1,r30,r29
	bra/xf	fail

	; Q: what about autodecrement?
	
ok:	
	add	r2,r0,0	
	.long	0x0e000004, 0x00f00000
		
fail:	
	add	r2,r0,47	
	.long	0x0e000004, 0x00f00000
