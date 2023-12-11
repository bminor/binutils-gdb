AARCH64 GAS  .*

   1              	# Testcase for a variety of ld st instructions
   2              		.text
   3              		.align  2
   4              		.global foo
   5              		.type   foo, %function
   5              	ginsn: SYM FUNC_BEGIN
   6              	foo:
   6              	ginsn: SYM foo
   7              	# ldstpair_indexed
   8 0000 FF7F8629 		stp     wzr, wzr, \[sp, 48\]!
   8              	ginsn: ADD %r31, 48, %r31
   9 0004 E00782A9 		stp     x0, x1, \[sp, 32\]!
   9              	ginsn: ADD %r31, 32, %r31
   9              	ginsn: STORE %r0, \[%r31\+0\]
   9              	ginsn: STORE %r1, \[%r31\+8\]
  10 0008 E827BC6D 		stp     d8, d9, \[sp, -64\]!
  10              	ginsn: ADD %r31, -64, %r31
  10              	ginsn: STORE %r72, \[%r31\+0\]
  10              	ginsn: STORE %r73, \[%r31\+8\]
  11 000c E827C46C 		ldp     d8,  d9,  \[sp\], 64
  11              	ginsn: LOAD \[%r31\+0\], %r72
  11              	ginsn: LOAD \[%r31\+8\], %r73
  11              	ginsn: ADD %r31, 64, %r31
  12              	# 32-bit FP regs
  13 0010 E51BB42D 		stp     s5, s6, \[sp, -96\]!
  13              	ginsn: ADD %r31, -96, %r31
  13              	ginsn: STORE %r69, \[%r31\+0\]
  13              	ginsn: STORE %r70, \[%r31\+4\]
  14 0014 E51BCC2C 		ldp     s5, s6, \[sp\], 96
  14              	ginsn: LOAD \[%r31\+0\], %r69
  14              	ginsn: LOAD \[%r31\+4\], %r70
  14              	ginsn: ADD %r31, 96, %r31
  15              	# 32-bit INT regs
  16 0018 E10BB029 		stp     w1, w2, \[sp, -128\]!
  16              	ginsn: ADD %r31, -128, %r31
  16              	ginsn: STORE %r1, \[%r31\+0\]
  16              	ginsn: STORE %r2, \[%r31\+4\]
  17 001c E10BD028 		ldp     w1, w2, \[sp\], 128
  17              	ginsn: LOAD \[%r31\+0\], %r1
  17              	ginsn: LOAD \[%r31\+4\], %r2
  17              	ginsn: ADD %r31, 128, %r31
  18              	# ldstpair_off
  19 0020 E00702AD 		stp     q0, q1, \[sp, 64\]
  19              	ginsn: STORE %r64, \[%r31\+64\]
  19              	ginsn: STORE %r65, \[%r31\+80\]
  20 0024 FF7F0629 		stp     wzr, wzr, \[sp, 48\]
  21 0028 1F7840AD 		ldp     q31, q30, \[x0\]
  22              	# ldst_imm9
  23 002c E78F4F38 		ldrb    w7, \[sp, 248\]!
  23              	ginsn: ADD %r31, 248, %r31
  23              	ginsn: LOAD \[%r31\+0\], %r7
  24 0030 FD0FC33C 		ldr     q29, \[sp, 48\]!
  24              	ginsn: ADD %r31, 48, %r31
  24              	ginsn: LOAD \[%r31\+0\], %r93
  25 0034 FF0F42B8 		ldr     wzr, \[sp, 32\]!
  25              	ginsn: ADD %r31, 32, %r31
  26 0038 E30742F8 		ldr     x3, \[sp\], 32
AARCH64 GAS  .*


  26              	ginsn: LOAD \[%r31\+0\], %r3
  26              	ginsn: ADD %r31, 32, %r31
  27              	# 32-bit ldr
  28 003c E10744BC 		ldr     s1, \[sp\], 64
  28              	ginsn: LOAD \[%r31\+0\], %r65
  28              	ginsn: ADD %r31, 64, %r31
  29              	#ldst_pos
  30 0040 FF3340B9 		ldr     wzr, \[sp, 48\]
  31 0044 FD1300F9 		str     x29, \[sp, 32\]
  31              	ginsn: STORE %r29, \[%r31\+32\]
  32 0048 FD1340F9 		ldr     x29, \[sp, 32\]
  32              	ginsn: LOAD \[%r31\+32\], %r29
  33 004c C0035FD6 		ret
  33              	ginsn: RET
  34              		.size   foo, .-foo
  34              	ginsn: SYM FUNC_END
