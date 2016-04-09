	.text
	.align	4, 0
	.globl	foo
	.ent	foo
foo:
	b	bar
	.end	foo

	.align	align, 0
	.globl	bar
	.weak	bar
	.ent	bar
bar:
	jr	$ra
	.end	bar

# Force some (non-delay-slot) zero bytes, to make 'objdump' print ...
	.align  4, 0
	.space  16
