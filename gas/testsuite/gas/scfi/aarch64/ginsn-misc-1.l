.*: Assembler messages:
.*:9: Error: SCFI: unsupported stack manipulation pattern
.*:11: Error: SCFI: forward pass failed for func 'foo'

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
  10 \?\?\?\? C0035FD6 		ret
  10              	ginsn: RET
  11              		.size   foo,.-foo
  11              	ginsn: SYM FUNC_END
