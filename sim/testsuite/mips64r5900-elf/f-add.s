.include "t-macros.i"

	start

test_add1:	
	clearfcsr
	loadfp $f1 4.0
	loadfp $f2 0.1
	add.s $f3, $f2, $f1
	checkfp 0 $f3 4.1

test_add2:
	clearfcsr
	loadfp $f1 , -4.0
	loadfp $f2 8.0
	add.s $f3, $f2, $f1
	checkfp 0 $f3 4.0

test_add3:
	clearfcsr
	loadfpmax $f1
	loadfpmax $f2
	add.s $f3, $f2, $f1
	checkfpmax FCSR_O $f3

test_add4:
	clearfcsr
	loadfpmax $f1
	neg.s $f1, $f1
	loadfpmax $f2
	neg.s $f2, $f2
	add.s $f3, $f2, $f1
	checkfcsr FCSR_O
	clearfcsr
	neg.s $f3, $f3
	checkfpmax 0 $f3

	exit0
