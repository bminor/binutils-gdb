.include "t-macros.i"

	start

	# ensure that the US bit is clear
	load r2 0xff
	stsr psw, r3
	and r2, r3
	ldsr r3, psw

	# put something into EP
	load ep ep_base
	
test_sldb1:
	sld.b 0[ep], r4
	check1 r4 0xffffff80
	
test_sldbu1:
	sld.bu 0[ep], r4
	check1 r4, 0x80

test_sldh1:
	sld.h 0[ep], r4
	check1 r4 0xffff8080
	
test_sldhu1:
	sld.hu 0[ep], r4
	check1 r4, 0x8080

	
	# Now set the US bit
	load r2 0x100
	stsr psw, r3
	or r2, r3
	ldsr r3, psw

	
test_sldb2:
	sld.b 0[ep], r4
	check1 r4, 0x80
	
test_sldbu2:
	sld.bu 0[ep], r4
	check1 r4 0xffffff80

test_sldh2:
	sld.h 0[ep], r4
	check1 r4, 0x8080
	
test_sldhu2:
	sld.hu 0[ep], r4
	check1 r4 0xffff8080

	exit0

ep_base:
	.short 0x8080
