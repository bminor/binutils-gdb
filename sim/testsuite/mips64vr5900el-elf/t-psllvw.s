.include "t-macros.i"

	start

test_psllvw:	
        load $8 0x00000000ffffffff 0x00000000ffffffff
        load $9 0x000000000000001f 0x000000000000001f
        psllvw  $10,$8,$9       #Expectation:
        check10 0xffffffff80000000 0xffffffff80000000

	exit0
