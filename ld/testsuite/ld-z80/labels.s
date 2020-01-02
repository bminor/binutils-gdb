	.text
	.globl	label1
	.globl	label2
	.globl	label3
	.globl	label4
	.globl	label5
	.globl	label6
	.globl	label7
	.globl	label8
	.globl	label9
	.globl	value8
	.globl	value16
	.globl	value24
	.globl	value32
	.globl	field_0
	.globl	field_1
_start:
label1:
	ld	a,b
label2:
	ld	a,c
label3:
	ld	a,d
label4:
	ld	a,e
label5:
	ld	a,h
label6:
	ld	a,l
label7:
	ld	a,(hl)
label8:
	ld	a,a
label9:
	cpl

value8	.equ 0x12
value16	.equ	0x1234
value24	.equ	0x123456
value32	.equ	0x12345678

field_0	.equ	3
field_1	.equ	field_0 + 1
