.include "t-macros.i"

	start

test_sqrt_4_0:	
	clearfcsr
	loadfp $f1 4.0
	sqrt.s $f2, $f1
	checkfp 0 $f2 2.0

test_sqrt_0_01:
	clearfcsr
	loadfp $f1 0.01
	sqrt.s $f2, $f1
	checkfp 0 $f2 0.1

test_sqrt_minus_4_0:
	clearfcsr
	loadfp $f1 4.0
	neg.s $f1, $f1
	sqrt.s $f2, $f1
	checkfp FCSR_I $f2 2.0

	exit0
