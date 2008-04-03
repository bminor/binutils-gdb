.*: Assembler messages:
.*:4: Error: .*
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
.*:38: Error: .*
.*:40: Error: .*
.*:42: Error: .*
.*:44: Error: .*
.*:46: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.include "arch-10\.s"
[ 	]*1[ 	]+\# Test -march=
[ 	]*2[ 	]+\.text
[ 	]*3[ 	]+\# cmov feature 
[ 	]*4[ 	]+cmove	%eax,%ebx
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
[ 	]*19[ 	]+\# AVX
[ 	]*20[ 	]+vzeroall
[ 	]*21[ 	]+\# VMX
[ 	]*22[ 	]+vmxoff
[ 	]*23[ 	]+\# SMX
[ 	]*24[ 	]+getsec
[ 	]*25[ 	]+\# Xsave
[ 	]*26[ 	]+xgetbv
[ 	]*27[ 	]+\# AES
[ 	]*28[ 	]+aesenc  \(%ecx\),%xmm0
[ 	]*29[ 	]+\# CLMUL
[ 	]*30[ 	]+pclmulqdq \$8,%xmm1,%xmm0
[ 	]*31[ 	]+\# FMA
[ 	]*32[ 	]+vfmaddpd %ymm4,%ymm6,%ymm2,%ymm7
[ 	]*33[ 	]+\# 3DNow
[ 	]*34[ 	]+pmulhrw %mm4,%mm3
[ 	]*35[ 	]+\# 3DNow Extensions
[ 	]*36[ 	]+pswapd %mm4,%mm3
[ 	]*37[ 	]+\# SSE4a
[ 	]*38[ 	]+insertq %xmm2,%xmm1
[ 	]*39[ 	]+\# SVME
[ 	]*40[ 	]+vmload
[ 	]*41[ 	]+\# ABM
[ 	]*42[ 	]+lzcnt %ecx,%ebx
[ 	]*43[ 	]+\# SSE5
[ 	]*44[ 	]+frczss          %xmm2, %xmm1
[ 	]*45[ 	]+\# PadLock
[ 	]*46[ 	]+xstorerng
