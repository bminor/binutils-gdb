	.text
	.global _start
_start:
	lla	a0, data_g
.rept 0x3fffe
	nop
.endr
	call _start
	.option rvc
	.align 2
	add	a0, a1, a2

	.data
	.global data_g
	.dword 0x0
data_g:
	.word 0x1000
