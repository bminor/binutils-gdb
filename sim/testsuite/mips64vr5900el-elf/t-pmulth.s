.include "t-macros.i"

	start

test_pmulth:
	load $8 0x1234ffff2b3c8000 0x33947fff37bc7fff
	load $9 0x0001ffff00028000 0x00037fff00048000
        pmulth	$10,$8,$9   #Expectation:
        check10 0x0000000140000000 0x3fff0001c0008000
        checkHI 0x0000123400000001 0x00009abc3fff0001
        checkLO 0x0000567840000000 0x0000def0c0008000

	exit0
