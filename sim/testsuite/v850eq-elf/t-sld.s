.include "t-macros.i"

	start

	# US bit in the PSW
	load r2 PSW_US
	stsr psw, r3
	or r2, r3
	ldsr r3, psw

	# check that the bit really was set
	stsr psw, r3
	and r2, r3
	check1x 1 r3 PSW_US
	

	# put something into EP
	load ep ep_base
	
test_sldb1:
	sld.b 0[ep], r4
	check1x 2 r4 0xffffff80
	
test_sldbu1:
	sld.bu 0[ep], r4
	check1x 3 r4, 0x80

test_sldh1:
	sld.h 0[ep], r4
	check1x 4 r4 0xffff8080
	
test_sldhu1:
	sld.hu 0[ep], r4
	check1x 5 r4, 0x8080

test_sldw1:
	sld.w 0[ep], r4
	check1x 5 r4, 0x80808080

	
	# Now clear the US bit - switch signed/unsigned
	load r2 PSW_US
	not r2, r2
	stsr psw, r3
	and r2, r3
	ldsr r3, psw

	
test_sldb2:
	sld.b 0[ep], r4
	check1x 6 r4, 0x80
	
test_sldbu2:
	sld.bu 0[ep], r4
	check1x 7 r4 0xffffff80

test_sldh2:
	sld.h 0[ep], r4
	check1x 8 r4, 0x8080
	
test_sldhu2:
	sld.hu 0[ep], r4
	check1x 9 r4 0xffff8080

test_sldw2:
	sld.w 0[ep], r4
	check1x 5 r4, 0x80808080

	exit0

	.align 2
ep_base:
	.long 0x80808080

