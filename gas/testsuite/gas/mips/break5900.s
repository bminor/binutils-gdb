# Source file used to test the 20-bit break instructions
foo:	
	break
	break	0
	break	20
	break	40
	break	1023
	break	0xffff0
	break	0xfffff

# Test div macros to confirm its use of 20-bit break - from div.s testcase


	div	$4,1
	div	$4,$5,1
	divu	$0,$4,$5
	divu	$4,$5
	divu	$4,1
	rem	$4,$5,$6
	remu	$4,$5,2
	ddiv	$4,$5,$6
	ddivu	$4,$5,2
	drem	$4,$5,0x8000

# force some padding, to make objdump consistently report that there's some
# here...
	.space	8
