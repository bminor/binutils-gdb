	.SPACE $PRIVATE$
	.SUBSPA $DATA$,QUAD=1,ALIGN=8,ACCESS=31
	.SUBSPA $BSS$,QUAD=1,ALIGN=8,ACCESS=31,ZERO,SORT=82
	.SPACE $TEXT$
	.SUBSPA $LIT$,QUAD=0,ALIGN=8,ACCESS=44
	.SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=44,CODE_ONLY

	.SPACE $TEXT$
	.SUBSPA $CODE$

	.align 4
; A comment.  This should not be interpreted as a label, but both of the
; following statements should.
label_without_colon
label_with_colon:

; An instruction or pseudo-op may begin anywhere after column 0.
 b,n label_without_colon
