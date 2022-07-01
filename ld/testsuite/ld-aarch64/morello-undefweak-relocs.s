        .weak   globalsym
	.global globalsym
        .weak   localsym
        .weak   hiddensym
	.hidden hiddensym

	.data
	.chericap globalsym
	.chericap localsym
	.chericap hiddensym

        .text
        .globl  _start
_start:
	adrp	c0,	:got:globalsym
	ldr	c0, [c0, #:got_lo12:globalsym]
	adrp	c0,	:got:hiddensym
	ldr	c0, [c0, #:got_lo12:hiddensym]
	adrp	c0,	:got:localsym
	ldr	c0, [c0, #:got_lo12:localsym]
        ret
