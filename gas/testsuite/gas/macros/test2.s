	.macro	m arg1 arg2 arg3
	.long	\arg1
	.ifnc	,\arg2\arg3
	m	\arg2,\arg3
	.endif
	.endm

	m	r1,r2,r3
