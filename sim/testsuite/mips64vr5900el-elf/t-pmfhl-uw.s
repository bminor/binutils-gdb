.include "t-macros.i"

	start

test_pmfhl_uw:
	load $7 0x11111111ffffffff 0x33333333ffffffff
	load $8 0x22222222ffffffff 0x44444444ffffffff
        pmthi   $7
        pmtlo   $8
        nop
        nop
        pmfhl.uw $10    #Expectation:
        check10 0x1111111122222222 0x3333333344444444        
 
	exit0
