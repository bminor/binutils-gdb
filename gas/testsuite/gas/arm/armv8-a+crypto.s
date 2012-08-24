	.syntax unified
	.arch armv8-a
	.arch_extension crypto

	.arm
	aese.8	q0, q0
	aese.8	q7, q7
	aese.8	q8, q8
	aese.8	q15, q15
	aesd.8	q0, q0
	aesd.8	q7, q7
	aesd.8	q8, q8
	aesd.8	q15, q15
	aesmc.8	q0, q0
	aesmc.8	q7, q7
	aesmc.8	q8, q8
	aesmc.8	q15, q15
	aesimc.8	q0, q0
	aesimc.8	q7, q7
	aesimc.8	q8, q8
	aesimc.8	q15, q15

	.thumb
	aese.8	q0, q0
	aese.8	q7, q7
	aese.8	q8, q8
	aese.8	q15, q15
	aesd.8	q0, q0
	aesd.8	q7, q7
	aesd.8	q8, q8
	aesd.8	q15, q15
	aesmc.8	q0, q0
	aesmc.8	q7, q7
	aesmc.8	q8, q8
	aesmc.8	q15, q15
	aesimc.8	q0, q0
	aesimc.8	q7, q7
	aesimc.8	q8, q8
	aesimc.8	q15, q15
