.include "t-macros.i"

	start

test_pmfhl_lw:	
	load $7 0xffffffff11111111 0xffffffff33333333
	load $8 0xffffffff22222222 0xffffffff44444444
        pmthi   $7
        pmtlo   $8
        nop
        nop
        pmfhl.lw $10    #Expectation:
        check10 0x1111111122222222 0x3333333344444444

	exit0
