.include "t-macros.i"

	start

test_pintoh:	
        load $8 0xffff1111ffff3333 0xffff5555ffff7777
	load $9 0xffff2222ffff4444 0xffff6666ffff8888
        pintoh $10,$8,$9        #Expectation:
	check10 0x1111222233334444 0x5555666677778888
 
	exit0
