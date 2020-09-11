.data
.globl str
str:
	.string "Hello there big bad world!"
	.size str,.-str
.align 4
.globl cap
cap:
	.capinit str
	.8byte 0
	.8byte 0

.globl cap2
cap2:
	.capinit str2
	.8byte 0
	.8byte 0

.text
.globl _start
.type _start STT_FUNC
_start:
	adrp	c2, :got:cap
	ldr	c2, [c2, :got_lo12:cap]
	ldr	c2, [c2, :got_lo12:cap2]
	// An external symbol.  The linker should create a R_MORELLO_GLOB_DAT
	// for this since it is a capability load.
	ldr	c2, [c2, :got_lo12:cap3]
