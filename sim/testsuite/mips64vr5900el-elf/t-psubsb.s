.include "t-macros.i"

	start

test_psubsb:	
        load $8 0x0200250148026b03 0x8efcb1fdd4fef780
        load $9 0x0180028003810482 0x057c067d077e087f
        psubsb  $10,$8,$9
	check10 0x017f237f457f677f 0x8980ab80cd80ef80
 
	exit0
