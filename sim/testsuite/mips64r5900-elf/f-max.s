.include "t-macros.i"

	start

	loadfp $f1, 1.0
	loadfp $f2, 2.0

test_max1:	
	clearfcsr
	max.s $f3, $f1, $f2
	checkfp 0 $f3 2.0

test_max2:	
	clearfcsr
	max.s $f3, $f2, $f1
	checkfp 0 $f3 2.0

	exit0
