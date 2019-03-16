#source: optimize-6.s
#as: -O2 -march=+noavx+noavx512vl
#objdump: -drw
#name: optimized encoding 6c with -O2

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	62 f1 f5 4f 55 e9    	vandnpd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 48 55 e9    	vandnpd %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 74 4f 55 e9    	vandnps %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 74 48 55 e9    	vandnps %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 75 4f df e9    	vpandnd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 48 df e9    	vpandnd %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 f5 4f df e9    	vpandnq %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 48 df e9    	vpandnq %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 f5 4f 57 e9    	vxorpd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 48 57 e9    	vxorpd %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 74 4f 57 e9    	vxorps %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 74 48 57 e9    	vxorps %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 75 4f ef e9    	vpxord %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 48 ef e9    	vpxord %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 f5 4f ef e9    	vpxorq %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 48 ef e9    	vpxorq %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 75 4f f8 e9    	vpsubb %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 48 f8 e9    	vpsubb %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 75 4f f9 e9    	vpsubw %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 48 f9 e9    	vpsubw %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 75 4f fa e9    	vpsubd %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 75 48 fa e9    	vpsubd %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	62 f1 f5 4f fb e9    	vpsubq %zmm1,%zmm1,%zmm5\{%k7\}
 +[a-f0-9]+:	62 f1 f5 48 fb e9    	vpsubq %zmm1,%zmm1,%zmm5
 +[a-f0-9]+:	c5 f4 47 e9          	kxorw  %k1,%k1,%k5
 +[a-f0-9]+:	c5 f4 47 e9          	kxorw  %k1,%k1,%k5
 +[a-f0-9]+:	c5 f4 42 e9          	kandnw %k1,%k1,%k5
 +[a-f0-9]+:	c5 f4 42 e9          	kandnw %k1,%k1,%k5
#pass
