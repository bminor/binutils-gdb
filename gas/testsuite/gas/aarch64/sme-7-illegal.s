/* Scalable Matrix Extension (SME).  */

/* Load vector to ZA array.  */
ldr za[w11, 0], [x0]
ldr za[w12, 1], [sp, x0]
ldr za[w12, 0], [sp, #1, mul vl]
ldr za[w13, 9], [x17, #19, mul vl]
ldr za[w13, 21], [x17, #21, mul vl]
ldr za[w15, 32], [x17, #15, mul vl]
ldr za[w16, 15], [sp, #15, mul vl]
ldr za[w12, 0], [x0, #0, mul #1]
ldr za[w13, 0], [sp, #0, mul #2]
ldr za[w14, 9], [x17, #9, mul #3]
ldr za[w15, 15], [sp, #15, mul #4]

/* Store vector from ZA array.  */
str za[w11, 0], [x0]
str za[w12, 1], [sp, x0]
str za[w12, 0], [sp, #1, mul vl]
str za[w13, 9], [x17, #19, mul vl]
str za[w13, 21], [x17, #21, mul vl]
str za[w15, 32], [x17, #15, mul vl]
str za[w16, 15], [sp, #15, mul vl]
str za[w12, 0], [x0, #0, mul #1]
str za[w13, 0], [sp, #0, mul #2]
str za[w14, 9], [x17, #9, mul #3]
str za[w15, 15], [sp, #15, mul #4]

/* Operands indexes are tied.  */
ldr za[w13, 13], [x17, #23, mul vl]
str za[w13, 13], [x17, #23, mul vl]
ldr za[w13, 23], [x17, #13, mul vl]
str za[w13, 23], [x17, #13, mul vl]
ldr za[w13, 16], [x17, #16, mul vl]
str za[w13, 16], [x17, #16, mul vl]
ldr za[w13, -1], [x17, #1, mul vl]
str za[w13, -1], [x17, #1, mul vl]
ldr za[w13, 1], [x17, #-1, mul vl]
str za[w13, 1], [x17, #-1, mul vl]
