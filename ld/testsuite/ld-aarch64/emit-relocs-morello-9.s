# Attempting to check that the LSB is set on all relocations to a function
# symbol.
#
# This should only happen for those relocations which load an address into a
# register, since relocations that jump to a PC relative address like `bl`
# should not include the LSB.
.text
.global _start
.type _start,@function
.type otherstart,@function
_start:
otherstart:
	movk	x0, #:abs_g0_nc:_start
	movk	x0, #:abs_g0_nc:otherstart
	adr	c0, _start
	adr	c0, otherstart
	add	c0, c0, :lo12:_start
	add	c0, c0, :lo12:otherstart
	# The below are not as much of a worry if they go wrong since they
	# check overflow, and the likelyhood of there being a function which
	# fits in the lowest 16 bits of an address is low.  However, we can
	# still test it in our testsuite with arguments to the linker, so we
	# still get to check this edge case.
	movz	x0, #:abs_g0_s:_start
	movz	x0, #:abs_g0_s:otherstart
	movk	x0, #:abs_g0:_start
	movk	x0, #:abs_g0:otherstart
.data
.align 4
.global val
val:
	# LSB should be included in the value of function symbols even if they
	# are just added via absolute relocations.
	.hword _start
	.hword 0
	.word _start
	.xword _start
	.hword otherstart
	.hword 0
	.word otherstart
	.xword otherstart
	.size val, .-val
