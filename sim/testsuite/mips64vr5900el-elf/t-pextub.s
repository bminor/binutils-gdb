.include "t-macros.i"

	start

test_pextub:	
	load $8 0x0807060504030201 0x0000000000000000
	load $9 0x100F0E0D0C0B0A09 0x0000000000000000
        pextub  $10, $8, $9
	check10 0x0810070F060E050D 0x040C030B020A0109

	exit0
