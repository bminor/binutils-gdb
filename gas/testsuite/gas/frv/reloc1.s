	.globl	f1
begin:
	nop
	nop
f1:
	nop
	nop
	nop
f2:
	call	f1
	bra	f1
	call	f2
	bra	f2
	.space	16
