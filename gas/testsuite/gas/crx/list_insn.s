# Instructions including a register list (opcode is represented as a mask).
 .data
foodata: .word 42
	 .text
footext:

	.global push
push:
push ra, {r3, r4}
push r2

	.global pushx
pushx:
pushx sp, {r0, r1, r2, r3, r4, r5, r6, r7}

	.global pop
pop:
pop r0, {r10}
pop r2

	.global popx
popx:
popx sp, {r0, r1, r3, r4, r5, r6, r7}

	.global popret
popret:
popret r14, {ra, r1}
popret ra

	.global loadm
loadm:
loadm r0, {r1, r0}

	.global loadma
loadma:
loadma r13, {r12, r4, r2}

	.global popa
popa:
popa ra, {r15}

	.global storm
storm:
storm r15, {ra}

	.global storma
storma:
storma r3, {r0}

