	.data
str:
	.string "Hello there big bad world!"
	.size str,.-str
	.global ptr
ptr:
	.8byte str
	.align 4
	.type cap STT_OBJECT
	.global cap
cap:
	.chericap str

	.text
	.globl _start
	.type _start STT_FUNC
_start:
	ldr	x2, [x2, :got_lo12:cap]
	adrp	c2, :got:cap
	ldr	c2, [c2, :got_lo12:cap]
	ldr	c2, [x2, :got_lo12:cap]
