.include "t-macros.i"

	start

test_phmsubh:	
	load $7 0x8000800080007fff 0x800080007fff8000
	load $8 0x8000800080007fff 0x80007fff80007fff
        phmsubh    $10,$7,$8      #Expectation:
	check10 0x000000000000ffff 0x7fff800000000000
	#checkHI 0xxxxxxxxx00000000 0xxxxxxxxx7fff8000
	#checkLO 0xxxxxxxxx0000ffff 0xxxxxxxxx00000000
	pmfhl.lw $10
	check10 0x000000000000ffff 0x7fff800000000000

	exit0
