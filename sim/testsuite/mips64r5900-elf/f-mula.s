.include "t-macros.i"

	start

test_mula1:	
	clearfcsr
	loadfp $f1 2.0
	loadfp $f2 4.0
	mula.s $f2, $f1
	checkacc 0 8.0

	exit0
