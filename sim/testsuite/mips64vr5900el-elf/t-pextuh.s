.include "t-macros.i"

	start

test_pextuh:	
	load $8 0x0807060504030201 0x0000000000000000
	load $9 0x100F0E0D0C0B0A09 0x0000000000000000
        pextuh  $10, $8, $9
	check10 0x0807100F06050E0D 0x04030C0B02010A09

	exit0
