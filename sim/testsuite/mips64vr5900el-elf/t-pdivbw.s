.include "t-macros.i"

	start

test_pdivbw:
        load $7 0x7fffffff80000000 0x0000000100000001
	load $8 0x0000000000000000 0x000000000000ffff
        pdivbw  $7,$8          #Expectation:
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0x8000000180000000 0xffffffffffffffff

test_pdivbw_0:
        load $7 0x7fffffff80000000 0x0000000100000001
	load $8 0x0000000000000000 0x0000000000000000
        pdivbw  $7,$8          #Expectation:
	# don't crash!

	exit0
