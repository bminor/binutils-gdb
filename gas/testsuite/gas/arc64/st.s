; common stores

  st r4, [r1]                  ; st c, [b]
  st r4, [0x1338]              ; st c, [limm]         limm: small
  st r6, [0x7fffffff]          ; st c, [limm]         limm: big
  st 0x8000000, [r3]           ; st limm, [b]         limm: big
  st 0x1234, [0x1234]          ; st limm, [limm]      limm: equal

  st r0, [r1,255]              ; st c, [b, s9]        s9: biggest
  st r2, [r4,-256]             ; st c, [b, s9]        s9: smallest
  st r6, [0x7fffffff,12]       ; st c, [limm,s9]      limm: positive
  st 0x4321, [0x4321,13]       ; st limm, [limm,s9]   limm: equal
  st r8, [-1024]               ; st c, [limm]         limm: negative
  st         32, [r2, -236]    ; st limm, [b, s9]     limm: smallest positive
  st        -33, [r2, -246]    ; st limm, [b, s9]     limm: biggest negative
  st 0x12345678, [r2, -256]    ; st limm, [b, s9]     limm: positive
  st     -42000, [r1, 255]     ; st limm, [b, s9]     limm: negative

  ; Now, repetition of instructions above with suffices (*.a/aw/ab/as)
 
  st.a r0, [r1,255]
  st.a r2, [r4,-256]
  st.a 0x12345678, [r2, -256]
  st.a     -42000, [r1, 255]
  
  st.aw r0, [r1,255]
  st.aw r2, [r4,-256]
  st.aw 0x12345678, [r2, -256]
  st.aw     -42000, [r1, 255]
  
  st.ab r0, [r1,255]
  st.ab r2, [r4,-256]
  st.ab 0x12345678, [r2, -256]
  st.ab     -42000, [r1, 255]
  
  st.as r0, [r1,255]
  st.as r2, [r4,-256]
  st    r4, [0x1338]
  st    r6, [0x7fffffff]
  st    r8, [-1024]
  st.as 0x12345678, [r2, -256]
  st.as     -42000, [r1, 255]

  ; st w6, ...
  st  31, [r1]                  ; st w6, [b, s9=0]  w6: biggest 
  st -32, [r1]                  ; st w6, [b, s9=0]  w6: smallest
  st -32, [r1,255]              ; st w6, [b, s9]    s9: biggest
  st   5, [r4,-256]             ; st w6, [b, s9]    s9: smallest
  st   0, [0x1338]              ; st w6, [limm]     limm: small positive
  st   1, [0x7fffffff]          ; st w6, [limm]     limm: positive
  st  -1, [-1024]               ; st w6, [limm]     limm: negative
  
  st.di     31, [r1]            ; st w6, [b, s9=0]  w6: biggest 
  st.as.di -32, [r1,255]        ; st w6, [b, s9]    s9: biggest
  st.aw.di   5, [r4,-256]       ; st w6, [b, s9]    s9: smallest
  st.ab.di -10, [r3,155]        ; st w6, [b, s9]
  st.aw     10, [r4,156]        ; st w6, [b, s9]
  st         0, [0x1338]        ; st w6, [limm]     limm: small positive
  st.di      1, [0x7fffffff]    ; st w6, [limm]     limm: positive

	;; sth w6 ...
	sth.di	4,[r4]
	sth.di.as	5,[r5,9]
	st	0,[r6]
	stb	1,[r1,0]
	st.aw	0,[r6,2]
	stb.ab	1,[r1,1]
	sth	4,[@symb]
