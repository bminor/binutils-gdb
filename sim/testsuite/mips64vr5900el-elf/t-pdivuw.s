.include "t-macros.i"

	start

test_pdivuw:
	load $7 0x000000007fffffff 0xffffffff80000000
	load $8 0xffffffffffffffff 0xffffffffffffffff
        pdivuw    $7,$8           #Expectation:
        checkHI 0x000000007fffffff 0xffffffff80000000
        checkLO 0x0000000000000000 0x0000000000000000
 
test_pdivuw1:
	load $7 0x7fffffff00000000 0x80000000ffffffff
	load $8 0xffffffffffffffff 0xffffffffffffffff
        pdivuw   $7,$8           #Expectation:
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0x0000000000000000 0x0000000000000001

test_pdivuw2:
	load $7 0x0000000000001000 0x0000000000001fff
	load $8 0x0000000000000100 0x0000000000000100
        pdivuw   $7,$8           #Expectation:
        checkHI 0x0000000000000000 0x00000000000000ff
        checkLO 0x0000000000000010 0x000000000000001f

test_pdivuw_0:
	load $7 0x000000007fffffff 0xffffffff80000000
	load $8 0x0000000000000000 0x0000000000000000
        pdivuw    $7,$8           #Expectation:
	# don't crash!
 
	exit0
