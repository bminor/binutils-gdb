.include "t-macros.i"

	start

test_pextuw:	
	load $8 0x0807060504030201 0x0000000000000000
	load $9 0x100F0E0D0C0B0A09 0x0000000000000000
        pextuw  $10, $8, $9
	check10 0x08070605100F0E0D 0x040302010C0B0A09

	exit0
