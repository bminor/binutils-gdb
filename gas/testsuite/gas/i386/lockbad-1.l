.*: Assembler messages:
.*:5: Error: .*
.*:6: Error: .*
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
.*:38: Error: .*
.*:39: Error: .*
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
GAS LISTING .*


[ 	]*1[ 	]+\# Unlockable Instructions
[ 	]*2[ 	]+
[ 	]*3[ 	]+\.text
[ 	]*4[ 	]+foo:
[ 	]*5[ 	]+lock mov %ecx, %eax
[ 	]*6[ 	]+lock mov \(%ebx\), %eax
[ 	]*7[ 	]+
[ 	]*8[ 	]+lock add %ebx, %eax
[ 	]*9[ 	]+lock add \$0x64, %ebx
[ 	]*10[ 	]+lock adc %ebx, %eax
[ 	]*11[ 	]+lock adc \$0x64, %ebx
[ 	]*12[ 	]+lock and %ebx, %eax
[ 	]*13[ 	]+lock and \$0x64, %ebx
[ 	]*14[ 	]+lock btc %eax, %ebx
[ 	]*15[ 	]+lock btc \$0x64, %ebx
[ 	]*16[ 	]+lock btr %eax, %ebx
[ 	]*17[ 	]+lock btr \$0x64, %ebx
[ 	]*18[ 	]+lock bts %eax, %ebx
[ 	]*19[ 	]+lock bts \$0x64, %ebx
[ 	]*20[ 	]+lock cmpxchg %eax,%ebx
[ 	]*21[ 	]+lock decl %ebx
[ 	]*22[ 	]+lock incl %ebx
[ 	]*23[ 	]+lock negl %ebx
[ 	]*24[ 	]+lock notl %ebx
[ 	]*25[ 	]+lock or %ebx, %eax
[ 	]*26[ 	]+lock or \$0x64, %ebx
[ 	]*27[ 	]+lock sbb %ebx, %eax
[ 	]*28[ 	]+lock sbb \$0x64, %ebx
[ 	]*29[ 	]+lock sub %ebx, %eax
[ 	]*30[ 	]+lock sub \$0x64, %ebx
[ 	]*31[ 	]+lock xadd %eax, %ebx
[ 	]*32[ 	]+lock xchg %ebx, %eax
[ 	]*33[ 	]+lock xchg %eax, %ebx
[ 	]*34[ 	]+lock xor %ebx, %eax
[ 	]*35[ 	]+lock xor \$0x64, %ebx
[ 	]*36[ 	]+
[ 	]*37[ 	]+\.intel_syntax noprefix
[ 	]*38[ 	]+lock mov eax,ebx
[ 	]*39[ 	]+lock mov eax,DWORD PTR \[ebx\]
[ 	]*40[ 	]+
[ 	]*41[ 	]+lock add eax,ebx
[ 	]*42[ 	]+lock add ebx,0x64
[ 	]*43[ 	]+lock adc eax,ebx
[ 	]*44[ 	]+lock adc ebx,0x64
[ 	]*45[ 	]+lock and eax,ebx
[ 	]*46[ 	]+lock and ebx,0x64
[ 	]*47[ 	]+lock btc ebx,eax
[ 	]*48[ 	]+lock btc ebx,0x64
[ 	]*49[ 	]+lock btr ebx,eax
[ 	]*50[ 	]+lock btr ebx,0x64
[ 	]*51[ 	]+lock bts ebx,eax
[ 	]*52[ 	]+lock bts ebx,0x64
[ 	]*53[ 	]+lock cmpxchg ebx,eax
[ 	]*54[ 	]+lock dec ebx
[ 	]*55[ 	]+lock inc ebx
[ 	]*56[ 	]+lock neg ebx
[ 	]*57[ 	]+lock not ebx
GAS LISTING .*


[ 	]*58[ 	]+lock or eax,ebx
[ 	]*59[ 	]+lock or ebx,0x64
[ 	]*60[ 	]+lock sbb eax,ebx
[ 	]*61[ 	]+lock sbb ebx,0x64
[ 	]*62[ 	]+lock sub eax,ebx
[ 	]*63[ 	]+lock sub ebx,0x64
[ 	]*64[ 	]+lock xadd ebx,eax
[ 	]*65[ 	]+lock xchg ebx,eax
[ 	]*66[ 	]+lock xchg ebx,eax
[ 	]*67[ 	]+lock xor eax,ebx
[ 	]*68[ 	]+lock xor ebx,0x64
