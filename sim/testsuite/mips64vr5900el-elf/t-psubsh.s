.include "t-macros.i"

	start

test_psubsh:	
        load $8 0x0124000045690001 0x89aefffecdf3ffff
        load $9 0x0001800000028000 0x00037ffe00047fff
        psubsh  $10,$8,$9
	check10 0x01237fff45677fff 0x89ab8000cdef8000

	exit0
