	.macro start
	.text
	.align 2
	.globl _start
_start:
	ldi r0, 0
	.endm


	.macro exit47
	ldi r4, 1
	ldi r0, 47
	trap 15
	.endm


	.macro exit0
	ldi r4, 1
	ldi r0, 0
	trap 15
	.endm


	.macro load reg val
	ldi \reg, #\val
	.endm


	.macro load2w reg hi lo
	ld2w \reg, @(1f,r0)
	.data
	.align 2
1:	.short \hi
	.short \lo
	.text
	.endm


	.macro check exit reg val
	cmpeqi	\reg, #\val
	brf0t 1f
0:	ldi r4, 1
	ldi r0, \exit
	trap 15
1:
	.endm


	.macro check2w2 exit reg hi lo
	st2w	\reg, @(1f,r0)
	ld	r2, @(1f, r0)
	cmpeqi	r2, #\hi
	brf0f	0f
	ld	r2, @(1f + 2, r0)
	cmpeqi	r2, #\lo
	brf0f	0f
	bra	2f
0:	ldi r4, 1
	ldi r0, \exit
	trap 15
	.data
	.align 2
1:	.long 0
	.text
2:
	.endm


	.macro loadacc2 acc guard hi lo
	ldi	r2, #\lo
	mvtaclo	r2, \acc
	ldi	r2, #\hi
	mvtachi	r2, \acc
	ldi	r2, #\guard
	mvtacg	r2, \acc
	.endm


	.macro checkacc2 exit acc guard hi lo
	ldi	r2, #\guard
	mvfacg	r3, \acc
	cmpeq	r2, r3
	brf0f	0f
	ldi	r2, #\hi
	mvfachi	r3, \acc
	cmpeq	r2, r3
	brf0f	0f
	ldi	r2, #\lo
	mvfaclo	r3, \acc
	cmpeq	r2, r3
	brf0f	0f
	bra	4f
0:	ldi r4, 1
	ldi r0, \exit
	trap 15
4:
	.endm


	.macro loadpsw2 val
	ldi r2, #\val
	mvtc	r2, cr0
	.endm


	.macro checkpsw2 exit val
	mvfc	r2, cr0
	cmpeqi	r2, #\val
	brf0t	1f
	ldi r4, 1
	ldi r0, \exit
	trap 15
1:
	.endm


	.macro hello
	;; 4:write (1, string, strlen (string))
	ldi r4, 4
	ldi r0, 1
	ldi r1, 1f
	ldi r2, 2f-1f-1
	trap 15
	.section	.rodata
1:	.string "Hello World!\n"
2:	.align 2
	.text
	.endm


	PSW_SM = 0x8000
	PSW_01 = 0x4000
	PSW_EA = 0x2000
	PSW_DB = 0x1000
	PSW_DM = 0x0800
	PSW_IE = 0x0400
	PSW_RP = 0x0200
	PSW_MD = 0x0100
	PSW_FX = 0x0080
	PSW_ST = 0x0040
	PSW_10 = 0x0020
	PSW_11 = 0x0010
	PSW_F0 = 0x0008
	PSW_F1 = 0x0004
	PSW_14 = 0x0002
	PSW_C  = 0x0001
