	.text
	data16 jmp foo

bar:
	mov %eax, %ebx

	data16 call foo

	data16 xbegin foo
