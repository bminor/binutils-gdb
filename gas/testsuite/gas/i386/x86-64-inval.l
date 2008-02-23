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
.*:71: Error: .*
.*:72: Error: .*
.*:73: Error: .*
.*:74: Error: .*
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
.*:95: Error: .*
.*:96: Error: .*
.*:97: Error: .*
.*:98: Error: .*
.*:99: Error: .*
.*:100: Error: .*
.*:101: Error: .*
.*:102: Error: .*
.*:103: Error: .*
.*:104: Error: .*
.*:105: Error: .*
.*:106: Error: .*
.*:107: Error: .*
.*:108: Error: .*
.*:109: Error: .*
.*:110: Error: .*
.*:111: Error: .*
.*:112: Error: .*
.*:113: Error: .*
.*:114: Error: .*
.*:115: Error: .*
.*:116: Error: .*
.*:117: Error: .*
.*:118: Error: .*
.*:119: Error: .*
.*:120: Error: .*
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
[ 	]*14[ 	]+callw \*%ax	\# 16-bit data size not allowed
[ 	]*15[ 	]+callw \*\(%ax\)	\# 16-bit data size not allowed
[ 	]*16[ 	]+callw \*\(%eax\)	\# 16-bit data size not allowed
[ 	]*17[ 	]+callw \*\(%r8\)	\# 16-bit data size not allowed
[ 	]*18[ 	]+callw \*\(%rax\)	\# 16-bit data size not allowed
[ 	]*19[ 	]+callq \*\(%ax\)	\# no 16-bit addressing
[ 	]*20[ 	]+daa		\# illegal
[ 	]*21[ 	]+das		\# illegal
[ 	]*22[ 	]+enterl \$0,\$0	\# can't have 32-bit stack operands
[ 	]*23[ 	]+into		\# illegal
[ 	]*24[ 	]+foo:	jcxz foo	\# No prefix exists to select CX as a counter
[ 	]*25[ 	]+jmpl \*%eax	\# 32-bit data size not allowed
[ 	]*26[ 	]+jmpl \*\(%ax\)	\# 32-bit data size not allowed
[ 	]*27[ 	]+jmpl \*\(%eax\)	\# 32-bit data size not allowed
[ 	]*28[ 	]+jmpl \*\(%r8\)	\# 32-bit data size not allowed
[ 	]*29[ 	]+jmpl \*\(%rax\)	\# 32-bit data size not allowed
[ 	]*30[ 	]+jmpw \*%ax	\# 16-bit data size not allowed
[ 	]*31[ 	]+jmpw \*\(%ax\)	\# 16-bit data size not allowed
[ 	]*32[ 	]+jmpw \*\(%eax\)	\# 16-bit data size not allowed
[ 	]*33[ 	]+jmpw \*\(%r8\)	\# 16-bit data size not allowed
[ 	]*34[ 	]+jmpw \*\(%rax\)	\# 16-bit data size not allowed
[ 	]*35[ 	]+jmpq \*\(%ax\)	\# no 16-bit addressing
[ 	]*36[ 	]+lcalll \$0,\$0	\# illegal
[ 	]*37[ 	]+lcallq \$0,\$0	\# illegal
[ 	]*38[ 	]+ldsl %eax,\(%rax\) \# illegal
[ 	]*39[ 	]+ldsq %rax,\(%rax\) \# illegal
[ 	]*40[ 	]+lesl %eax,\(%rax\) \# illegal
[ 	]*41[ 	]+lesq %rax,\(%rax\) \# illegal
[ 	]*42[ 	]+ljmpl \$0,\$0	\# illegal
[ 	]*43[ 	]+ljmpq \$0,\$0	\# illegal
[ 	]*44[ 	]+ljmpq \*\(%rax\)	\# 64-bit data size not allowed
[ 	]*45[ 	]+loopw foo	\# No prefix exists to select CX as a counter
[ 	]*46[ 	]+loopew foo	\# No prefix exists to select CX as a counter
[ 	]*47[ 	]+loopnew foo	\# No prefix exists to select CX as a counter
[ 	]*48[ 	]+loopnzw foo	\# No prefix exists to select CX as a counter
[ 	]*49[ 	]+loopzw foo	\# No prefix exists to select CX as a counter
[ 	]*50[ 	]+leavel		\# can't have 32-bit stack operands
[ 	]*51[ 	]+pop %ds		\# illegal
[ 	]*52[ 	]+pop %es		\# illegal
[ 	]*53[ 	]+pop %ss		\# illegal
[ 	]*54[ 	]+popa		\# illegal
[ 	]*55[ 	]+popl %eax	\# can't have 32-bit stack operands
[ 	]*56[ 	]+push %cs	\# illegal
[ 	]*57[ 	]+push %ds	\# illegal
GAS LISTING .*


[ 	]*58[ 	]+push %es	\# illegal
[ 	]*59[ 	]+push %ss	\# illegal
[ 	]*60[ 	]+pusha		\# illegal
[ 	]*61[ 	]+pushl %eax	\# can't have 32-bit stack operands
[ 	]*62[ 	]+pushfl		\# can't have 32-bit stack operands
[ 	]*63[ 	]+popfl		\# can't have 32-bit stack operands
[ 	]*64[ 	]+retl		\# can't have 32-bit stack operands
[ 	]*65[ 	]+insertq \$4,\$2,%xmm2,%ebx \# The last operand must be XMM register\.
[ 	]*66[ 	]+fnstsw %eax
[ 	]*67[ 	]+fnstsw %al
[ 	]*68[ 	]+fstsw %eax
[ 	]*69[ 	]+fstsw %al
[ 	]*70[ 	]+in \$8,%rax
[ 	]*71[ 	]+out %rax,\$8
[ 	]*72[ 	]+movzxl \(%rax\),%rax
[ 	]*73[ 	]+movnti %ax, \(%rax\)
[ 	]*74[ 	]+movntiw %ax, \(%rax\)
[ 	]*75[ 	]+
[ 	]*76[ 	]+\.intel_syntax noprefix
[ 	]*77[ 	]+cmpxchg16b dword ptr \[rax\] \# Must be oword
[ 	]*78[ 	]+movq xmm1, XMMWORD PTR \[rsp\]
[ 	]*79[ 	]+movq xmm1, DWORD PTR \[rsp\]
[ 	]*80[ 	]+movq xmm1, WORD PTR \[rsp\]
[ 	]*81[ 	]+movq xmm1, BYTE PTR \[rsp\]
[ 	]*82[ 	]+movq XMMWORD PTR \[rsp\],xmm1
[ 	]*83[ 	]+movq DWORD PTR \[rsp\],xmm1
[ 	]*84[ 	]+movq WORD PTR \[rsp\],xmm1
[ 	]*85[ 	]+movq BYTE PTR \[rsp\],xmm1
[ 	]*86[ 	]+fnstsw eax
[ 	]*87[ 	]+fnstsw al
[ 	]*88[ 	]+fstsw eax
[ 	]*89[ 	]+fstsw al
[ 	]*90[ 	]+in rax,8
[ 	]*91[ 	]+out 8,rax
[ 	]*92[ 	]+movsx ax, \[rax\]
[ 	]*93[ 	]+movsx eax, \[rax\]
[ 	]*94[ 	]+movsx rax, \[rax\]
[ 	]*95[ 	]+movzx ax, \[rax\]
[ 	]*96[ 	]+movzx eax, \[rax\]
[ 	]*97[ 	]+movzx rax, \[rax\]
[ 	]*98[ 	]+movnti word ptr \[rax\], ax
[ 	]*99[ 	]+calld eax	\# 32-bit data size not allowed
[ 	]*100[ 	]+calld \[ax\]	\# 32-bit data size not allowed
[ 	]*101[ 	]+calld \[eax\]	\# 32-bit data size not allowed
[ 	]*102[ 	]+calld \[r8\]	\# 32-bit data size not allowed
[ 	]*103[ 	]+calld \[rax\]	\# 32-bit data size not allowed
[ 	]*104[ 	]+callw ax	\# 16-bit data size not allowed
[ 	]*105[ 	]+callw \[ax\]	\# 16-bit data size not allowed
[ 	]*106[ 	]+callw \[eax\]	\# 16-bit data size not allowed
[ 	]*107[ 	]+callw \[r8\]	\# 16-bit data size not allowed
[ 	]*108[ 	]+callw \[rax\]	\# 16-bit data size not allowed
[ 	]*109[ 	]+callq \[ax\]	\# no 16-bit addressing
[ 	]*110[ 	]+jmpd eax	\# 32-bit data size not allowed
[ 	]*111[ 	]+jmpd \[ax\]	\# 32-bit data size not allowed
[ 	]*112[ 	]+jmpd \[eax\]	\# 32-bit data size not allowed
[ 	]*113[ 	]+jmpd \[r8\]	\# 32-bit data size not allowed
[ 	]*114[ 	]+jmpd \[rax\]	\# 32-bit data size not allowed
GAS LISTING .*


[ 	]*115[ 	]+jmpw ax		\# 16-bit data size not allowed
[ 	]*116[ 	]+jmpw \[ax\]	\# 16-bit data size not allowed
[ 	]*117[ 	]+jmpw \[eax\]	\# 16-bit data size not allowed
[ 	]*118[ 	]+jmpw \[r8\]	\# 16-bit data size not allowed
[ 	]*119[ 	]+jmpw \[rax\]	\# 16-bit data size not allowed
[ 	]*120[ 	]+jmpq \[ax\]	\# no 16-bit addressing
