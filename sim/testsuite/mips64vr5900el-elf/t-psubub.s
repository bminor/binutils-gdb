.include "t-macros.i"

	start

test_psubub:	
        load $8 0x0200250148026b03 0x8e04b105d406f77f
        load $9 0x0101020203030404 0x0505060607070880
        psubub  $10,$8,$9
	check10 0x0100230045006700 0x8900ab00cd00ef00
 
	exit0
