	.syntax unified
	.text
	.arch armv8-a

	// SWP
	.arm
	swp r0, r1, [r2]

	// deprecated MCRs
	mcr p15, 0, r0, c7, c5, 4
	mcr p15, 0, r1, c7, c10, 4
	mcr p15, 0, r2, c7, c10, 5
	mrc p14, 6, r1, c0, c0, 0
	mrc p14, 6, r0, c1, c0, 0

	// deprecated SETEND
	setend be

	.thumb
	setend le

	// HLT A32
	.arm
	hlt 0x10000
	hltne 0x1

	// HLT T32
	.thumb
	hlt 64
	it ne
	hltne 0

	// STRL A32
	.arm
	strlb pc, [r0]
	strlb r0, [pc]
	strlh pc, [r0]
	strlh r0, [pc]
	strl pc, [r0]
	strl r0, [pc]
	strlexb r1, pc, [r0]
	strlexb r1, r0, [pc]
	strlexb pc, r0, [r1]
	strlexb r0, r0, [r1]
	strlexb r0, r1, [r0]
	strlexh r1, pc, [r0]
	strlexh r1, r0, [pc]
	strlexh pc, r0, [r1]
	strlexh r0, r0, [r1]
	strlexh r0, r1, [r0]
	strlex r1, pc, [r0]
	strlex r1, r0, [pc]
	strlex pc, r0, [r1]
	strlex r0, r0, [r1]
	strlex r0, r1, [r0]
	strlexd r1, lr, [r0]
	strlexd r1, r0, [pc]
	strlexd pc, r0, [r1]
	strlexd r0, r0, [r1]
	strlexd r0, r2, [r0]
	strlexd r0, r1, [r2]

	// STRL T32
	.thumb
	strlb pc, [r0]
	strlb r0, [pc]
	strlh pc, [r0]
	strlh r0, [pc]
	strl pc, [r0]
	strl r0, [pc]
	strlexb r1, pc, [r0]
	strlexb r1, r0, [pc]
	strlexb pc, r0, [r1]
	strlexb r0, r0, [r1]
	strlexb r0, r1, [r0]
	strlexh r1, pc, [r0]
	strlexh r1, r0, [pc]
	strlexh pc, r0, [r1]
	strlexh r0, r0, [r1]
	strlexh r0, r1, [r0]
	strlex r1, pc, [r0]
	strlex r1, r0, [pc]
	strlex pc, r0, [r1]
	strlex r0, r0, [r1]
	strlex r0, r1, [r0]
	strlexd r1, lr, [r0]
	strlexd r1, r0, [pc]
	strlexd pc, r0, [r1]
	strlexd r0, r0, [r1]
	strlexd r0, r2, [r0]
	strlexd r0, r1, [r2]

	// LDRA A32
	.arm
	ldrab pc, [r0]
	ldrab r0, [pc]
	ldrah pc, [r0]
	ldrah r0, [pc]
	ldra pc, [r0]
	ldra r0, [pc]
	ldraexb pc, [r0]
	ldraexb r0, [pc]
	ldraexh pc, [r0]
	ldraexh r0, [pc]
	ldraex pc, [r0]
	ldraex r0, [pc]
	ldraexd lr, [r0]
	ldraexd r0, [pc]
	ldraexd r1, [r2]

	// LDRA T32
	.thumb
	ldrab pc, [r0]
	ldrab r0, [pc]
	ldrah pc, [r0]
	ldrah r0, [pc]
	ldra pc, [r0]
	ldra r0, [pc]
	ldraexb pc, [r0]
	ldraexb r0, [pc]
	ldraexh pc, [r0]
	ldraexh r0, [pc]
	ldraex pc, [r0]
	ldraex r0, [pc]
	ldraexd r0, pc, [r0]
	ldraexd pc, r0, [r0]
	ldraexd r1, r0, [pc]
