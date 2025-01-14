# Check 64bit MOVRS instructions

	.text
_start:
	movrsw	-256(%rdx), %dx
	movrsl	-512(%rdx), %edx
	movrsq	-1024(%rdx), %r12
	movrsb	-128(%rdx), %bl

_intel:
	.intel_syntax noprefix
	movrsw	dx, WORD PTR [rdx-256]
	movrsl	edx, DWORD PTR [rdx-512]
	movrsq	r12, QWORD PTR [rdx-1024]
	movrsb	bl, BYTE PTR [rdx-128]
