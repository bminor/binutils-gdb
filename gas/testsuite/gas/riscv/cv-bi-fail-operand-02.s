# Comparison value must be an immediate
foo:
	cv.beqimm t0, t1, foo
	cv.bneimm t3, t4, foo
