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
; PA2.0 system instruction tests.  
;
; We could/should test some of the corner cases for register and 
; immediate fields.  We should also check the assorted field
; selectors to make sure they're handled correctly.
	rfi
	rfi,r
	ssm 923,%r4
	rsm 923,%r4
	mfctl,w %cr11,%r4

	probe,r (%sr0,%r5),%r6,%r7
	probei,r (%sr0,%r5),1,%r7
	probe,w (%sr0,%r5),%r6,%r7
	probei,w (%sr0,%r5),1,%r7
	
