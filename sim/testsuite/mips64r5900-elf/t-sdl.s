.include "t-macros.i"

	start

test_sdl:	
        load $8 0x0000000000000000 dest
	load $9 0xbeefdead 0xdeadbeef
	sdl $9, 7($8)
	ld $10, dest

        check10 0x0000000000000000 0xdeadbeef

	exit0

dest:	.long 0
