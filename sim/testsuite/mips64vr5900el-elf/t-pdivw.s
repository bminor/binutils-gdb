.include "t-macros.i"

	start

test_pdivw:
	load $7 0x000000007fffffff 0xffffffff80000000
	load $8 0xffffffffffffffff 0xffffffffffffffff
        pdivw   $7,$8           #Expectation:
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0xffffffff80000001 0xffffffff80000000

test_pdivw_0:
	load $7 0x000000007fffffff 0xffffffff80000000
	load $8 0x0000000000000000 0x0000000000000000
        pdivw   $7,$8           #Expectation:
	# don't crash!

	exit0
