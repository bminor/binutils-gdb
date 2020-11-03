/* Atomic 64-byte load/store instructions.  */
.arch armv8.7-a+ls64

/* Single-copy Atomic 64-byte Load.  */
	ld64b x0, [x1]
	ld64b x1, [x2]
	ld64b x2, [x4]
	ld64b x4, [x8]
	ld64b x8, [x16]
	ld64b x16, [x1]
	ld64b x16, [sp]

/* Single-copy Atomic 64-byte Store without Return.  */
	st64b x0, [x1]
	st64b x1, [x2]
	st64b x2, [x4]
	st64b x4, [x8]
	st64b x8, [x16]
	st64b x16, [x1]
	st64b x16, [sp]

/* Single-copy Atomic 64-byte Store with Return.  */
	st64bv x0, x1, [x2]
	st64bv x1, x2, [x4]
	st64bv x2, x4, [x8]
	st64bv x4, x8, [x16]
	st64bv x8, x16, [x30]
	st64bv x16, x30, [x0]
	st64bv x30, x1, [x2]
	st64bv x30, x1, [sp]

/* Single-copy Atomic 64-byte EL0 Store with Return.  */
	st64bv0 x0, x1, [x2]
	st64bv0 x1, x2, [x4]
	st64bv0 x2, x4, [x8]
	st64bv0 x4, x8, [x16]
	st64bv0 x8, x16, [x30]
	st64bv0 x16, x30, [x0]
	st64bv0 x30, x1, [x2]
	st64bv0 x30, x1, [sp]
