	.LEVEL 2.0
	.SPACE $PRIVATE$
	.SUBSPA $DATA$,QUAD=1,ALIGN=8,ACCESS=31
	.SUBSPA $BSS$,QUAD=1,ALIGN=8,ACCESS=31,ZERO,SORT=82
	.SPACE $TEXT$
	.SUBSPA $LIT$,QUAD=0,ALIGN=8,ACCESS=44
	.SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=44,CODE_ONLY

	.SPACE $TEXT$
	.SUBSPA $CODE$

	.align 4
; PA2.0 multimedia (halfword) instruction tests
;
; We could/should test some of the corner cases for register and 
; immediate fields.  We should also check the assorted field
; selectors to make sure they're handled correctly.

	havg %r7,%r11,%r12
	hshladd %r7,1,%r11,%r12
	hshladd %r7,2,%r11,%r12
	hshladd %r7,3,%r11,%r12
	hshradd %r7,1,%r11,%r12
	hshradd %r7,2,%r11,%r12
	hshradd %r7,3,%r11,%r12
