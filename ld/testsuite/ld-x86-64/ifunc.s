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
.LFB25:
	movl	%edi, %esi
	xorl	%eax, %eax
	movl	$.LC0, %edi
	jmp	printf
.LFE25:
	.size	bar, .-bar
	.section	.rodata.str1.1
.LC1:
	.string	"main: calling func"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
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
.LFB24:
	subq	$8, %rsp
.LCFI0:
	movl	$.LC1, %edi
	call	puts
	call	func
	movl	$.LC2, %edi
	call	puts
	call	long_func_name
	movl	$.LC3, %edi
	call	puts
	xorl	%eax, %eax
	addq	$8, %rsp
	ret
.LFE24:
	.size	main, .-main
	.p2align 4,,15
.globl long_func_name
	.type	long_func_name, @function
long_func_name:
.LFB23:
	subq	$8, %rsp
.LCFI1:
	call	rand
	movl	%eax, %edx
	movl	$func1, %eax
	testl	%edx, %edx
	movl	$func0, %edx
	cmovne	%rdx, %rax
	addq	$8, %rsp
	ret
.LFE23:
	.size	long_func_name, .-long_func_name
	.p2align 4,,15
.globl func
	.type	func, @function
func:
.LFB22:
	subq	$8, %rsp
.LCFI2:
	call	rand
	movl	%eax, %edx
	movl	$func1, %eax
	testl	%edx, %edx
	movl	$func0, %edx
	cmovne	%rdx, %rax
	addq	$8, %rsp
	ret
.LFE22:
	.size	func, .-func
	.section	.rodata.str1.1
.LC4:
	.string	"func1\n"
	.text
	.p2align 4,,15
.globl func1
	.type	func1, @function
func1:
.LFB21:
	movl	$.LC4, %edi
	jmp	puts
.LFE21:
	.size	func1, .-func1
	.section	.rodata.str1.1
.LC5:
	.string	"func0\n"
	.text
	.p2align 4,,15
.globl func0
	.type	func0, @function
func0:
.LFB20:
	movl	$.LC5, %edi
	jmp	puts
.LFE20:
	.size	func0, .-func0
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zR"
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.uleb128 0x1
	.byte	0x3
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.long	.LFB25
	.long	.LFE25-.LFB25
	.uleb128 0x0
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.long	.LFB24
	.long	.LFE24-.LFB24
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI0-.LFB24
	.byte	0xe
	.uleb128 0x10
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.long	.LFB23
	.long	.LFE23-.LFB23
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI1-.LFB23
	.byte	0xe
	.uleb128 0x10
	.align 8
.LEFDE5:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.long	.LFB22
	.long	.LFE22-.LFB22
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI2-.LFB22
	.byte	0xe
	.uleb128 0x10
	.align 8
.LEFDE7:
.LSFDE9:
	.long	.LEFDE9-.LASFDE9
.LASFDE9:
	.long	.LASFDE9-.Lframe1
	.long	.LFB21
	.long	.LFE21-.LFB21
	.uleb128 0x0
	.align 8
.LEFDE9:
.LSFDE11:
	.long	.LEFDE11-.LASFDE11
.LASFDE11:
	.long	.LASFDE11-.Lframe1
	.long	.LFB20
	.long	.LFE20-.LFB20
	.uleb128 0x0
	.align 8
.LEFDE11:
	.ident	"GCC: (GNU) 4.3.0 20080428 (Red Hat 4.3.0-8)"
	.section	.note.GNU-stack,"",@progbits
