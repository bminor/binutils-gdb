.include "t-macros.i"

	start

test_cvt1:	
	clearfcsr
	loadfp $f1 4.0
	cvt.s.w $f2, $f1
	mfc1 $10, $2
	check10 0 4

test_cvt2:	
	clearfcsr
	loadfp $f1 4.0
	neg.s $f1, $f1
	cvt.s.w $f2, $f1
	mfc1 $10, $2
	check10 0 , -4

test_cvt3:
	clearfcsr
	li	$4, 4
	mtc1	$4, $4
	cvt.w.s $f3, $f4
	checkfp 0 $f3 4.0

test_cvt4:
	clearfcsr
	li	$4, -4
	mtc1	$4, $4
	cvt.w.s $f3, $f4
	checkfp 0 $f3 , -4.0

	exit0
