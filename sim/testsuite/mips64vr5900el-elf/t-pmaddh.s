.include "t-macros.i"

	start

test_pmaddh:	
        load $6 0x01237fff45677fff 0x89ab8000cdef8000
        load $7 0x00017fff00017fff 0x00017fff00018000
        load $8 0x0000000000007fff 0x00000000ffff8000
        load $9 0x00000000ffff8000 0x0000000000007fff
        pmthi    $8
        pmtlo    $9
        nop
        nop
        pmaddh    $10,$6,$7      #Expectation:
        check10 0x3fff80003ffe8001 0xc000000040007fff
        checkHI 0x000001233fff8000 0xffff89abc0000000
        checkLO 0x000045673ffe8001 0xffffcdef40007fff

	exit0
