.*: Assembler messages:
.*:9: Error: SCFI: unsupported stack manipulation pattern
.*:15: Error: SCFI: forward pass failed for func 'foo'

AARCH64 GAS  .*


   1              	# Testcase for a variety of misc instructions
   2              	# Ensure graceful handling, irrespective of ginsn generation
   3              	symbol:
   4 \?\?\?\? 7700     	        .string "w"
   5              	
   6              		.type   foo, %function
   6              	ginsn: SYM FUNC_BEGIN
   7              	foo:
   7              	ginsn: SYM foo
   8 \?\?\?\? 00001024 		mov     x16, 4384
   8      82D2
   8              	ginsn: MOV 4384, %r16
   9 \?\?\?\? FF63308B 		add     sp, sp, x16
   9              	ginsn: ADD %r31, %r16, %r31
  10 \?\?\?\? 00000090 	        adrp    x0, symbol
  11 \?\?\?\? 1F000091 		add     sp, x0, :lo12:symbol
  11              	ginsn: OTH 0, 0, %r31
  12 \?\?\?\? FF0F00D1 		sub     sp, sp, 3
  12              	ginsn: SUB %r31, 3, %r31
  13 \?\?\?\? FF0F0091 		add     sp, sp, 3
  13              	ginsn: ADD %r31, 3, %r31
  14 \?\?\?\? C0035FD6 		ret
  14              	ginsn: RET
  15              		.size   foo,.-foo
  15              	ginsn: SYM FUNC_END
