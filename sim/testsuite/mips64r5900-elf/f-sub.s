.include "t-macros.i"

	start

test_sub1:	
	clearfcsr
	loadfp $f1 1.0
	loadfp $f2 2.0
	sub.s $f3, $f2, $f1
	checkfp 0 $f3 1.0

	exit0
