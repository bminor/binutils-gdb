.include "t-macros.i"

	start

test_psravw:	
        load $8 0x000000007fffffff 0x0000000080000000
        load $9 0x0000000000000008 0x0000000000000008
        psravw  $10,$8,$9       #Expectation:
        check10 0x00000000007fffff 0xffffffffff800000

	exit0
