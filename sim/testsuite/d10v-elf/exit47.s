	.text
	.align 2
	.globl _start
_start:
	# exit (47)
	ldi r6, 1
	ldi r2, 47
	trap 15
