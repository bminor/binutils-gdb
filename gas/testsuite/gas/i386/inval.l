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
.*:58: Error: .*
.*:59: Error: .*
.*:61: Error: .*
.*:62: Error: .*
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
.*:75: Error: .*
.*:76: Error: .*
.*:77: Error: .*
.*:78: Error: .*
.*:79: Error: .*
.*:80: Error: .*
.*:82: Error: .*
.*:83: Error: .*
.*:84: Error: .*
.*:85: Error: .*
.*:87: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+\# All the following should be illegal
[ 	]*3[ 	]+mov	\(%dx\),%al
[ 	]*4[ 	]+mov	\(%eax,%esp,2\),%al
[ 	]*5[ 	]+setae	%eax
[ 	]*6[ 	]+pushb	%ds
[ 	]*7[ 	]+popb	%ds
[ 	]*8[ 	]+pushb	%al
[ 	]*9[ 	]+popb	%al
[ 	]*10[ 	]+pushb	%ah
[ 	]*11[ 	]+popb	%ah
[ 	]*12[ 	]+pushb	%ax
[ 	]*13[ 	]+popb	%ax
[ 	]*14[ 	]+pushb	%eax
[ 	]*15[ 	]+popb	%eax
[ 	]*16[ 	]+movb	%ds,%ax
[ 	]*17[ 	]+movb	%ds,%eax
[ 	]*18[ 	]+movb	%ax,%ds
[ 	]*19[ 	]+movb	%eax,%ds
[ 	]*20[ 	]+movdb	%eax,%mm0
[ 	]*21[ 	]+movqb	0,%mm0
[ 	]*22[ 	]+ldsb	0,%eax
[ 	]*23[ 	]+setnew	0
[ 	]*24[ 	]+movdw	%eax,%mm0
[ 	]*25[ 	]+movqw	0,%mm0
[ 	]*26[ 	]+div	%cx,%al
[ 	]*27[ 	]+div	%cl,%ax
[ 	]*28[ 	]+div	%ecx,%al
[ 	]*29[ 	]+imul	10,%bx,%ecx
[ 	]*30[ 	]+imul	10,%bx,%al
[ 	]*31[ 	]+popab
[ 	]*32[ 	]+stil
[ 	]*33[ 	]+aaab
[ 	]*34[ 	]+cwdel
[ 	]*35[ 	]+cwdw
[ 	]*36[ 	]+callww	0
[ 	]*37[ 	]+foo:	jaw	foo
[ 	]*38[ 	]+jcxzw	foo
[ 	]*39[ 	]+jecxzl	foo
[ 	]*40[ 	]+loopb	foo
[ 	]*41[ 	]+xlatw	%es:%bx
[ 	]*42[ 	]+xlatl	%es:%bx
[ 	]*43[ 	]+intl	2
[ 	]*44[ 	]+int3b
[ 	]*45[ 	]+hltb
[ 	]*46[ 	]+fstb	%st\(0\)
[ 	]*47[ 	]+fcompll	28\(%ebp\)
[ 	]*48[ 	]+fldlw	\(%eax\)
[ 	]*49[ 	]+movl	\$%ebx,%eax
[ 	]*50[ 	]+insertq	\$4,\$2,%xmm2,%ebx
[ 	]*51[ 	]+cvtsi2ssq \(%eax\),%xmm1
[ 	]*52[ 	]+cvtsi2sdq \(%eax\),%xmm1
[ 	]*53[ 	]+fnstsw %eax
[ 	]*54[ 	]+fnstsw %al
[ 	]*55[ 	]+fstsw %eax
[ 	]*56[ 	]+fstsw %al
[ 	]*57[ 	]+
GAS LISTING .*


[ 	]*58[ 	]+movsxw \(%eax\),%eax
[ 	]*59[ 	]+movzxw \(%eax\),%eax
[ 	]*60[ 	]+
[ 	]*61[ 	]+movnti %ax, \(%eax\)
[ 	]*62[ 	]+movntiw %ax, \(%eax\)
[ 	]*63[ 	]+
[ 	]*64[ 	]+\.intel_syntax noprefix
[ 	]*65[ 	]+cvtsi2ss xmm1,QWORD PTR \[eax\]
[ 	]*66[ 	]+cvtsi2sd xmm1,QWORD PTR \[eax\]
[ 	]*67[ 	]+cvtsi2ssq xmm1,QWORD PTR \[eax\]
[ 	]*68[ 	]+cvtsi2sdq xmm1,QWORD PTR \[eax\]
[ 	]*69[ 	]+movq xmm1, XMMWORD PTR \[esp\]
[ 	]*70[ 	]+movq xmm1, DWORD PTR \[esp\]
[ 	]*71[ 	]+movq xmm1, WORD PTR \[esp\]
[ 	]*72[ 	]+movq xmm1, BYTE PTR \[esp\]
[ 	]*73[ 	]+movq XMMWORD PTR \[esp\],xmm1
[ 	]*74[ 	]+movq DWORD PTR \[esp\],xmm1
[ 	]*75[ 	]+movq WORD PTR \[esp\],xmm1
[ 	]*76[ 	]+movq BYTE PTR \[esp\],xmm1
[ 	]*77[ 	]+fnstsw eax
[ 	]*78[ 	]+fnstsw al
[ 	]*79[ 	]+fstsw eax
[ 	]*80[ 	]+fstsw al
[ 	]*81[ 	]+
[ 	]*82[ 	]+movsx ax, \[eax\]
[ 	]*83[ 	]+movsx eax, \[eax\]
[ 	]*84[ 	]+movzx ax, \[eax\]
[ 	]*85[ 	]+movzx eax, \[eax\]
[ 	]*86[ 	]+
[ 	]*87[ 	]+movnti word ptr \[eax\], ax
