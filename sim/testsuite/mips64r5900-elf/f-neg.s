.include "t-macros.i"

	start

test_neg1:	
	clearfcsr
	loadfp $f1 4.0
	neg.s $f3, $f1
	checkfp 0 $f3 , -4.0

	exit0
