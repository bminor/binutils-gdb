.include "t-macros.i"

	start

test_psubuh:	
        load $8 0x0124000045690001 0x89ae0002cdf30003
        load $9 0x0001000100020002 0x0003000300040004
        psubuh  $10,$8,$9
	check10 0x0123000045670000 0x89ab0000cdef0000
 
	exit0
