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
   8 0000 FFFF8629 		stp     wzr, wzr, \[sp, 52\]!
   8              	ginsn: ADD %r31, 52, %r31
   8              	ginsn: STORE %r1, \[%r31\+0\]
   8              	ginsn: STORE %r1, \[%r31\+4\]
   9 0004 E00782A9 		stp     x0, x1, \[sp, 32\]!
   9              	ginsn: ADD %r31, 32, %r31
   9              	ginsn: STORE %r0, \[%r31\+0\]
   9              	ginsn: STORE %r1, \[%r31\+8\]
  10 0008 E827BC6D 		stp     d8, d9, \[sp, -64\]!
  10              	ginsn: ADD %r31, -64, %r31
  11              	# Following is skipped from ginsn generation
  12 000c E00702AD 		stp     q0, q1, \[sp, 64\]
  13              	# ldstpair_off
  14 0010 FFFF0629 		stp     wzr, wzr, \[sp, 52\]
  14              	ginsn: STORE %r1, \[%r31\+52\]
  14              	ginsn: STORE %r1, \[%r31\+56\]
  15 0014 1F7840AD 		ldp     q31, q30, \[x0\]
  16 0018 E827C46C 		ldp     d8,  d9,  \[sp\], 64
  16              	ginsn: ADD %r31, 64, %r31
  17              	# ldst_imm9
  18 001c E7FF4F38 		ldrb    w7, \[sp, 255\]!
  18              	ginsn: ADD %r31, 255, %r31
  18              	ginsn: LOAD \[%r31\+0\], %r7
  19 0020 FDCFC23C 		ldr     q29, \[sp, 44\]!
  19              	ginsn: ADD %r31, 44, %r31
  20 0024 C0035FD6 		ret
  20              	ginsn: RET
  21              		.size   foo, .-foo
  21              	ginsn: SYM FUNC_END
