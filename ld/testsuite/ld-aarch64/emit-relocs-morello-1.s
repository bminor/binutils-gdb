.data
.global val
val:
	.byte 0x42
	.byte 0x42
	.byte 0x42
	.size val, .-val

.align 4
.global cap
cap:
	.capinit val
	.8byte 0
	.8byte 0
	.size cap, .-cap

.text
.global _start
_start:
	ldr	c0, [c0, :got_lo12:val]
