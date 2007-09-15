.text
	leal	symbol(%rip), %eax
	leal	0x11111111(%rip), %eax
	leal	1(%rip), %eax
	leal	(%rip), %eax
.intel_syntax noprefix
mov     rax, QWORD PTR __CTOR_END__-8[rip]
mov     rax, QWORD PTR __CTOR_END__+-8[rip]
mov     rax, QWORD PTR -8+__CTOR_END__[rip]
mov     rax, QWORD PTR __CTOR_END__[rip-8]
