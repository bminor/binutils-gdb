# Comparison target must be a register
foo:
	cv.beqimm 20, 10, foo
	cv.bneimm 8, -4, foo
