.*: Assembler messages:
.*:6: Error: .*
.*:8: Error: .*
.*:10: Error: .*
.*:12: Error: .*
.*:14: Error: .*
.*:16: Error: .*
.*:18: Error: .*
.*:20: Error: .*
.*:22: Error: .*
.*:24: Error: .*
.*:26: Error: .*
.*:28: Error: .*
.*:30: Error: .*
.*:32: Error: .*
.*:34: Error: .*
.*:36: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.include "arch-10\.s"
[ 	]*1[ 	]+\# Test -march=
[ 	]*2[ 	]+\.text
[ 	]*3[ 	]+\# cmov feature 
[ 	]*4[ 	]+\?\?\?\? 0F44D8   	cmove	%eax,%ebx
[ 	]*5[ 	]+\# MMX
[ 	]*6[ 	]+paddb %mm4,%mm3
[ 	]*7[ 	]+\# SSE
[ 	]*8[ 	]+addss %xmm4,%xmm3
[ 	]*9[ 	]+\# SSE2
[ 	]*10[ 	]+addsd %xmm4,%xmm3
[ 	]*11[ 	]+\# SSE3
[ 	]*12[ 	]+addsubpd %xmm4,%xmm3
[ 	]*13[ 	]+\# SSSE3
[ 	]*14[ 	]+phaddw %xmm4,%xmm3
[ 	]*15[ 	]+\# SSE4\.1
[ 	]*16[ 	]+phminposuw  %xmm1,%xmm3
[ 	]*17[ 	]+\# SSE4\.2
[ 	]*18[ 	]+crc32   %ecx,%ebx
[ 	]*19[ 	]+\# VMX
[ 	]*20[ 	]+vmxoff
[ 	]*21[ 	]+\# SMX
[ 	]*22[ 	]+getsec
[ 	]*23[ 	]+\# 3DNow
[ 	]*24[ 	]+pmulhrw %mm4,%mm3
[ 	]*25[ 	]+\# 3DNow Extensions
[ 	]*26[ 	]+pswapd %mm4,%mm3
[ 	]*27[ 	]+\# SSE4a
[ 	]*28[ 	]+insertq %xmm2,%xmm1
[ 	]*29[ 	]+\# SVME
[ 	]*30[ 	]+vmload
[ 	]*31[ 	]+\# ABM
[ 	]*32[ 	]+lzcnt %ecx,%ebx
[ 	]*33[ 	]+\# SSE5
[ 	]*34[ 	]+frczss          %xmm2, %xmm1
[ 	]*35[ 	]+\# PadLock
[ 	]*36[ 	]+xstorerng
