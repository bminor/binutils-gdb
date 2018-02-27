#as: -O2
#objdump: -drw
#name: optimized encoding 1 with -O2

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	62 f1 f5 4f 55 e9    	vandnpd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 af 55 e9    	vandnpd %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f1 55 e9          	vandnpd %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 55 e9          	vandnpd %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 74 4f 55 e9    	vandnps %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 74 af 55 e9    	vandnps %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f0 55 e9          	vandnps %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f0 55 e9          	vandnps %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 df e9          	vpandn %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 75 4f df e9    	vpandnd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 af df e9    	vpandnd %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f1 df e9          	vpandn %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 df e9          	vpandn %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 f5 4f df e9    	vpandnq %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 af df e9    	vpandnq %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f1 df e9          	vpandn %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 df e9          	vpandn %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 f5 4f 57 e9    	vxorpd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 af 57 e9    	vxorpd %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f1 57 e9          	vxorpd %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 57 e9          	vxorpd %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 74 4f 57 e9    	vxorps %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 74 af 57 e9    	vxorps %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f0 57 e9          	vxorps %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f0 57 e9          	vxorps %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 ef e9          	vpxor  %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 75 4f ef e9    	vpxord %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 af ef e9    	vpxord %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f1 ef e9          	vpxor  %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 ef e9          	vpxor  %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	62 f1 f5 4f ef e9    	vpxorq %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 af ef e9    	vpxorq %ymm1,%ymm1,%ymm5\{%k7\}\{z\}
 +[a-f0-9]+:	c5 f1 ef e9          	vpxor  %xmm1,%xmm1,%xmm5
 +[a-f0-9]+:	c5 f1 ef e9          	vpxor  %xmm1,%xmm1,%xmm5
#pass
