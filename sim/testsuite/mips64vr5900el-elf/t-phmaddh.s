.include "t-macros.i"

	start

test_phmaddh:	
	load $7 0x8000800080007fff 0x800080007fff8000
	load $8 0x8000800080007fff 0x80007fff80007fff
        phmaddh    $10,$7,$8      #Expectation:
        check10 0x800000007fff0001 0x0000800080010000
        #checkHI xxxxxxxxxx80000000 0xxxxxxxxx00008000
        #checkLO xxxxxxxxxx7fff0001 0xxxxxxxxx80010000
	pmfhl.lw $10
	check10 0x800000007fff0001 0x0000800080010000

	exit0
