# Destination operand must be a register
target:
	cv.lb 20, 10(t1)
	cv.lb 32, (t2), 15
	cv.lb 40, t2(t3)
	cv.lb 28, (t4), t3
	cv.lbu 16, 20(t5)
	cv.lbu 20, (t6), 30
	cv.lbu 44, t4(t1)
	cv.lbu 48, (t2), t5
	cv.lh 52, 25(t3)
	cv.lh 12, (t4), 10
	cv.lh 16, t6(t5)
	cv.lh 36, (t6), t1
	cv.lhu 24, 35(t1)
	cv.lhu 12, (t2), 13
	cv.lhu 32, t2(t3)
	cv.lhu 40, (t4), t3
	cv.lw 44, 18(t5)
	cv.lw 48, (t6), 8
	cv.lw 24, t4(t1)
	cv.lw 12, (t2), t5
