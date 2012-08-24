	.syntax unified
	.text
	.arch armv8-a

	.arm
foo:
	sevl
	hlt 0x0
	hlt 0xf
	hlt 0xfff0
	strlb r0, [r0]
	strlb r1, [r1]
	strlb r14, [r14]
	strlh r0, [r0]
	strlh r1, [r1]
	strlh r14, [r14]
	strl r0, [r0]
	strl r1, [r1]
	strl r14, [r14]
	strlexb r0, r1, [r14]
	strlexb r1, r14, [r0]
	strlexb r14, r0, [r1]
	strlexh r0, r1, [r14]
	strlexh r1, r14, [r0]
	strlexh r14, r0, [r1]
	strlex r0, r1, [r14]
	strlex r1, r14, [r0]
	strlex r14, r0, [r1]
	strlexd r0, r2, r3, [r14]
	strlexd r1, r12, r13, [r0]
	strlexd r14, r0, r1, [r1]
	ldrab r0, [r0]
	ldrab r1, [r1]
	ldrab r14, [r14]
	ldrah r0, [r0]
	ldrah r1, [r1]
	ldrah r14, [r14]
	ldra r0, [r0]
	ldra r1, [r1]
	ldra r14, [r14]
	ldraexb r0, [r0]
	ldraexb r1, [r1]
	ldraexb r14, [r14]
	ldraexh r0, [r0]
	ldraexh r1, [r1]
	ldraexh r14, [r14]
	ldraex r0, [r0]
	ldraex r1, [r1]
	ldraex r14, [r14]
	ldraexd r0, r1, [r0]
	ldraexd r2, r3, [r1]
	ldraexd r12, r13, [r14]

	.thumb
	.thumb_func
bar:
	sevl
	sevl.n
	sevl.w
	dcps1
	dcps2
	dcps3
	hlt 0
	hlt 63
	strlb r0, [r0]
	strlb r1, [r1]
	strlb r14, [r14]
	strlh r0, [r0]
	strlh r1, [r1]
	strlh r14, [r14]
	strl r0, [r0]
	strl r1, [r1]
	strl r14, [r14]
	strlexb r0, r1, [r14]
	strlexb r1, r14, [r0]
	strlexb r14, r0, [r1]
	strlexh r0, r1, [r14]
	strlexh r1, r14, [r0]
	strlexh r14, r0, [r1]
	strlex r0, r1, [r14]
	strlex r1, r14, [r0]
	strlex r14, r0, [r1]
	strlexd r0, r1, r1, [r14]
	strlexd r1, r14, r14, [r0]
	strlexd r14, r0, r0, [r1]
	ldrab r0, [r0]
	ldrab r1, [r1]
	ldrab r14, [r14]
	ldrah r0, [r0]
	ldrah r1, [r1]
	ldrah r14, [r14]
	ldra r0, [r0]
	ldra r1, [r1]
	ldra r14, [r14]
	ldraexb r0, [r0]
	ldraexb r1, [r1]
	ldraexb r14, [r14]
	ldraexh r0, [r0]
	ldraexh r1, [r1]
	ldraexh r14, [r14]
	ldraex r0, [r0]
	ldraex r1, [r1]
	ldraex r14, [r14]
	ldraexd r0, r1, [r0]
	ldraexd r1, r14, [r1]
	ldraexd r14, r0, [r14]
