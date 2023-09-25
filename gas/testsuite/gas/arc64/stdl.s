; 128-bit double stores

  stdl r4, [r1]                    ; stdl c,    [b]
  stdl r2, [0x1338]                ; stdl c,    [ximm]      ximm: default
  stdl r6, [0x7fffffff]            ; stdl c,    [ximm]      ximm: big
  stdl r2, [0x80000000]            ; stdl c,    [limm]      limm: positive
  stdl r0, [-1234]                 ; stdl c,    [ximm]      ximm: negative
  stdl 0x80000000, [r3]            ; stdl limm, [b]         limm: smallest
  stdl 0x7fffffff, [r3]            ; stdl ximm, [b]         ximm: biggest

  stdl r0, [r1, 255]               ; stdl c,    [b, s9]     s9: biggest
  stdl r2, [r4,-256]               ; stdl c,    [b, s9]     s9: smallest
  stdl     -42000, [r1,  255]      ; stdl ximm, [b, s9]     ximm: negative
  stdl 0x82345678, [r2, -256]      ; stdl limm, [b, s9]     limm: positive

  ; Now, repetition of instructions above with suffices (*.a/aw/ab/as)

  stdl.a r0, [r1,255]
  stdl.a r4, [r4,-256]
  stdl.a 0x82345678, [r2, -256]
  stdl.a     -42000, [r1, 255]

  stdl.aw r0, [r1,255]
  stdl.aw r2, [r4,-256]
  stdl.aw 0x82345678, [r2, -256]
  stdl.aw     -42000, [r1, 255]

  stdl.ab r0, [r1,255]
  stdl.ab r6, [r4,-256]
  stdl.ab 0x82345678, [r2, -256]
  stdl.ab     -42000, [r1, 255]

  stdl.as r0, [r1,255]
  stdl.as r4, [r4,-256]
  stdl.as 0x82345678, [r2, -256]
  stdl.as     -42000, [r1, 255]
  stdl.as 0x82345678, [r2, -256]
