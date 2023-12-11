.*: Assembler messages:
.*:9: Error: SCFI: unsupported stack manipulation pattern
.*:13: Error: SCFI: forward pass failed for func 'foo'

AARCH64 GAS  .*


   1              	# Testcase for a variety of misc instructions
   2              	# Ensure graceful handling, irrespective of ginsn generation
   3              	symbol:
   4 \?\?\?\? 7700     		.string "w"
   5              	
   6              		.type   foo, %function
   6              	ginsn: SYM FUNC_BEGIN
   7              	foo:
   7              	ginsn: SYM foo
   8 \?\?\?\? 00000000 		adrp    x0, symbol
   8      0090
   9 \?\?\?\? 1F000091 		add     sp, x0, :lo12:symbol
   9              	ginsn: OTH 0, 0, %r31
  10 \?\?\?\? 2000A0F2 		movk    x0, 0x1, lsl 16
  11 \?\?\?\? E10380F9 		prfm    PLDL1STRM, \[sp\]
  11              	ginsn: LOAD \[%r31\+0\], %r1
  12 \?\?\?\? C0035FD6 		ret
  12              	ginsn: RET
  13              		.size   foo,.-foo
  13              	ginsn: SYM FUNC_END
