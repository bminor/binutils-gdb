.include "t-macros.i"

	start

	loadfp $f1, 1.0
	loadfp $f2, 2.0

test_min1:	
	clearfcsr
	min.s $f3, $f1, $f2
	checkfp 0 $f3 1.0

test_min2:	
	clearfcsr
	min.s $f3, $f2, $f1
	checkfp 0 $f3 1.0

	exit0
