.include "t-macros.i"

	start

test_madda1:	
	clearfcsr
	loadacc  8.0
	loadfp $f1 2.0
	loadfp $f2 4.0
	madda.s $f2, $f1
	checkacc 0 16.0

	exit0
