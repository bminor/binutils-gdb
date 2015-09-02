	.text
	.type start,"function"
	.global start
start:
	.type _start,"function"
	.global _start
_start:
	.type __start,"function"
	.global __start
__start:
	.type __start,"function"
	call	.L6
.L6:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-.L6], %ebx
	mov	foo@GOT(%ebx), %eax
	mov	bar@GOT(%ebx), %eax
	.global	foo
	.type	foo, %gnu_indirect_function
foo:
	ret
	.type	bar, %function
bar:
	ret
