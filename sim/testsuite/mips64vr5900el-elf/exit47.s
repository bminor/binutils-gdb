	# The main body of code loops on the variable FLAG, waiting
	# for it to be set.  When set, the message `Tick' is displayed
	# and the flag cleared.

	# The interrupt handler, sets the global FLAG variable and
	# then resumes normal execution.

	.globl _start
	.set noreorder
_start:

	li	$4, 47
	break 1023
	nop
