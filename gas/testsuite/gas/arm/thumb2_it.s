	.text
	.thumb
	.syntax unified
	.thumb_func
foo:
	ittet eq
	addeq r0, r0, r2
	addeq r0, r0, r8
	addne r0, r1, r2
	addseq r0, r1, r2
	add r0, r0, r2
	add r0, r0, r8
	adds r0, r0, r2
	adds r0, r0, r8
	adds r0, r1, r2

	itet eq
	orreq r0, r0, r2
	orrne r0, r0, r8
	orrseq r0, r0, r2
	orr r0, r0, r2
	orr r0, r0, r8
	orrs r0, r0, r2

