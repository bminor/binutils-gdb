.include "t-macros.i"

	start

test_pmaddw:
        load $6 0x0123456780000000 0X89ABCDEF7FFFFFFF
        load $7 0x0123456780000000 0X89ABCDEF7FFFFFFF
	load $8 0x0123456700000000 0X89ABCDEFFFFFFFFF
	load $9 0X012345677FFFFFFF 0X89ABCDEF80000000
        pmthi    $8
        pmtlo    $9
        nop
        nop
        pmaddw    $10,$6,$7       #Expectation:
        check10 0x400000007fffffff 0x3ffffffe80000001
        checkHI 0x0000000040000000 0x000000003FFFFFFE
        checkLO 0x000000007fffffff 0xffffffff80000001

test_pmaddw2:	
        load $6 0x0123456780000000 0x89abcdef7fffffff
        load $7 0x0123456780000000 0x89abcdef7fffffff
	load $8 0x01234567ffffffff 0x89abcdef00000000
	load $9 0x0123456780000000 0x89abcdef7fffffff
        pmthi    $8
        pmtlo    $9
        nop
        nop
        pmaddw    $10,$6,$7       #Expectation:
        check10 0x3fffffff80000000 0x3fffffff80000000
        checkHI 0x000000003fffffff 0x000000003fffffff
        checkLO 0xffffffff80000000 0xffffffff80000000

	exit0
