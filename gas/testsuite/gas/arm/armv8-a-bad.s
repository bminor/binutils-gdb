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
