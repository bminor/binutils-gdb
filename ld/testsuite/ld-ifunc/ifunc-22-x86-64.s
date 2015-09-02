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
	movq    foo@GOTPCREL(%rip), %rax
	movq    bar@GOTPCREL(%rip), %rax
	.type foo, %gnu_indirect_function
	.globl foo
foo:
	ret
	.type bar, %function
bar:
	ret
