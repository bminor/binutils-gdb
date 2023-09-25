; 64-bit stores

  stl r4, [r1]                    ; stl c,    [b]
  stl r3, [0x1338]                ; stl c,    [ximm]      ximm: default
  stl r5, [0x7fffffff]            ; stl c,    [ximm]      ximm: big
  stl r2, [0x80000000]            ; stl c,    [limm]      limm: positive
  stl r7, [-1234]                 ; stl c,    [ximm]      ximm: negative
  stl 0x80000000, [r3]            ; stl limm, [b]         limm: smallest
  stl 0x7fffffff, [r3]            ; stl ximm, [b]         ximm: biggest

  stl r1, [r1, 255]               ; stl c,    [b, s9]     s9: biggest
  stl r2, [r4,-256]               ; stl c,    [b, s9]     s9: smallest
  stl     -42000, [r1,  255]      ; stl ximm, [b, s9]     ximm: negative
  stl 0x82345678, [r2, -256]      ; stl limm, [b, s9]     limm: positive

  ; Now, repetition of instructions above with suffices (*.a/aw/ab/as)

  stl.a r0, [r1,255]
  stl.a r1, [r4,-256]
  stl.a 0x82345678, [r2, -256]
  stl.a     -42000, [r1, 255]

  stl.aw r0, [r1,255]
  stl.aw r2, [r4,-256]
  stl.aw 0x82345678, [r2, -256]
  stl.aw     -42000, [r1, 255]

  stl.ab r0, [r1,255]
  stl.ab r3, [r4,-256]
  stl.ab 0x82345678, [r2, -256]
  stl.ab     -42000, [r1, 255]

  stl.as r0, [r1,255]
  stl.as r1, [r4,-256]
  stl.as 0x82345678, [r2, -256]
  stl.as     -42000, [r1, 255]
  stl.as 0x82345678, [r2, -256]

  ; stl w6, ...
  stl     31, [r1]            ; stl w6, [b, s9=0]  w6: biggest
  stl    -32, [r1]            ; stl w6, [b, s9=0]  w6: smallest
  stl    -32, [r1,255]        ; stl w6, [b, s9]    s9: biggest
  stl      5, [r4,-256]       ; stl w6, [b, s9]    s9: smallest
  stl.as -32, [r1,255]        ; stl w6, [b, s9]    s9: biggest
  stl.aw   5, [r4,-256]       ; stl w6, [b, s9]    s9: smallest
  stl.ab -10, [r3,155]        ; stl w6, [b, s9]
  stl.aw  10, [r4,156]        ; stl w6, [b, s9]
  stl      0, [0x1338]        ; stl w6, [ximm]     ximm: small positive
  stl      1, [0x8fffffff]    ; stl w6, [limm]     limm: positive
  stl     -1, [-1024]         ; stl w6, [ximm]     ximm: negative
