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

	hadd %r7,%r11,%r12
	hadd,ss %r7,%r11,%r12
	hadd,us %r7,%r11,%r12

	havg %r7,%r11,%r12
	hshl %r7,13,%r11
	hshladd %r7,1,%r11,%r12
	hshladd %r7,2,%r11,%r12
	hshladd %r7,3,%r11,%r12
	hshr %r7,13,%r11
	hshr,s %r7,13,%r11
	hshr,u %r7,13,%r11
	hshradd %r7,1,%r11,%r12
	hshradd %r7,2,%r11,%r12
	hshradd %r7,3,%r11,%r12

	hsub %r14,%r22,%r9
	hsub,ss %r14,%r22,%r9
	hsub,us %r14,%r22,%r9

	mixh,l %r14,%r22,%r9
	mixh,r %r14,%r22,%r9
	mixw,l %r14,%r22,%r9
	mixw,r %r14,%r22,%r9

	permh,0321 %r22,%r9
	permh,2222 %r22,%r9
	permh,3030 %r22,%r9

