.include "t-macros.i"

	start

	load r2 test_jmp1
	add 1, r2 # misalign
	jmp [r2]
test_jmp:

	exit47	
	exit47	
	exit47	
	exit47	
	exit47	
	exit47	
	exit47	
	exit47

test_jmp1:
	exit0
