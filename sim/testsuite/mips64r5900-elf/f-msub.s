.include "t-macros.i"

	start

test_msub1:	
	clearfcsr
	loadacc  4.0
	loadfp $f1 1.0
	loadfp $f2 2.0
	msub.s $f3, $f2, $f1
	checkfp 0 $f3 2.0

	exit0
