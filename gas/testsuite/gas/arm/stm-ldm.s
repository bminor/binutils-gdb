	.text
	.syntax unified
	stmfd	sp!, {r0}
	stmfd	sp!, {r1, r2, r3}
	stmfd	sp!, {r9}
	ldmia sp!, {r9}
	ldmia sp!, {r1, r2, r3}
	ldmia sp!, {r0}
