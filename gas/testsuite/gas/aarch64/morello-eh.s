	.text
	.arch morello
	.globl	f1
	.type	f1,@function
f1:
	.cfi_startproc
	mrs	c29, ddc
	sub	csp, csp, #96
	stp	c29, c30, [sp, #64]
	add	c29, csp, #64
	.cfi_def_cfa sp, 32
	.cfi_offset x30, -16
	.cfi_offset ddc, -32
	ret
	.size	f1, .-f1
	.cfi_endproc

	.arch morello+c64
	.globl	f2
	.type	f2,@function
f2:
	.cfi_startproc purecap
	mrs	c29, ddc
	sub	csp, csp, #96
	stp	c29, c30, [csp, #64]
	add	c29, csp, #64
	.cfi_def_cfa csp, 32
	.cfi_offset c30, -16
	.cfi_offset ddc, -32
	.size	f2, .-f2
	.cfi_endproc
