.include "t-macros.i"

	start

test_adda1:	
	clearfcsr
	loadfp $f1 4.0
	loadfp $f2 0.1
	adda.s $f1, $f2
	checkacc 0 4.1

test_add2:
	clearfcsr
	loadfp $f1 , -4.0
	loadfp $f2 8.0
	adda.s $f2, $f1
	checkacc 0 4.0

test_add3:
	clearfcsr
	loadfpmax $f1
	loadfpmax $f2
	adda.s $f2, $f1
	checkaccmax FCSR_O

test_add4:
	clearfcsr
	loadfpmax $f1
	neg.s $f1, $f1
	loadfpmax $f2
	neg.s $f2, $f2
	adda.s $f1, $f2
	checkfcsr FCSR_O
	clearfcsr
	msub.s $f3, $f0, $f0
	neg.s $f3, $f3
	clearfcsr
	checkfpmax 0 $f3

	exit0
