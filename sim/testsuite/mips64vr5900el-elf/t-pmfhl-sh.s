.include "t-macros.i"

	start

test_pmfhl_sh:	
	load $7 0x7fffffffffff0000 0xffffffff00007fff
	load $8 0x0007ffffffffffff 0xffff00007fffffff
        pmthi   $7
        pmtlo   $8
        nop
        nop
        pmfhl.sh $10    #Expectation:
	check10 0x7fff80007fffffff 0xffff7fff80007fff           

	exit0
