#as: -march=i686+sse4+vmx+smx+sse5+3dnowa+svme+padlock
#objdump: -dw
#name: i386 arch 10

.*:     file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	0f 44 d8             	cmove  %eax,%ebx
[ 	]*[a-f0-9]+:	0f fc dc             	paddb  %mm4,%mm3
[ 	]*[a-f0-9]+:	f3 0f 58 dc          	addss  %xmm4,%xmm3
[ 	]*[a-f0-9]+:	f2 0f 58 dc          	addsd  %xmm4,%xmm3
[ 	]*[a-f0-9]+:	66 0f d0 dc          	addsubpd %xmm4,%xmm3
[ 	]*[a-f0-9]+:	66 0f 38 01 dc       	phaddw %xmm4,%xmm3
[ 	]*[a-f0-9]+:	66 0f 38 41 d9       	phminposuw %xmm1,%xmm3
[ 	]*[a-f0-9]+:	f2 0f 38 f1 d9       	crc32l %ecx,%ebx
[ 	]*[a-f0-9]+:	0f 01 c4             	vmxoff 
[ 	]*[a-f0-9]+:	0f 37                	getsec 
[ 	]*[a-f0-9]+:	0f 0f dc b7          	pmulhrw %mm4,%mm3
[ 	]*[a-f0-9]+:	0f 0f dc bb          	pswapd %mm4,%mm3
[ 	]*[a-f0-9]+:	f2 0f 79 ca          	insertq %xmm2,%xmm1
[ 	]*[a-f0-9]+:	0f 01 da             	vmload 
[ 	]*[a-f0-9]+:	f3 0f bd d9          	lzcnt  %ecx,%ebx
[ 	]*[a-f0-9]+:	0f 7a 12 ca          	frczss %xmm2,%xmm1
[ 	]*[a-f0-9]+:	0f a7 c0             	xstore-rng 
#pass
