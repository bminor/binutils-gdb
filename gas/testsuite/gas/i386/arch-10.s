# Test -march=
	.text
# cmov feature 
cmove	%eax,%ebx
# MMX
paddb %mm4,%mm3
# SSE
addss %xmm4,%xmm3
# SSE2
addsd %xmm4,%xmm3
# SSE3
addsubpd %xmm4,%xmm3
# SSSE3
phaddw %xmm4,%xmm3
# SSE4.1
phminposuw  %xmm1,%xmm3
# SSE4.2
crc32   %ecx,%ebx
# VMX
vmxoff
# SMX
getsec
# 3DNow
pmulhrw %mm4,%mm3
# 3DNow Extensions
pswapd %mm4,%mm3
# SSE4a
insertq %xmm2,%xmm1
# SVME
vmload
# ABM
lzcnt %ecx,%ebx
# SSE5
frczss          %xmm2, %xmm1
# PadLock
xstorerng
