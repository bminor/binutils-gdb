.include "t-macros.i"

	start

test_div1:	
	clearfcsr
	loadfp $f1 2.0
	loadfp $f2 4.0
	div.s $f3, $f2, $f1
	checkfp 0 $f3 2.0

	
test_div2:
	clearfcsr
	loadfp $f1 0.0
	loadfp $f2 0.0
	div.s $f3, $f2, $f1
	checkfpmax FCSR_I $f3

test_div3:
	clearfcsr
	loadfp $f1 0.0
	loadfp $f2 1.0
	div.s $f3, $f2, $f1
	checkfpmax FCSR_D $f3
	
	exit0
