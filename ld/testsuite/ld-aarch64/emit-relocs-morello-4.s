// Checking that having a relocation against very large arrays that are not
// problematic w.r.t. capability bounds representation do not error.
// The below is somthing that requires being padded
.bss
.globl bigarray
.p2align 4
bigarray:
	.zero 64004
	.zero 12                 // Tail padding to ensure precise bounds.
	.size bigarray, 64016

.text
.globl _start
.type _start STT_FUNC
_start:
	adrp	c0, :got:bigarray
	ldr	c0, [c0, #:got_lo12:bigarray]
