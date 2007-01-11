	.syntax unified
	.text
	.align	2
	.global	thumb2_add
	.thumb
	.thumb_func
thumb2_add:
	add r0, pc, #0x800
	add r9, pc, #0
	add r9, pc, #0x400
	add r8, r9, #0x400
	add r8, r9, #0x101
	add r3, r1, #0x101
	sub r0, pc, #0x800
	sub r9, pc, #0
	sub r9, pc, #0x400
	sub r8, r9, #0x400
	sub r8, r9, #0x101
	sub r3, r1, #0x101
	add r3, #1
	sub r3, #1
