# NB: In the below, the 64bit values are embeded into the
# machine code.  The address of which being determined
# by a jal (after a jump to an aligned address).

	.macro load reg hi lo
	j 1f
	nop
	.align 3
1:	jal 2f
	nop
	.quad \hi
	.quad \lo
2:
	ld \reg, 0($31)
	pcpyld	\reg, \reg, $0
	ld \reg, 8($31)
	.endm

	.macro check10 hi lo
	j 1f
	nop
	.align 3
1:	jal 2f
	nop
	.quad \hi
	.quad \lo	
2:	pcpyud	$11, $10, $0
	ld	$5, 0($31)
	beq	$11, $5, 3f
	nop
	li	$4, 47
	break	1023
	nop
3:	nop
	ld	$5, 8($31)
	beq	$10, $5, 3f
	nop
	li	$4, 47
	break	1023
	nop
3:	nop
	.endm

	.macro checkHI hi lo
	j 1f
	nop
	.align 3
1:	jal 2f
	nop
	.quad \hi
	.quad \lo
2:	mfhi1	$11
	ld	$5, 0($31)
	beq	$11, $5, 3f
	nop
	li	$4, 47
	break	1023
	nop
3:	nop
	mfhi	$11
	ld	$5, 8($31)
	beq	$11, $5, 3f
	nop
	li	$4, 47
	break	1023
	nop
3:	nop
	.endm


	.macro checkLO hi lo
	j 1f
	nop
	.align 3
1:	jal 2f
	nop
	.quad \hi
	.quad \lo
2:	mflo1	$11
	ld	$5, 0($31)
	beq	$11, $5, 3f
	nop
	li	$4, 47
	break	1023
	nop
3:	nop
	mflo	$11
	ld	$5, 8($31)
	beq	$11, $5, 3f
	nop
	li	$4, 47
	break	1023
	nop
3:	nop
	.endm

	.macro start
	.text
	.globl _start
	.set noreorder
_start:
	.endm

	.macro exit0
	li	$4, 0
	break 1023
	nop
	.endm
