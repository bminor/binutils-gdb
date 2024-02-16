.*: Assembler messages:
.*:24: Error: untraceable control flow for func 'foo'
GAS LISTING .*

   1              	# Testcase with a variety of "change of flow instructions"
   2              	#
   3              	# This test does not have much going on wrt synthesis of CFI;
   4              	# it just aims to ensure x8_64 -> ginsn decoding behaves
   5              	# gracefully for these "change of flow instructions"
   6              		.text
   7              		.globl  foo
   8              		.type   foo, @function
   8              	ginsn: SYM FUNC_BEGIN
   9              	foo:
   9              	ginsn: SYM foo
  10 \?\?\?\? 4801D0   		addq    %rdx, %rax
  10              	ginsn: ADD %r1, %r0, %r0
  11 \?\?\?\? E200     		loop    foo
  11              	ginsn: JCC 
  12 \?\?\?\? 3EFFE0   		notrack jmp     \*%rax
  12              	ginsn: JMP %r0, 
  13 \?\?\?\? 41FFD0   		call    \*%r8
  13              	ginsn: CALL
  14 \?\?\?\? FF14C500 		call    \*cost_arr\(,%rax,8\)
  14      000000
  14              	ginsn: CALL
  15 \?\?\?\? FF142500 		call    \*symbol\+1
  15      000000
  15              	ginsn: CALL
  16 \?\?\?\? 67E313   		jecxz   .L179
  16              	ginsn: JCC 
  17 \?\?\?\? FF6730   		jmp     \*48\(%rdi\)
  17              	ginsn: JMP %r5, 
  18 \?\?\?\? FF24C500 		jmp     \*cost_arr\(,%rax,8\)
  18      000000
  18              	ginsn: JMP %r0, 
  19 \?\?\?\? FF242500 		jmp     \*symbol\+1
  19      000000
  19              	ginsn: JMP %r4, 
  20 \?\?\?\? 7000     		jo      .L179
  20              	ginsn: JCC 
  21              	.L179:
  21              	ginsn: SYM .L179
  22 \?\?\?\? C3       		ret
  22              	ginsn: RET
  23              	.LFE0:
  23              	ginsn: SYM .LFE0
  24              		.size   foo, .-foo
  24              	ginsn: SYM FUNC_END
