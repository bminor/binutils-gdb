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
; Basic immediate instruction tests.  
;
; We could/should test some of the corner cases for register and 
; immediate fields.  We should also check the assorted field
; selectors to make sure they're handled correctly.
	fmpyfadd,sgl %fr5R,%fr10L,%fr13R,%fr14L
	fmpyfadd,dbl %fr5,%fr10,%fr13,%fr14
	fmpyfadd,sgl %fr18R,%fr27L,%fr6R,%fr21L
	fmpyfadd,dbl %fr18,%fr27,%fr6,%fr21

	fmpynfadd,sgl %fr5R,%fr10L,%fr13R,%fr14L
	fmpynfadd,dbl %fr5,%fr10,%fr13,%fr14
	fmpynfadd,sgl %fr18R,%fr27L,%fr6R,%fr21L
	fmpynfadd,dbl %fr18,%fr27,%fr6,%fr21

	fneg,sgl %fr5,%fr10R
	fneg,dbl %fr5,%fr10
	fneg,quad %fr5,%fr10
	fneg,sgl %fr20R,%fr24L
	fneg,dbl %fr20,%fr24

	fnegabs,sgl %fr5,%fr10R
	fnegabs,dbl %fr5,%fr10
	fnegabs,quad %fr5,%fr10
	fnegabs,sgl %fr20R,%fr24L
	fnegabs,dbl %fr20,%fr24

