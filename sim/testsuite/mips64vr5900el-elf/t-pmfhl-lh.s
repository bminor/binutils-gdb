.include "t-macros.i"

	start

test_pmfhl_lh:	
	load $7 0xffff1111ffff2222 0xffff5555ffff6666
	load $8 0xffff3333ffff4444 0xffff7777ffff8888
        pmthi   $7
        pmtlo   $8
        nop
        nop
        pmfhl.lh $10    #Expectation:
	check10 0x1111222233334444 0x5555666677778888     

	exit0
