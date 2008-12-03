	.file	"ifunc.c"
#APP
	.type func, %indirect_function
	.type long_func_name, %indirect_function
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"arg is %d\n"
#NO_APP
	.text
	.p2align 4,,15
.globl bar
	.type	bar, @function
bar:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	$.LC0, (%esp)
	movl	%eax, 4(%esp)
	call	printf
	leave
	ret
	.size	bar, .-bar
	.section	.rodata.str1.1
.LC1:
	.string	"main: calling func"
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 4
.LC2:
	.string	"main: func finished, calling long_func_name"
	.section	.rodata.str1.1
.LC3:
	.string	"main: long_func_name finished"
	.text
	.p2align 4,,15
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$4, %esp
	movl	$.LC1, (%esp)
	call	puts
	call	func
	movl	$.LC2, (%esp)
	call	puts
	call	long_func_name
	movl	$.LC3, (%esp)
	call	puts
	addl	$4, %esp
	xorl	%eax, %eax
	popl	%ecx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.p2align 4,,15
.globl long_func_name
	.type	long_func_name, @function
long_func_name:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	rand
	movl	$func0, %edx
	testl	%eax, %eax
	jne	.L7
	movl	$func1, %edx
.L7:
	movl	%edx, %eax
	leave
	ret
	.size	long_func_name, .-long_func_name
	.p2align 4,,15
.globl func
	.type	func, @function
func:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	rand
	movl	$func0, %edx
	testl	%eax, %eax
	jne	.L12
	movl	$func1, %edx
.L12:
	movl	%edx, %eax
	leave
	ret
	.size	func, .-func
	.section	.rodata.str1.1
.LC4:
	.string	"func1\n"
	.text
	.p2align 4,,15
.globl func1
	.type	func1, @function
func1:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$.LC4, (%esp)
	call	puts
	leave
	ret
	.size	func1, .-func1
	.section	.rodata.str1.1
.LC5:
	.string	"func0\n"
	.text
	.p2align 4,,15
.globl func0
	.type	func0, @function
func0:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$.LC5, (%esp)
	call	puts
	leave
	ret
	.size	func0, .-func0
	.ident	"GCC: (GNU) 4.3.0 20080428 (Red Hat 4.3.0-8)"
	.section	.note.GNU-stack,"",@progbits
