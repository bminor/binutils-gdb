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

	.macro load reg val
	jr 1f
	.align 2
1:	jarl 2f, \reg 
	.long \val
2:	ld.w 0[\reg], \reg
	.endm

	.macro check1 reg val
	jr 1f
	.align 2
1:	jarl 2f, r1
	.long \val
2:	ld.w 0[r1], r1
	cmp r1, \reg
	be 1f
	exit47
1:	
	.endm

	.macro check1x sts reg val
	jr 1f
	.align 2
1:	jarl 2f, r1
	.long \val
2:	ld.w 0[r1], r1
	cmp r1, \reg
	be 1f
	mov 1, r6
	addi \sts, r0, r7
	trap 31
1:	
	.endm


# definitions of various PSW bits
	PSW_US = 0x100
	PSW_NP = 0x80
	PSW_EP = 0x40
	PSW_ID = 0x20
	PSW_SAT = 0x10
	PSW_CY = 0x8
	PSW_OV = 0x4
	PSW_S = 0x2
	PSW_Z = 0x1


# definitions of various interrupt addresses
	INT_RESET = 0x0
	INT_NMI = 0x10
