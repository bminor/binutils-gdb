
	.text
	.global misc
misc:
	di
	ei
	halt
	nop
	reti
	trap 0
	trap 31
#	ldsr r5,r6
#	stsr r6,r5
