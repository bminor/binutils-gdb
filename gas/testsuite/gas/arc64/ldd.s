; 64-bit double loads

; ldd a, [b]
ldd       r0, [r1]
ldd.di    r0, [r1]
ldd.aw    r0, [r1]
ldd.aw.di r0, [r1]
ldd.ab    r0, [r1]
ldd.ab.di r0, [r1]
ldd.as    r0, [r1]
ldd.as.di r0, [r1]

; ldd a, [b, s9]
ldd       r4, [r3,  255]
ldd.di    r6, [r1, -256]
ldd.aw    r0, [r7,  123]
ldd.aw.di r4, [r1, -123]
ldd.ab    r2, [r1,   10]
ldd.ab.di r0, [r1,   -1]
ldd.as    r4, [r1,  255]
ldd.as.di r4, [r1, -246]

; ldd a, [limm]      -> no writeback (.aw, .ab) can be used
ldd.as    r0, [0x0]
ldd.di    r0, [0x255]
ldd       r0, [-256]
ldd       r4, [0x87654321]

; ldd a, [b, c]
ldd       r0, [r1, r2]
ldd.di    r2, [r3, r4]
ldd.aw    r4, [r5, r6]
ldd.aw.di r6, [r6, r4]
ldd.ab    r0, [r4, r2]
ldd.ab.di r2, [r1, r3]
ldd.as    r4, [r3, r5]
ldd.as.di r6, [r5, r1]

; ldd a, [b, limm]
ldd       r4, [r5,  258]
ldd.as    r4, [r1,  256]
ldd.aw    r0, [r3,  0x12345678]
ldd.ab.di r2, [r4, -0x12345678]

; ldd a, [limm, c]      -> no writeback (.aw, .ab) can be used
ldd       r4, [260, r2]
ldd.as    r4, [256, r1]
ldd       r4, [255, r1]
ldd.di    r0, [ 0x12345678, r5]
ldd.as.di r2, [-0x12345678, r4]

; ldd a, [b, s9]
; use adress-scaling to fit numbers into 4-byte instructions
ldd       r0, [r1,  256]
ldd       r2, [r3,  260]
