.include "t-macros.i"

	start

	# Set the callt base pointer
	load r1 callt_base
	ldsr r1, ctbp

test_callt:
	callt 2
	exit47



	.align 1
callt_base:
	.short -1
	.short -1
	.short (dest - callt_base)
	.short -1
	.short -1

dest:
	stsr ctpc, r2
	check1 r2 test_callt+2
	
	exit0
