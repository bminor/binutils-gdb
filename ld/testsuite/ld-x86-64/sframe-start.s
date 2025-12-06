	.text
	.globl	_start
	.type	_start, @function
_start:
	.cfi_startproc
	.cfi_undefined 16
	xor	%rbp, %rbp
	call foo
	.cfi_endproc
	.size	_start, .-_start
	.section	.note.GNU-stack,"",@progbits
