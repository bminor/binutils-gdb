.data
num:
	.word 0x42
	.size num, .-num
ptr:
	.8byte num
.align 4
cap:
	.capinit num
	.8byte 0
	.8byte 0

globcap:
	.capinit globvar
	.8byte 0
	.8byte 0

.text
.globl _start
.type _start STT_FUNC
_start:
	ldr	c2, [c2, :lo12:cap]
	ldr	c2, [x2, :lo12:cap]
	ldr	x2, [x2, :lo12:cap]
	ldr	w2, [x2, :lo12:cap]

// Capability pointing to an undefined variable.
f1:
	ldr	c2, [c2, :lo12:globcap]
