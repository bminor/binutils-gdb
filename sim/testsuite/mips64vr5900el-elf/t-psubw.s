.include "t-macros.i"

	start

test_pmsubw:	
        load $6 0x0123456780000000 0x89abcdef7fffffff
        load $7 0x0123456780000000 0x89abcdef7fffffff
        load $8 0x0123456780000000 0x89abcdef7fffffff
        load $9 0x0123456700000000 0x89abcdefffffffff
        pmthi    $8
        pmtlo    $9
        nop
        nop
        pmsubw    $10,$6,$7       #Expectation:
        check10 0x4000000000000000 0x40000000fffffffe
        checkHI 0x0000000040000000 0x0000000040000000
        checkLO 0x0000000000000000 0xfffffffffffffffe
	
	exit0
