.include "t-macros.i"

	start

test_ppacb:	
	load $8 0x0010000F000E000D 0x000C000B000A0009
	load $9 0x0008000700060005 0x0004000300020001
        ppacb   $10, $8, $9
	check10 0x100F0E0D0C0B0A09 0x0807060504030201
	
	exit0
