.include "t-macros.i"

	start

test_suba1:	
	clearfcsr
	loadacc 0.0
	loadfp $f1 1.0
	loadfp $f2 2.0
	suba.s $f2, $f1
	checkacc 0 1.0

	exit0
