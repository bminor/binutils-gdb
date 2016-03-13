	.text
	.global _start
_start:
	movl	foo@GOT(%ebx), %eax
