# Offset operand must be in range [-2048, 2047]
target:
	cv.sb t2, -2049(t1)
	cv.sb t2, (t1), -2049
	cv.sh t2, -2049(t1)
	cv.sh t2, (t1), -2049
	cv.sw t2, -2049(t1)
	cv.sw t2, (t1), -2049
	cv.sb t2, -3669(t1)
	cv.sb t2, (t1), -3669
	cv.sh t2, -3669(t1)
	cv.sh t2, (t1), -3669
	cv.sw t2, -3669(t1)
	cv.sw t2, (t1), -3669
	cv.sb t2, 2048(t1)
	cv.sb t2, (t1), 2048
	cv.sh t2, 2048(t1)
	cv.sh t2, (t1), 2048
	cv.sw t2, 2048(t1)
	cv.sw t2, (t1), 2048
	cv.sb t2, 5341(t1)
	cv.sb t2, (t1), 5341
	cv.sh t2, 5341(t1)
	cv.sh t2, (t1), 5341
	cv.sw t2, 5341(t1)
	cv.sw t2, (t1), 5341
