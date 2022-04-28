.data
val:
	.byte 0x42
	.byte 0x42
	.byte 0x42
	.size val, .-val

.text
.global _start
_start:
	ldr	c0, [c0, :got_lo12:val]

