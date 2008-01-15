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
.*:15: Error: .*
.*:16: Error: .*
.*:17: Error: .*
.*:18: Error: .*
.*:19: Error: .*
.*:20: Error: .*
.*:21: Error: .*
.*:22: Error: .*
.*:23: Error: .*
.*:24: Error: .*
.*:25: Error: .*
.*:26: Error: .*
.*:27: Error: .*
.*:28: Error: .*
.*:29: Error: .*
.*:30: Error: .*
.*:31: Error: .*
.*:32: Error: .*
.*:33: Error: .*
.*:34: Error: .*
.*:35: Error: .*
.*:36: Error: .*
.*:37: Error: .*
.*:38: Error: .*
.*:39: Error: .*
.*:40: Error: .*
.*:41: Error: .*
.*:42: Error: .*
.*:43: Error: .*
.*:44: Error: .*
.*:45: Error: .*
.*:46: Error: .*
.*:47: Error: .*
.*:48: Error: .*
.*:49: Error: .*
.*:50: Error: .*
.*:51: Error: .*
.*:52: Error: .*
.*:53: Error: .*
.*:54: Error: .*
.*:55: Error: .*
.*:56: Error: .*
.*:57: Error: .*
.*:58: Error: .*
.*:59: Error: .*
.*:60: Error: .*
.*:61: Error: .*
.*:62: Error: .*
.*:63: Error: .*
.*:64: Error: .*
.*:65: Error: .*
.*:66: Error: .*
.*:67: Error: .*
.*:68: Error: .*
.*:69: Error: .*
.*:70: Error: .*
.*:73: Error: .*
.*:74: Error: .*
.*:75: Error: .*
.*:76: Error: .*
.*:77: Error: .*
.*:78: Error: .*
.*:79: Error: .*
.*:80: Error: .*
.*:81: Error: .*
.*:82: Error: .*
.*:83: Error: .*
.*:84: Error: .*
.*:85: Error: .*
.*:86: Error: .*
.*:87: Error: .*
.*:88: Error: .*
.*:89: Error: .*
.*:90: Error: .*
.*:91: Error: .*
.*:92: Error: .*
.*:93: Error: .*
.*:94: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+\# All the following should be illegal for x86-64
[ 	]*3[ 	]+aaa		\# illegal
[ 	]*4[ 	]+aad		\# illegal
[ 	]*5[ 	]+aam		\# illegal
[ 	]*6[ 	]+aas		\# illegal
[ 	]*7[ 	]+arpl %ax,%ax	\# illegal
[ 	]*8[ 	]+bound %eax,\(%rax\) \# illegal
[ 	]*9[ 	]+calll \*%eax	\# 32-bit data size not allowed
[ 	]*10[ 	]+calll \*\(%ax\)	\# 32-bit data size not allowed
[ 	]*11[ 	]+calll \*\(%eax\)	\# 32-bit data size not allowed
[ 	]*12[ 	]+calll \*\(%r8\)	\# 32-bit data size not allowed
[ 	]*13[ 	]+calll \*\(%rax\)	\# 32-bit data size not allowed
[ 	]*14[ 	]+callq \*\(%ax\)	\# 32-bit data size not allowed
[ 	]*15[ 	]+callw \*\(%ax\)	\# no 16-bit addressing
[ 	]*16[ 	]+daa		\# illegal
[ 	]*17[ 	]+das		\# illegal
[ 	]*18[ 	]+enterl \$0,\$0	\# can't have 32-bit stack operands
[ 	]*19[ 	]+into		\# illegal
[ 	]*20[ 	]+foo:	jcxz foo	\# No prefix exists to select CX as a counter
[ 	]*21[ 	]+jmpl \*%eax	\# 32-bit data size not allowed
[ 	]*22[ 	]+jmpl \*\(%rax\)	\# 32-bit data size not allowed
[ 	]*23[ 	]+lcalll \$0,\$0	\# illegal
[ 	]*24[ 	]+lcallq \$0,\$0	\# illegal
[ 	]*25[ 	]+ldsl %eax,\(%rax\) \# illegal
[ 	]*26[ 	]+ldsq %rax,\(%rax\) \# illegal
[ 	]*27[ 	]+lesl %eax,\(%rax\) \# illegal
[ 	]*28[ 	]+lesq %rax,\(%rax\) \# illegal
[ 	]*29[ 	]+ljmpl \$0,\$0	\# illegal
[ 	]*30[ 	]+ljmpq \$0,\$0	\# illegal
[ 	]*31[ 	]+ljmpq \*\(%rax\)	\# 64-bit data size not allowed
[ 	]*32[ 	]+loopw foo	\# No prefix exists to select CX as a counter
[ 	]*33[ 	]+loopew foo	\# No prefix exists to select CX as a counter
[ 	]*34[ 	]+loopnew foo	\# No prefix exists to select CX as a counter
[ 	]*35[ 	]+loopnzw foo	\# No prefix exists to select CX as a counter
[ 	]*36[ 	]+loopzw foo	\# No prefix exists to select CX as a counter
[ 	]*37[ 	]+leavel		\# can't have 32-bit stack operands
[ 	]*38[ 	]+pop %ds		\# illegal
[ 	]*39[ 	]+pop %es		\# illegal
[ 	]*40[ 	]+pop %ss		\# illegal
[ 	]*41[ 	]+popa		\# illegal
[ 	]*42[ 	]+popl %eax	\# can't have 32-bit stack operands
[ 	]*43[ 	]+push %cs	\# illegal
[ 	]*44[ 	]+push %ds	\# illegal
[ 	]*45[ 	]+push %es	\# illegal
[ 	]*46[ 	]+push %ss	\# illegal
[ 	]*47[ 	]+pusha		\# illegal
[ 	]*48[ 	]+pushl %eax	\# can't have 32-bit stack operands
[ 	]*49[ 	]+pushfl		\# can't have 32-bit stack operands
[ 	]*50[ 	]+popfl		\# can't have 32-bit stack operands
[ 	]*51[ 	]+retl		\# can't have 32-bit stack operands
[ 	]*52[ 	]+insertq \$4,\$2,%xmm2,%ebx \# The last operand must be XMM register\.
[ 	]*53[ 	]+fnstsw %eax
[ 	]*54[ 	]+fnstsw %al
[ 	]*55[ 	]+fstsw %eax
[ 	]*56[ 	]+fstsw %al
[ 	]*57[ 	]+in \$8,%rax
GAS LISTING .*


[ 	]*58[ 	]+out %rax,\$8
[ 	]*59[ 	]+movsxb \(%rax\),%eax
[ 	]*60[ 	]+movsxb \(%rax\),%rax
[ 	]*61[ 	]+movsxw \(%rax\),%eax
[ 	]*62[ 	]+movsxw \(%rax\),%rax
[ 	]*63[ 	]+movsxl \(%rax\),%rax
[ 	]*64[ 	]+movzxb \(%rax\),%eax
[ 	]*65[ 	]+movzxb \(%rax\),%rax
[ 	]*66[ 	]+movzxw \(%rax\),%eax
[ 	]*67[ 	]+movzxw \(%rax\),%rax
[ 	]*68[ 	]+movzxl \(%rax\),%rax
[ 	]*69[ 	]+movnti %ax, \(%rax\)
[ 	]*70[ 	]+movntiw %ax, \(%rax\)
[ 	]*71[ 	]+
[ 	]*72[ 	]+\.intel_syntax noprefix
[ 	]*73[ 	]+cmpxchg16b dword ptr \[rax\] \# Must be oword
[ 	]*74[ 	]+movq xmm1, XMMWORD PTR \[rsp\]
[ 	]*75[ 	]+movq xmm1, DWORD PTR \[rsp\]
[ 	]*76[ 	]+movq xmm1, WORD PTR \[rsp\]
[ 	]*77[ 	]+movq xmm1, BYTE PTR \[rsp\]
[ 	]*78[ 	]+movq XMMWORD PTR \[rsp\],xmm1
[ 	]*79[ 	]+movq DWORD PTR \[rsp\],xmm1
[ 	]*80[ 	]+movq WORD PTR \[rsp\],xmm1
[ 	]*81[ 	]+movq BYTE PTR \[rsp\],xmm1
[ 	]*82[ 	]+fnstsw eax
[ 	]*83[ 	]+fnstsw al
[ 	]*84[ 	]+fstsw eax
[ 	]*85[ 	]+fstsw al
[ 	]*86[ 	]+in rax,8
[ 	]*87[ 	]+out 8,rax
[ 	]*88[ 	]+movsx ax, \[rax\]
[ 	]*89[ 	]+movsx eax, \[rax\]
[ 	]*90[ 	]+movsx rax, \[rax\]
[ 	]*91[ 	]+movzx ax, \[rax\]
[ 	]*92[ 	]+movzx eax, \[rax\]
[ 	]*93[ 	]+movzx rax, \[rax\]
[ 	]*94[ 	]+movnti word ptr \[rax\], ax
