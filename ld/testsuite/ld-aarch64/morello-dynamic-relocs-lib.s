	.arch morello+crc+c64
	.text
	.p2align 4,,11
	.global memcpy
	.type   memcpy,%function
memcpy:
	ret
	.size memcpy, .-memcpy
	.global	var
	.bss
	.align	2
	.type	var, %object
	.size	var, 4
var:
	.zero	4
