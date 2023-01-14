	.text
_start:
	vldmxcsr (%rbp)
	lgdt (%rbp)
	vmptrld (%rbp)
	vmclear (%rbp)
	fsts (%rbp)
	flds (%rbp)
	fistl (%rbp)
	fists (%rbp)
	fildl (%rbp)
	filds (%rbp)
	fsave (%rbp)
	frstor (%rbp)
	filds (%rbp)
	fisttps (%rbp)
	fldenv (%rbp)
	fstenv (%rbp)
	fadds  (%rbp)
	fadds  (%rsp)
	fadd  %st(3),%st
	fadds (%rcx)
	filds (%rcx)
	fists (%rcx)
	xrstor (%rcx)
	prefetchnta (%rcx)
	cmpxchg8b (%rcx)
	cmpxchg16b (%rcx)
	incl	%ecx
	lgdt (%rax)
	pfcmpeq 2(%rsi),%mm4
	popq (%rax)
	popq %rax
	rclw	(%rcx)
	testl	$1,(%rcx)
	incl	(%rcx)
	notl	(%rcx)
	divl	(%rcx)
	mull	(%rcx)
	idivl	(%rcx)
	imull	(%rcx)
	leaq	(%rax,%rax,2), %rax
	leave
	outsb
	lodsb
	rep movsl
	rep scasl
	rep cmpsl
	rep lodsl
	addl $1, (%r11)
	btl $1, (%r11)
	xadd %rax,(%rbx)
	xadd %rax,%rbx
	xchg %rax,(%rbx)
	xchg %rax,%rbx
	cmp    %rax,0x40(%rbp)
	cmp    0x40(%rbp),%rax
	add %rax,0x40(%rbp)
	add (%rax),%rax
	test %rax,0x40(%rbp)
	test 0x40(%rbp),%rax
