# Base operand must be a register
target:
	cv.sb t0, 10(12)
	cv.sb t1, (24), 20
	cv.sb t2, t1(25)
	cv.sb t3, (75), t2
	cv.sh t4, 30(13)
	cv.sh t5, (16), 40
	cv.sh t6, t3(31)
	cv.sh t0, (37), t4
	cv.sw t1, 12(51)
	cv.sw t2, (43), 16
	cv.sw t3, t5(61)
	cv.sw t4, (67), t6
	cv.lb t0, 12(12)
	cv.lb t1, (24), 13
	cv.lb t2, t3(25)
	cv.lb t3, (75), t4
	cv.lbu t4, 22(51)
	cv.lbu t5, (43), 10
	cv.lbu t6, t5(61)
	cv.lbu t0, (67), t6
	cv.lh t1, 19(13)
	cv.lh t2, (16), 41
	cv.lh t3, t0(31)
	cv.lh t4, (37), t1
	cv.lhu t5, 15(14)
	cv.lhu t6, (17), 12
	cv.lhu t0, t2(14)
	cv.lhu t1, (39), t3
	cv.lw t2, 4(24)
	cv.lw t3, (21), 6
	cv.lw t5, t4(16)
	cv.lw t4, (47), t5
