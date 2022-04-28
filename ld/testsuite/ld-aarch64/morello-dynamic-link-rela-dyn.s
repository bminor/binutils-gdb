	.arch morello+crc+c64
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
.LFB0:
	.cfi_startproc purecap
	adrp	c0, :got:weakval
	ldr	c0, [c0, #:got_lo12:weakval]
	adrp	c0, :got:globval
	ldr	c0, [c0, #:got_lo12:globval]
	adrp	c0, :got:val
	ldr	c0, [c0, #:got_lo12:val]
	adrp	c0, :got:var
	ldr	c0, [c0, #:got_lo12:var]
	str	wzr, [c0]
	ret
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
.data
	.type val, %object
val:
	.byte 0x42
	.byte 0x42
	.byte 0x42
	.size val, .-val
	.chericap val
	.global globval
globval:
	.byte 0x1
	.byte 0x1
	.byte 0x1
	.size globval, .-globval
	.weak weakval
weakval:
	.byte 0x1
	.byte 0x1
	.byte 0x1
	.size weakval, .-weakval
