; 64-bit double stores

  std r4, [r1]                  ; std c, [b]
  std r4, [0x1338]              ; std c, [limm]         limm: small
  std r6, [0x7fffffff]          ; std c, [limm]         limm: big
  std 0x8000000, [r3]           ; std limm, [b]         limm: big
  std 0x1234, [0x1234]          ; std limm, [limm]      limm: equal

  std r0, [r1,255]              ; std c, [b, s9]        s9: biggest
  std r2, [r4,-256]             ; std c, [b, s9]        s9: smallest
  std r6, [0x7fffffff,12]       ; std c, [limm,s9]      limm: positive
  std 0x4321, [0x4321,13]       ; std limm, [limm,s9]   limm: equal
  std r8, [-1024]               ; std c, [limm]         limm: negative
  std         32, [r2, -236]    ; std limm, [b, s9]     limm: smallest positive
  std        -33, [r2, -246]    ; std limm, [b, s9]     limm: biggest negative
  std 0x12345678, [r2, -256]    ; std limm, [b, s9]     limm: positive
  std     -42000, [r1, 255]     ; std limm, [b, s9]     limm: negative

  ; Now, repetition of instructions above with suffices (*.a/aw/ab/as)
 
  std.a r0, [r1,255]
  std.a r2, [r4,-256]
  std.a 0x12345678, [r2, -256]
  std.a     -42000, [r1, 255]
  
  std.aw r0, [r1,255]
  std.aw r2, [r4,-256]
  std.aw 0x12345678, [r2, -256]
  std.aw     -42000, [r1, 255]
  
  std.ab r0, [r1,255]
  std.ab r2, [r4,-256]
  std.ab 0x12345678, [r2, -256]
  std.ab     -42000, [r1, 255]
  
  std.as r0, [r1,255]
  std.as r2, [r4,-256]
  std.as r4, [0x1338]
  std.as r6, [0x7fffffff]
  std.as r8, [-1024]
  std.as 0x12345678, [r2, -256]
  std.as     -42000, [r1, 255]
  std.as.di  0x4321, [0x4321, 15]

  ; std w6, ...
  std  31, [r1]                  ; std w6, [b, s9=0]  w6: biggest 
  std -32, [r1]                  ; std w6, [b, s9=0]  w6: smallest
  std -32, [r1,255]              ; std w6, [b, s9]    s9: biggest
  std   5, [r4,-256]             ; std w6, [b, s9]    s9: smallest
  std   0, [0x1338]              ; std w6, [limm]     limm: small positive
  std   1, [0x7fffffff]          ; std w6, [limm]     limm: positive
  std  -1, [-1024]               ; std w6, [limm]     limm: negative
  
  std.di     31, [r1]            ; std w6, [b, s9=0]  w6: biggest 
  std.as.di -32, [r1,255]        ; std w6, [b, s9]    s9: biggest
  std.aw.di   5, [r4,-256]       ; std w6, [b, s9]    s9: smallest
  std.ab.di -10, [r3,155]        ; std w6, [b, s9]
  std.aw     10, [r4,156]        ; std w6, [b, s9]
  std         0, [0x1338]        ; std w6, [limm]     limm: small positive
  std.di      1, [0x7fffffff]    ; std w6, [limm]     limm: positive
