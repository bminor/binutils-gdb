AARCH64 GAS  .*

   1              	# Testcase for a variety of ld st instructions.
   2              	# stg ops must generate the sp update arithmetic insn, when applicable
   3              	# due to writeback.
   4              		.text
   5              		.align  2
   6              		.global foo
   7              		.type   foo, %function
   7              	ginsn: SYM FUNC_BEGIN
   8              	foo:
   8              	ginsn: SYM foo
   9              	# ldstpair_indexed
  10 0000 FF7F8629 		stp     wzr, wzr, \[sp, 48\]!
  10              	ginsn: ADD %r31, 48, %r31
  11 0004 E00782A9 		stp     x0, x1, \[sp, 32\]!
  11              	ginsn: ADD %r31, 32, %r31
  11              	ginsn: STORE %r0, \[%r31\+0\]
  11              	ginsn: STORE %r1, \[%r31\+8\]
  12 0008 E827BC6D 		stp     d8, d9, \[sp, -64\]!
  12              	ginsn: ADD %r31, -64, %r31
  12              	ginsn: STORE %r72, \[%r31\+0\]
  12              	ginsn: STORE %r73, \[%r31\+8\]
  13 000c E827C46C 		ldp     d8, d9, \[sp\], 64
  13              	ginsn: LOAD \[%r31\+0\], %r72
  13              	ginsn: LOAD \[%r31\+8\], %r73
  13              	ginsn: ADD %r31, 64, %r31
  14              	# 32-bit FP regs
  15 0010 E51BB42D 		stp     s5, s6, \[sp, -96\]!
  15              	ginsn: ADD %r31, -96, %r31
  16 0014 E51BCC2C 		ldp     s5, s6, \[sp\], 96
  16              	ginsn: ADD %r31, 96, %r31
  17              	# 32-bit INT regs
  18 0018 E10BB029 		stp     w1, w2, \[sp, -128\]!
  18              	ginsn: ADD %r31, -128, %r31
  19 001c E10BD028 		ldp     w1, w2, \[sp\], 128
  19              	ginsn: ADD %r31, 128, %r31
  20              	# ldstpair_off
  21 0020 E00702AD 		stp     q0, q1, \[sp, 64\]
  21              	ginsn: STORE %r64, \[%r31\+64\]
  21              	ginsn: STORE %r65, \[%r31\+80\]
  22 0024 FF7F0629 		stp     wzr, wzr, \[sp, 48\]
  23 0028 1F7840AD 		ldp     q31, q30, \[x0\]
  23              	ginsn: LOAD \[%r0\+0\], %r95
  23              	ginsn: LOAD \[%r0\+16\], %r94
  24              	# ldst_imm9
  25 002c E78F4F38 		ldrb    w7, \[sp, 248\]!
  25              	ginsn: ADD %r31, 248, %r31
  26 0030 FD0FC33C 		ldr     q29, \[sp, 48\]!
  26              	ginsn: ADD %r31, 48, %r31
  26              	ginsn: LOAD \[%r31\+0\], %r93
  27 0034 FF0F42B8 		ldr     wzr, \[sp, 32\]!
  27              	ginsn: ADD %r31, 32, %r31
  28 0038 E30742F8 		ldr     x3, \[sp\], 32
  28              	ginsn: LOAD \[%r31\+0\], %r3
  28              	ginsn: ADD %r31, 32, %r31
  29              	# 32-bit ldr
  30 003c E10744BC 		ldr     s1, \[sp\], 64
  30              	ginsn: ADD %r31, 64, %r31
AARCH64 GAS  .*


  31              	# ldst_pos
  32 0040 FF3340B9 		ldr     wzr, \[sp, 48\]
  33 0044 FD1300F9 		str     x29, \[sp, 32\]
  33              	ginsn: STORE %r29, \[%r31\+32\]
  34 0048 FD1340F9 		ldr     x29, \[sp, 32\]
  34              	ginsn: LOAD \[%r31\+32\], %r29
  35              	# store tag
  36 004c FF2F20D9 		stg     sp, \[sp, 32\]!
  36              	ginsn: ADD %r31, 32, %r31
  37              	# ldpsw
  38 0050 E83F6069 		ldpsw   x8, x15, \[sp, -256\]
  39 0054 C0035FD6 		ret
  39              	ginsn: RET
  40              		.size   foo, .-foo
  40              	ginsn: SYM FUNC_END
