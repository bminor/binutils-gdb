.include "t-macros.i"

	start

	### $f3 = $f2 / sqrt ($f1)

test_rsqrt:	
	loadfp $f1 4.0
	loadfp $f2 1.0
	rsqrt.s $f3, $f2, $f1
	checkfp 0 $f3 0.5

	################

 
##        +1.0 / sqrt (+0.0)
##                -> 0x7fffffff (+Max) D+SD
test_rsqrt_1:
	clearfcsr
	loadfp $f1, +1.0
	loadfp $f2, +0.0
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00010021 $f3 0x7fffffff


##        -1.0 / sqrt (+0.0)
##                -> 0xffffffff (-Max) D+SD
test_rsqrt_2:
	clearfcsr
	loadfp $f1, -1.0
	loadfp $f2, +0.0
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00010021 $f3 0xffffffff


##       0 / sqrt (-0)
##and    0 / sqrt (0)
##                ->0x7fffffff  I + SI
test_rsqrt_3a:
	clearfcsr
	loadfp $f1, +0.0
	loadfp $f2, -0.0
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0x7fffffff

test_rsqrt_3b:
	clearfcsr
	loadfp $f1, +0.0
	loadfp $f2, +0.0
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0x7fffffff


##        Correctly:
##                (-0) / sqrt (-0)
##                        ->FFFFFFFF (I + SI)
test_rsqrt_4a:
	clearfcsr
	loadfp $f1, -0.0
	loadfp $f2, -0.0
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0xffffffff

test_rsqrt_4b:
	clearfcsr
	loadfp $f1, -0.0
	loadfp $f2, +0.0
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0xffffffff


##	(max) / sqrt (min*2) -> no O+SO
##
test_rsqrt_5aa:
	clearfcsr
	loadfpx $f1, 0x7fffffff
	loadfpx $f2, 0x00800001
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00000001 $f3 0x7fffffff

test_rsqrt_5ab:
	clearfcsr
	loadfpx $f1, 0x7fffffff
	loadfpx $f2, 0x80800001
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0x7fffffff

test_rsqrt_5ba:
	clearfcsr
	loadfpx $f1, 0xffffffff
	loadfpx $f2, 0x00800001
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00000001 $f3 0xffffffff

test_rsqrt_5bb:
	clearfcsr
	loadfpx $f1, 0xffffffff
	loadfpx $f2, 0x80800001
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0xffffffff

	
##	(max) / sqrt (min*2) -> no U+SU
##
test_rsqrt_6aa:
	clearfcsr
	loadfpx $f1, 0x00800001
	loadfpx $f2, 0x7fffffff
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00000001 $f3 0x00000000

test_rsqrt_6ab:
	clearfcsr
	loadfpx $f1, 0x00800001
	loadfpx $f2, 0xffffffff
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0x00000000

test_rsqrt_6ba:
	clearfcsr
	loadfpx $f1, 0x80800001
	loadfpx $f2, 0x7fffffff
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00000001 $f3 0x80000000

test_rsqrt_6bb:
	clearfcsr
	loadfpx $f1, 0x80800001
	loadfpx $f2, 0xffffffff
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020041 $f3 0x80000000


##        Correctly:
##                (max/min) / sqrt ([+-]0)
##                        ->MAX/MIN (D + SD)
test_rsqrt_7aa:
	clearfcsr
	loadfpx $f1, 0x7fffffff
	loadfpx $f2, 0x00000000
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00010021 $f3 0x7fffffff

test_rsqrt_7ab:
	clearfcsr
	loadfpx $f1, 0x7fffffff
	loadfpx $f2, 0x80000000
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00010021 $f3 0x7fffffff

test_rsqrt_7ba:
	clearfcsr
	loadfpx $f1, 0xffffffff
	loadfpx $f2, 0x00000000
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00010021 $f3 0xffffffff

test_rsqrt_7bb:
	clearfcsr
	loadfpx $f1, 0xffffffff
	loadfpx $f2, 0x80000000
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00010021 $f3 0xffffffff

	
##	Like max/sqrt but check that bits are set/cleared
##
test_rsqrt_9aa:
	clearfcsr
	loadfpx $f1, 0x7fffffff
	loadfpx $f2, 0x00000000
	rsqrt.s $f3, $f1, $f2
	loadfpx $f1, 0x00800001
	loadfpx $f2, 0xffffffff
	rsqrt.s $f3, $f1, $f2
	checkfpx 1 0x00020061 $f3 0x00000000


	exit0
