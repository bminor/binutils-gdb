	# The main body of code loops on the variable FLAG, waiting
	# for it to be set.  When set, the message `Tick' is displayed
	# and the flag cleared.

	# The interrupt handler, sets the global FLAG variable and
	# then resumes normal execution.

	.globl _start
	.set noreorder
_start:

	
	# write (1, Hello World!\r\n, 14)
	li	$4, 1
	la	$5, hello
	li	$6, 14
	la	$2,+0xffffffffbfc00504 # write address
	lw	$2, 0($2)
	jal	$2
	nop

	li	$4, 0
	break 1023
	nop
	
hello:	.asciiz "Hello World!\r\n"
