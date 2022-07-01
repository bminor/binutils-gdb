	.arch morello+crc+c64
	.text
	.p2align 4,,11
	.global	_start
	.type	_start, %function
_start:
	.cfi_startproc purecap
	ret
	.cfi_endproc
	.size	_start, .-_start
	.data
varptr:
	.chericap var
