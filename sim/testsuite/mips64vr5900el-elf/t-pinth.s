.include "t-macros.i"

	start

test_pinth:	
        load $8 0x1111333355557777 0xffffffffffffffff
        load $9 0xffffffffffffffff 0x2222444466668888
        pinth   $10,$8,$9        #Expectation:
        check10 0x1111222233334444 0x5555666677778888
 
	exit0
