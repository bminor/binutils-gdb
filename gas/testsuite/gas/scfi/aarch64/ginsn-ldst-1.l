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
  11 000c E827C46C 		ldp     d8,  d9,  \[sp\], 64
  11              	ginsn: ADD %r31, 64, %r31
  12              	# ldstpair_off
  13 0010 E00702AD 		stp     q0, q1, \[sp, 64\]
  14 0014 FF7F0629 		stp     wzr, wzr, \[sp, 48\]
  15 0018 1F7840AD 		ldp     q31, q30, \[x0\]
  16              	# ldst_imm9
  17 001c E78F4F38 		ldrb    w7, \[sp, 248\]!
  17              	ginsn: ADD %r31, 248, %r31
  17              	ginsn: LOAD \[%r31\+0\], %r7
  18 0020 FD0FC33C 		ldr     q29, \[sp, 48\]!
  18              	ginsn: ADD %r31, 48, %r31
  19 0024 FF0F42B8 		ldr     wzr, \[sp, 32\]!
  19              	ginsn: ADD %r31, 32, %r31
  20 0028 E30742F8 		ldr     x3, \[sp\], 32
  20              	ginsn: LOAD \[%r31\+0\], %r3
  20              	ginsn: ADD %r31, 32, %r31
  21              	#ldst_pos
  22 002c FF3340B9 		ldr     wzr, \[sp, 48\]
  23 0030 FD1300F9 		str     x29, \[sp, 32\]
  23              	ginsn: STORE %r29, \[%r31\+32\]
  24 0034 FD1340F9 		ldr     x29, \[sp, 32\]
  24              	ginsn: LOAD \[%r31\+32\], %r29
  25 0038 C0035FD6 		ret
  25              	ginsn: RET
  26              		.size   foo, .-foo
  26              	ginsn: SYM FUNC_END
