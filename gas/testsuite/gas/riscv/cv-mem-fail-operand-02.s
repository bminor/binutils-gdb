# Source operand must be a register
target:
	cv.sb 12, 10(t1)
	cv.sb 14, (t2), 20
	cv.sb 16, t1(t3)
	cv.sb 20, (t4), t2
	cv.sh 30, 30(t5)
	cv.sh 15, (t6), 40
	cv.sh 45, t3(t1)
	cv.sh 52, (t2), t4
	cv.sw 12, 12(t3)
	cv.sw 10, (t4), 16
	cv.sw 82, t5(t5)
	cv.sw 14, (t1), t6
