// Checking that having a relocation against very large arrays whose size can
// not be represented gives the corresponding error.
.bss
.globl bigarray
.p2align 4
bigarray:
	.zero 64004
	.size bigarray, 64004

.text
.globl _start
.type _start STT_FUNC
_start:
	adrp	c0, :got:bigarray
	ldr	c0, [c0, #:got_lo12:bigarray]
