.include "t-macros.i"

	start

test_pdivuw:
	load $7 0x000000007fffffff 0xffffffff80000000
	load $8 0xffffffffffffffff 0xffffffffffffffff
        pdivuw    $7,$8           #Expectation:
        checkHI 0x000000007fffffff 0xffffffff80000000
        checkLO 0x0000000000000000 0x0000000000000000
 
test_pdivuw_0:
	load $7 0x000000007fffffff 0xffffffff80000000
	load $8 0x0000000000000000 0x0000000000000000
        pdivuw    $7,$8           #Expectation:
	# don't crash!
 
	exit0
