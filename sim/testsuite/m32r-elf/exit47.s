	;; Return with exit code 47.

	.globl _start
_start:
	ldi8 r2,#47
	ldi8 r1,#1
	ldi8 r0,#0
	trap #0
