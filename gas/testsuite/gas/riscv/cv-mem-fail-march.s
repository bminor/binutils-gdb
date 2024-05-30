# Absence of xcvmem march option disables all CORE-V mem extensions
target:
	cv.lb t4, t2(t0)
	cv.lbu t6, t1(t4)
	cv.lh t2, t0(t3)
	cv.lhu t0, t5(t1)
	cv.lw t1, t3(t6)
	cv.lb t4, (t0), t2
	cv.lbu t6, (t4), t1
	cv.lh t2, (t3), t0
	cv.lhu t0, (t1), t5
	cv.lw t1, (t6), t3
	cv.lb t4, (t0), 23
	cv.lbu t6, (t4), 0
	cv.lh t2, (t3), 77
	cv.lhu t0, (t1), 101
	cv.lw t1, (t6), 6
	cv.sb t0, t1(t2)
	cv.sh t1, t3(t4)
	cv.sw t1, t2(t4)
	cv.sb t0, (t2), t1
	cv.sh t1, (t2), t6
	cv.sw t5, (t2), t6
	cv.sb t6, (t1), 10
	cv.sh t3, (t5), 80
	cv.sw t1, (t4), 20
