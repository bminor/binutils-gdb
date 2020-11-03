/* Atomic 64-byte load/store instructions require Armv8.7-a extension.  */
.arch armv8.6-a

/* Single-copy Atomic 64-byte Load.  */
	ld64b x0, [x1]

/* Single-copy Atomic 64-byte Store without Return.  */
	st64b x0, [x1]

/* Single-copy Atomic 64-byte Store with Return.  */
	st64bv x0, x1, [x2]

/* Single-copy Atomic 64-byte EL0 Store with Return.  */
	st64bv0 x0, x1, [x2]
