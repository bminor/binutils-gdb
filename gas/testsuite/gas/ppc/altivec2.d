#as: -maltivec
#objdump: -dr -Maltivec
#name: Altivec ISA 2.07 instructions

.*: +file format elf(32)?(64)?-powerpc.*

Disassembly of section \.text:

0+00 <start>:
   0:	7c 60 e2 0e 	lvepxl  v3,0,r28
   4:	7e 64 92 0e 	lvepxl  v19,r4,r18
   8:	7f 60 9a 4e 	lvepx   v27,0,r19
   c:	7c 39 92 4e 	lvepx   v1,r25,r18
  10:	7f e0 da 0a 	lvexbx  v31,0,r27
  14:	7f 81 62 0a 	lvexbx  v28,r1,r12
  18:	7f e0 72 4a 	lvexhx  v31,0,r14
  1c:	7e 30 fa 4a 	lvexhx  v17,r16,r31
  20:	7e c0 ea 8a 	lvexwx  v22,0,r29
  24:	7e f9 2a 8a 	lvexwx  v23,r25,r5
  28:	7c 60 66 0a 	lvsm    v3,0,r12
  2c:	7f 7d 0e 0a 	lvsm    v27,r29,r1
  30:	7c e0 36 ca 	lvswxl  v7,0,r6
  34:	7c f0 46 ca 	lvswxl  v7,r16,r8
  38:	7d c0 94 ca 	lvswx   v14,0,r18
  3c:	7f 9c 84 ca 	lvswx   v28,r28,r16
  40:	7f 60 66 8a 	lvtlxl  v27,0,r12
  44:	7f 7c 06 8a 	lvtlxl  v27,r28,r0
  48:	7e e0 cc 8a 	lvtlx   v23,0,r25
  4c:	7c 39 74 8a 	lvtlx   v1,r25,r14
  50:	7e 80 c6 4a 	lvtrxl  v20,0,r24
  54:	7e dd c6 4a 	lvtrxl  v22,r29,r24
  58:	7f 00 44 4a 	lvtrx   v24,0,r8
  5c:	7d b7 e4 4a 	lvtrx   v13,r23,r28
  60:	7d 9c 60 dc 	mvidsplt v12,r28,r12
  64:	7d 5b 00 5c 	mviwsplt v10,r27,r0
  68:	7f 60 6e 0e 	stvepxl v27,0,r13
  6c:	7c 42 fe 0e 	stvepxl v2,r2,r31
  70:	7c 60 56 4e 	stvepx  v3,0,r10
  74:	7f 7c 06 4e 	stvepx  v27,r28,r0
  78:	7d a0 33 0a 	stvexbx v13,0,r6
  7c:	7d b9 1b 0a 	stvexbx v13,r25,r3
  80:	7e c0 0b 4a 	stvexhx v22,0,r1
  84:	7e 2e 53 4a 	stvexhx v17,r14,r10
  88:	7e a0 db 8a 	stvexwx v21,0,r27
  8c:	7f f2 0b 8a 	stvexwx v31,r18,r1
  90:	7f 40 6f 8a 	stvflxl v26,0,r13
  94:	7e cd af 8a 	stvflxl v22,r13,r21
  98:	7c a0 4d 8a 	stvflx  v5,0,r9
  9c:	7e b8 0d 8a 	stvflx  v21,r24,r1
  a0:	7d a0 57 4a 	stvfrxl v13,0,r10
  a4:	7d b1 cf 4a 	stvfrxl v13,r17,r25
  a8:	7e 20 55 4a 	stvfrx  v17,0,r10
  ac:	7d 0c fd 4a 	stvfrx  v8,r12,r31
  b0:	7e 40 ef ca 	stvswxl v18,0,r29
  b4:	7f 4e 47 ca 	stvswxl v26,r14,r8
  b8:	7c 00 7d ca 	stvswx  v0,0,r15
  bc:	7d b7 3d ca 	stvswx  v13,r23,r7
  c0:	10 d1 84 03 	vabsdub v6,v17,v16
  c4:	12 b2 24 43 	vabsduh v21,v18,v4
  c8:	13 34 4c 83 	vabsduw v25,v20,v9
