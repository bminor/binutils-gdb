.include "t-macros.i"

	start

test_msuba1:	
	clearfcsr
	loadacc  2.0
	loadfp $f1 1.0
	loadfp $f2 2.0
	msuba.s $f2, $f1
	checkacc 0 0.0

	exit0
