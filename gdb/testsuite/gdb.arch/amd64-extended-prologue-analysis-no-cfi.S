/* This testcase is part of GDB, the GNU debugger.

   Copyright 2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This file is compiled from gdb.arch/amd64-extended-prologue-analysis.c
   using gcc 11.4.0 with flags: -g0 -O1 -S -fno-omit-frame-pointer
   -fno-asynchronous-unwind-tables.  */

	.file	"amd64-extended-prologue-analysis.c"
	.text
	.globl	bar
	.type	bar, @function
bar:
	endbr64
	leal	(%rdi,%rdi), %eax
	ret
	.size	bar, .-bar
	.globl	foo
	.type	foo, @function
foo:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	movl	%edi, %r12d
	movl	%esi, %r14d
	movl	%edx, %r13d
	call	bar
	movl	%eax, %ebx
	movl	%r14d, %edi
	call	bar
	addl	%eax, %ebx
	movl	%r13d, %edi
	call	bar
	addl	%ebx, %eax
	addl	%r12d, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	ret
	.size	foo, .-foo
	.globl	main
	.type	main, @function
main:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	leal	(%rdi,%rdi), %ecx
	leal	2(%rdi), %edx
	leal	1(%rdi), %esi
	call	foo
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
