.section .tbss
.globl var2
var2:
        .word   0
        .size   var2, .-var2

var3:
        .word   0
        .word   0
        .word   0
        .word   0
        .word   0
        .size   var3, .-var3

.section .text
.ifndef STATIC
f1:
	bl	extf
	mrs	c2, CTPIDR_EL0
	nop
	adrp	c0, :tlsdesc:var1
	ldr	c1, [c0, #:tlsdesc_lo12:var1]
	add	c0, c0, #:tlsdesc_lo12:var1
	.tlsdesccall var1
	blr	c1
	scbnds	c0, c0, x1
.endif

f2:
	bl	_start
	mrs	c2, CTPIDR_EL0
	nop
	adrp	c0, :tlsdesc:var2
	ldr	c1, [c0, #:tlsdesc_lo12:var2]
	add	c0, c0, #:tlsdesc_lo12:var2
	.tlsdesccall var2
	blr	c1
	scbnds	c0, c0, x1

.globl _start
.type _start, STT_FUNC
_start:
	mrs	c2, CTPIDR_EL0
	nop
	adrp	c0, :tlsdesc:var3
	ldr	c1, [c0, #:tlsdesc_lo12:var3]
	add	c0, c0, #:tlsdesc_lo12:var3
	.tlsdesccall var3
	blr	c1
	scbnds	c0, c0, x1
