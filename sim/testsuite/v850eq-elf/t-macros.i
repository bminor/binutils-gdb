	.macro start
	.text
	.globl _start
_start:
	.endm

	.macro exit47
	mov 1, r6
	addi 47, r0, r7
	trap 31
	.endm

	.macro exit0
	mov 1, r6
	mov 0, r7
	trap 31
	.endm

	.macro load32 reg val
	jr 1f
	.align 2
1:	jarl 2f, \reg 
	.long \val
2:	ld.w 0[\reg], \reg
	.endm
