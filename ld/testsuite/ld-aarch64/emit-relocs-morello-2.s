.data
str:
	.string "Hello there big bad world!"
	.size str,.-str
ptr:
	.8byte str
.align 4
.globl cap
cap:
	.capinit str
	.8byte 0
	.8byte 0

.text
.globl _start
.type _start STT_FUNC
_start:
	adrp	c2, :got:cap
	ldr	c2, [c2, :got_lo12:cap]
	ldr	c2, [x2, :got_lo12:cap]
	ldr	x2, [x2, :got_lo12:cap]
