.include "t-macros.i"

	start

test_abs1:	
	clearfcsr
	loadfp $f1 4.0
	abs.s $f2, $f1
	checkfp 0 $f2 4.0

test_abs2:
	clearfcsr
	loadfp $f1 4.0
	neg.s $f1, $f1
	abs.s $f2, $f1
	checkfp 0 $f2 4.0

	exit0
