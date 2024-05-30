# Comparison value must be an immediate in range [-16, +15]
foo:
	cv.beqimm t0, -17, foo
	cv.beqimm t2, -32, foo
	cv.beqimm t4, 16, foo
	cv.beqimm t3, 44, foo
	cv.bneimm t0, -17, foo
	cv.bneimm t2, -32, foo
	cv.bneimm t4, 16, foo
	cv.bneimm t3, 44, foo
