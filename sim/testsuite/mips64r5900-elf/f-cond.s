.include "t-macros.i"

	start

	loadfp $f1 1.0
	loadfp $f2 2.0
	loadfp $f3 3.0
	
test_f1:	
	clearfcsr
	c.f.s $f1, $f1
	checkfcsr 0

	
test_eq1:
	clearfcsr
	c.eq.s $f1, $f1
	checkfcsr FCSR_C

test_eq2:
	clearfcsr
	c.eq.s $f1, $f2
	checkfcsr 0

	
test_lt1:
	clearfcsr
	c.lt.s $f1, $f1
	checkfcsr 0
	
test_lt2:
	clearfcsr
	c.lt.s $f1, $f2
	checkfcsr FCSR_C
	
test_lt3:
	clearfcsr
	c.lt.s $f3, $f2
	checkfcsr 0
	
		
test_le1:
	clearfcsr
	c.le.s $f1, $f1
	checkfcsr FCSR_C
	
test_le2:
	clearfcsr
	c.le.s $f1, $f2
	checkfcsr FCSR_C
	
test_le3:
	clearfcsr
	c.le.s $f3, $f2
	checkfcsr 0
	
	exit0
