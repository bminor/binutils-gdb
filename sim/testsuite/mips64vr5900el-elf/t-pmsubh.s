.include "t-macros.i"

	start

test_pmsubh:	
        load $6 0x01237fff45677fff 0x89ab8000cdef8000
        load $7 0x00017fff00017fff 0x00017fff00018000
	load $8 0x000001237fffffff 0xffff89ab7fffffff
        load $9 0x0000456780000000 0xffffcdef80000000
        pmthi    $8
        pmtlo    $9
        nop
        nop
        pmsubh    $10,$6,$7      #Expectation:
        check10 0x4000fffe4000ffff 0xbfff7fff40000000
        checkHI 0x000000004000fffe 0x00000000bfff7fff
        checkLO 0x000000004000ffff 0x0000000040000000

	exit0
