.include "t-macros.i"

	start

test_pextlh:	
	load $8 0x0000000000000000 0x0807060504030201
	load $9 0x0000000000000000 0x100F0E0D0C0B0A09
        pextlh  $10, $8, $9
	check10 0x0807100F06050E0D 0x04030C0B02010A09
	
	exit0
