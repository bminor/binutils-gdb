	.globl bar
bar:
	mov foo(%rip), %rax

	.data
xxx:
	.long foo
	.long xxx
