.include "t-macros.i"

	start

test_madd1:	
	clearfcsr
	loadacc 1.0
	loadfp $f1 2.0
	loadfp $f2 4.0
	madd.s $f3, $f2, $f1
	checkfp 0 $f3 9.0

test_madd2:	
	clearfcsr
	loadacc 4.0
	loadfp $f1 2.0
	loadfp $f2 2.0
	madd.s $f3, $f2, $f1
	checkfp 0 $f3 8.0

	exit0
