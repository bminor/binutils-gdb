.include "t-macros.i"

	start

test_pmthl_lw:	
        load $7 0x11111111ffffffff 0x33333333ffffffff
        load $8 0x55555555ffffffff 0x77777777ffffffff
        pmthi   $7
        pmtlo   $8
        nop
        nop
        load $7 0x2222222266666666 0x4444444488888888
        pmthl.lw $7
        checkHI 0x1111111122222222 0x3333333344444444
        checkLO 0x5555555566666666 0x7777777788888888               

	exit0
