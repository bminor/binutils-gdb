.include "t-macros.i"

	start

test_pmfhl_slw_ok:	
        load $7 0xffffffffffffffff 0xffffffff7fffffff
	load $8 0xffffffffffffffff 0xffffffffffffffff
        pmthi   $7
        pmtlo   $8
        nop
        nop
        pmfhl.slw $10   #Expectation:
	check10 0xffffffffffffffff 0x000000007fffffff
 
test_pmfhl_slw:	
        load $7 0xffffffffffffffff 0xffffffff7fffffff
	load $8 0xffffffff00000000 0xffffffffffffffff
        pmthi   $7
        pmtlo   $8
        nop
        nop
        pmfhl.slw $10   #Expectation:
	check10 0xffffffff80000000 0x000000007fffffff
 
	exit0
