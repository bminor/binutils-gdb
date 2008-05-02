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
.*:38: Error: .*
.*:40: Error: .*
.*:42: Error: .*
.*:44: Error: .*
.*:46: Error: .*
.*:48: Error: .*
.*:50: Error: .*
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
[ 	]*29[ 	]+\# PCLMUL
[ 	]*30[ 	]+pclmulqdq \$8,%xmm1,%xmm0
[ 	]*31[ 	]+\# FMA
[ 	]*32[ 	]+vfmaddpd %ymm4,%ymm6,%ymm2,%ymm7
[ 	]*33[ 	]+\# MOVBE
[ 	]*34[ 	]+movbe   \(%ecx\),%ebx
[ 	]*35[ 	]+\# EPT
[ 	]*36[ 	]+invept  \(%ecx\),%ebx
[ 	]*37[ 	]+\# 3DNow
[ 	]*38[ 	]+pmulhrw %mm4,%mm3
[ 	]*39[ 	]+\# 3DNow Extensions
[ 	]*40[ 	]+pswapd %mm4,%mm3
[ 	]*41[ 	]+\# SSE4a
[ 	]*42[ 	]+insertq %xmm2,%xmm1
[ 	]*43[ 	]+\# SVME
[ 	]*44[ 	]+vmload
[ 	]*45[ 	]+\# ABM
[ 	]*46[ 	]+lzcnt %ecx,%ebx
[ 	]*47[ 	]+\# SSE5
[ 	]*48[ 	]+frczss          %xmm2, %xmm1
[ 	]*49[ 	]+\# PadLock
[ 	]*50[ 	]+xstorerng
