.include "t-macros.i"

	start

test_mul1:	
	clearfcsr
	loadfp $f1 2.0
	loadfp $f2 4.0
	mul.s $f3, $f2, $f1
	checkfp 0 $f3 8.0

	exit0
