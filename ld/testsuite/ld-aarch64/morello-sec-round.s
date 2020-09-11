.text
.globl _start
_start:
	adrp c0, :got:text_sym
	bl _start
	ret
.align 17
_foo:
	ret
