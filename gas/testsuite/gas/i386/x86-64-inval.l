.*: Assembler messages:
.*:3: Error: .*
.*:4: Error: .*
.*:5: Error: .*
.*:6: Error: .*
.*:7: Error: .*
.*:8: Error: .*
.*:9: Error: .*
.*:10: Error: .*
.*:11: Error: .*
.*:12: Error: .*
.*:13: Error: .*
.*:14: Error: .*
+.*:15: Error: .*
+.*:16: Error: .*
+.*:17: Error: .*
+.*:18: Error: .*
+.*:19: Error: .*
+.*:20: Error: .*
+.*:21: Error: .*
+.*:22: Error: .*
+.*:23: Error: .*
+.*:24: Error: .*
+.*:25: Error: .*
+.*:26: Error: .*
+.*:27: Error: .*
+.*:28: Error: .*
+.*:29: Error: .*
+.*:30: Error: .*
+.*:31: Error: .*
+.*:32: Error: .*
+.*:33: Error: .*
+.*:34: Error: .*
+.*:35: Error: .*
+.*:36: Error: .*
+.*:37: Error: .*
+.*:38: Error: .*
+.*:39: Error: .*
+.*:40: Error: .*
+.*:41: Error: .*
+.*:42: Error: .*
+.*:43: Error: .*
+.*:44: Error: .*
+.*:45: Error: .*
+.*:46: Error: .*
+.*:47: Error: .*
+.*:48: Error: .*
+.*:49: Error: .*
+.*:50: Error: .*
+.*:51: Error: .*
+.*:52: Error: .*
+.*:53: Error: .*
GAS LISTING .*


   1 [ 	]*.text
   2 [ 	]*# All the following should be illegal for x86-64
   3 [ 	]*aaa		# illegal
   4 [ 	]*aad		# illegal
   5 [ 	]*aam		# illegal
   6 [ 	]*aas		# illegal
   7 [ 	]*arpl %ax,%ax	# illegal
   8 [ 	]*bound %eax,\(%rax\) # illegal
   9 [ 	]*calll \*%eax	# 32-bit data size not allowed
  10 [ 	]*calll \*\(%ax\)	# 32-bit data size not allowed
  11 [ 	]*calll \*\(%eax\)	# 32-bit data size not allowed
  12 [ 	]*calll \*\(%r8\)	# 32-bit data size not allowed
  13 [ 	]*calll \*\(%rax\)	# 32-bit data size not allowed
  14 [ 	]*callq \*\(%ax\)	# 32-bit data size not allowed
  15 [ 	]*callw \*\(%ax\)	# no 16-bit addressing
  16 [ 	]*daa		# illegal
  17 [ 	]*das		# illegal
  18 [ 	]*enterl \$0,\$0	# can't have 32-bit stack operands
  19 [ 	]*into		# illegal
  20 [ 	]*foo:[ 	]*jcxz foo	# No prefix exists to select CX as a counter
  21 [ 	]*jmpl \*%eax	# 32-bit data size not allowed
  22 [ 	]*jmpl \*\(%rax\)	# 32-bit data size not allowed
  23 [ 	]*lahf		# illegal
  24 [ 	]*lcalll \$0,\$0	# illegal
  25 [ 	]*lcallq \$0,\$0	# illegal
  26 [ 	]*ldsl %eax,\(%rax\) # illegal
  27 [ 	]*ldsq %rax,\(%rax\) # illegal
  28 [ 	]*lesl %eax,\(%rax\) # illegal
  29 [ 	]*lesq %rax,\(%rax\) # illegal
  30 [ 	]*ljmpl \$0,\$0	# illegal
  31 [ 	]*ljmpq \$0,\$0	# illegal
  32 [ 	]*ljmpq \*\(%rax\)	# 64-bit data size not allowed
  33 [ 	]*loopw foo	# No prefix exists to select CX as a counter
  34 [ 	]*loopew foo	# No prefix exists to select CX as a counter
  35 [ 	]*loopnew foo	# No prefix exists to select CX as a counter
  36 [ 	]*loopnzw foo	# No prefix exists to select CX as a counter
  37 [ 	]*loopzw foo	# No prefix exists to select CX as a counter
  38 [ 	]*leavel		# can't have 32-bit stack operands
  39 [ 	]*pop %ds		# illegal
  40 [ 	]*pop %es		# illegal
  41 [ 	]*pop %ss		# illegal
  42 [ 	]*popa		# illegal
  43 [ 	]*popl %eax	# can't have 32-bit stack operands
  44 [ 	]*push %cs	# illegal
  45 [ 	]*push %ds	# illegal
  46 [ 	]*push %es	# illegal
  47 [ 	]*push %ss	# illegal
  48 [ 	]*pusha		# illegal
  49 [ 	]*pushl %eax	# can't have 32-bit stack operands
  50 [ 	]*pushfl		# can't have 32-bit stack operands
  51 [ 	]*popfl		# can't have 32-bit stack operands
  52 [ 	]*retl		# can't have 32-bit stack operands
  53 [ 	]*sahf		# illegal
