# Source file used to test the dli macro.
	
	dli	$4,0
	dli	$4,1
	dli	$4,-1
	dli	$4,0x8000
	dli	$4,-0x8000
	dli	$4,0x10000
	dli	$4,0x1a5a5
	dli	$4,0x80001234
	dli	$4,0xffffffff
	dli	$4,0x00000000ffffffff
	dli	$4,0xffffffffffffffff
	dli	$4,0x000fffffffffffff
	dli	$4,0xffffffff80001234
	dli	$4,0xffff800012345678
	dli	$4,0x8000123456780000
	dli	$4,0xffffffffffff8765
	dli	$4,0xffffffffffff4321
