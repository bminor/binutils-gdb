	strb	w0, [x0, w0, uxtw]
	strb	wzr, [x0, w0, uxtw]
	strb	w0, [sp, w0, uxtw]
	strb	w0, [x0, wzr, uxtw]
	strb	w0, [x0, w0, uxtw #0]
	strb	w0, [x0, w0, sxtw]
	strb	w0, [x0, wzr, sxtw]
	strb	w0, [x0, w0, sxtw #0]
	strb	w0, [x0, x0]
	strb	w0, [x0, xzr]
	strb	w0, [x0, x0, lsl #0]
	strb	w0, [x0, x0, sxtx]
	strb	w0, [x0, xzr, sxtx]
	strb	w0, [x0, x0, sxtx #0]

	ldrb	w0, [x0, w0, uxtw]
	ldrb	wzr, [x0, w0, uxtw]
	ldrb	w0, [sp, w0, uxtw]
	ldrb	w0, [x0, wzr, uxtw]
	ldrb	w0, [x0, w0, uxtw #0]
	ldrb	w0, [x0, w0, sxtw]
	ldrb	w0, [x0, wzr, sxtw]
	ldrb	w0, [x0, w0, sxtw #0]
	ldrb	w0, [x0, x0]
	ldrb	w0, [x0, xzr]
	ldrb	w0, [x0, x0, lsl #0]
	ldrb	w0, [x0, x0, sxtx]
	ldrb	w0, [x0, xzr, sxtx]
	ldrb	w0, [x0, x0, sxtx #0]

	ldrsb	x0, [x0, w0, uxtw]
	ldrsb	xzr, [x0, w0, uxtw]
	ldrsb	x0, [sp, w0, uxtw]
	ldrsb	x0, [x0, wzr, uxtw]
	ldrsb	x0, [x0, w0, uxtw #0]
	ldrsb	x0, [x0, w0, sxtw]
	ldrsb	x0, [x0, wzr, sxtw]
	ldrsb	x0, [x0, w0, sxtw #0]
	ldrsb	x0, [x0, x0]
	ldrsb	x0, [x0, xzr]
	ldrsb	x0, [x0, x0, lsl #0]
	ldrsb	x0, [x0, x0, sxtx]
	ldrsb	x0, [x0, xzr, sxtx]
	ldrsb	x0, [x0, x0, sxtx #0]
	ldrsb	w0, [x0, w0, uxtw]
	ldrsb	wzr, [x0, w0, uxtw]
	ldrsb	w0, [sp, w0, uxtw]
	ldrsb	w0, [x0, wzr, uxtw]
	ldrsb	w0, [x0, w0, uxtw #0]
	ldrsb	w0, [x0, w0, sxtw]
	ldrsb	w0, [x0, wzr, sxtw]
	ldrsb	w0, [x0, w0, sxtw #0]
	ldrsb	w0, [x0, x0]
	ldrsb	w0, [x0, xzr]
	ldrsb	w0, [x0, x0, lsl #0]
	ldrsb	w0, [x0, x0, sxtx]
	ldrsb	w0, [x0, xzr, sxtx]
	ldrsb	w0, [x0, x0, sxtx #0]

	strh	w0, [x0, w0, uxtw]
	strh	wzr, [x0, w0, uxtw]
	strh	w0, [sp, w0, uxtw]
	strh	w0, [x0, wzr, uxtw]
	strh	w0, [x0, w0, uxtw #0]
	strh	w0, [x0, w0, uxtw #1]
	strh	w0, [x0, w0, sxtw]
	strh	w0, [x0, wzr, sxtw]
	strh	w0, [x0, w0, sxtw #0]
	strh	w0, [x0, w0, sxtw #1]
	strh	w0, [x0, x0]
	strh	w0, [x0, xzr]
	strh	w0, [x0, x0, lsl #0]
	strh	w0, [x0, x0, lsl #1]
	strh	w0, [x0, x0, sxtx]
	strh	w0, [x0, xzr, sxtx]
	strh	w0, [x0, x0, sxtx #0]
	strh	w0, [x0, x0, sxtx #1]

	ldrh	w0, [x0, w0, uxtw]
	ldrh	wzr, [x0, w0, uxtw]
	ldrh	w0, [sp, w0, uxtw]
	ldrh	w0, [x0, wzr, uxtw]
	ldrh	w0, [x0, w0, uxtw #0]
	ldrh	w0, [x0, w0, uxtw #1]
	ldrh	w0, [x0, w0, sxtw]
	ldrh	w0, [x0, wzr, sxtw]
	ldrh	w0, [x0, w0, sxtw #0]
	ldrh	w0, [x0, w0, sxtw #1]
	ldrh	w0, [x0, x0]
	ldrh	w0, [x0, xzr]
	ldrh	w0, [x0, x0, lsl #0]
	ldrh	w0, [x0, x0, lsl #1]
	ldrh	w0, [x0, x0, sxtx]
	ldrh	w0, [x0, xzr, sxtx]
	ldrh	w0, [x0, x0, sxtx #0]
	ldrh	w0, [x0, x0, sxtx #1]

	ldrsh	x0, [x0, w0, uxtw]
	ldrsh	xzr, [x0, w0, uxtw]
	ldrsh	x0, [sp, w0, uxtw]
	ldrsh	x0, [x0, wzr, uxtw]
	ldrsh	x0, [x0, w0, uxtw #0]
	ldrsh	x0, [x0, w0, uxtw #1]
	ldrsh	x0, [x0, w0, sxtw]
	ldrsh	x0, [x0, wzr, sxtw]
	ldrsh	x0, [x0, w0, sxtw #0]
	ldrsh	x0, [x0, w0, sxtw #1]
	ldrsh	x0, [x0, x0]
	ldrsh	x0, [x0, xzr]
	ldrsh	x0, [x0, x0, lsl #0]
	ldrsh	x0, [x0, x0, lsl #1]
	ldrsh	x0, [x0, x0, sxtx]
	ldrsh	x0, [x0, xzr, sxtx]
	ldrsh	x0, [x0, x0, sxtx #0]
	ldrsh	x0, [x0, x0, sxtx #1]
	ldrsh	w0, [x0, w0, uxtw]
	ldrsh	wzr, [x0, w0, uxtw]
	ldrsh	w0, [sp, w0, uxtw]
	ldrsh	w0, [x0, wzr, uxtw]
	ldrsh	w0, [x0, w0, uxtw #0]
	ldrsh	w0, [x0, w0, uxtw #1]
	ldrsh	w0, [x0, w0, sxtw]
	ldrsh	w0, [x0, wzr, sxtw]
	ldrsh	w0, [x0, w0, sxtw #0]
	ldrsh	w0, [x0, w0, sxtw #1]
	ldrsh	w0, [x0, x0]
	ldrsh	w0, [x0, xzr]
	ldrsh	w0, [x0, x0, lsl #0]
	ldrsh	w0, [x0, x0, lsl #1]
	ldrsh	w0, [x0, x0, sxtx]
	ldrsh	w0, [x0, xzr, sxtx]
	ldrsh	w0, [x0, x0, sxtx #0]
	ldrsh	w0, [x0, x0, sxtx #1]

	ldrsw	x0, [x0, w0, uxtw]
	ldrsw	xzr, [x0, w0, uxtw]
	ldrsw	x0, [sp, w0, uxtw]
	ldrsw	x0, [x0, wzr, uxtw]
	ldrsw	x0, [x0, w0, uxtw #0]
	ldrsw	x0, [x0, w0, uxtw #2]
	ldrsw	x0, [x0, w0, sxtw]
	ldrsw	x0, [x0, wzr, sxtw]
	ldrsw	x0, [x0, w0, sxtw #0]
	ldrsw	x0, [x0, w0, sxtw #2]
	ldrsw	x0, [x0, x0]
	ldrsw	x0, [x0, xzr]
	ldrsw	x0, [x0, x0, lsl #0]
	ldrsw	x0, [x0, x0, lsl #2]
	ldrsw	x0, [x0, x0, sxtx]
	ldrsw	x0, [x0, xzr, sxtx]
	ldrsw	x0, [x0, x0, sxtx #0]
	ldrsw	x0, [x0, x0, sxtx #2]
