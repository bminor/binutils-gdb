.include "t-macros.i"

	start

	# Set the callt base pointer
	load r1 ctret_dest
	ldsr r1, ctpc
	
test_ctret:
	ctret
	exit47

ctret_dest:
	exit0
