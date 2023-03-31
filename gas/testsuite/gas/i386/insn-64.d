#objdump: -dw
#name: .insn (64-bit code)

.*: +file format .*

Disassembly of section .text:

0+ <insn>:
[ 	]*[a-f0-9]+:	90[ 	]+nop
[ 	]*[a-f0-9]+:	f3 90[ 	]+pause
[ 	]*[a-f0-9]+:	f3 90[ 	]+pause
[ 	]*[a-f0-9]+:	d9 ee[ 	]+fldz
[ 	]*[a-f0-9]+:	f3 0f 01 e8[ 	]+setssbsy
[ 	]*[a-f0-9]+:	44 8b c1[ 	]+mov    %ecx,%r8d
[ 	]*[a-f0-9]+:	48 8b c8[ 	]+mov    %rax,%rcx
[ 	]*[a-f0-9]+:	41 89 48 08[ 	]+mov    %ecx,0x8\(%r8\)
[ 	]*[a-f0-9]+:	42 8b 0c 05 80 80 00 00[ 	]+mov    0x8080\(,%r8,1\),%ecx
[ 	]*[a-f0-9]+:	66 0f be cc[ 	]+movsbw %ah,%cx
[ 	]*[a-f0-9]+:	0f bf c8[ 	]+movswl %ax,%ecx
[ 	]*[a-f0-9]+:	48 63 c8[ 	]+movslq %eax,%rcx
[ 	]*[a-f0-9]+:	48 0f ca[ 	]+bswap  %rdx
[ 	]*[a-f0-9]+:	41 0f c8[ 	]+bswap  %r8d
[ 	]*[a-f0-9]+:	c5 fc 77[ 	]+vzeroall
[ 	]*[a-f0-9]+:	c4 e1 7c 77[ 	]+vzeroall
[ 	]*[a-f0-9]+:	c4 c1 71 58 d0[ 	]+vaddpd %xmm8,%xmm1,%xmm2
[ 	]*[a-f0-9]+:	c5 b5 58 d0[ 	]+vaddpd %ymm0,%ymm9,%ymm2
[ 	]*[a-f0-9]+:	c5 72 58 d0[ 	]+vaddss %xmm0,%xmm1,%xmm10
[ 	]*[a-f0-9]+:	c4 e3 69 68 19 80[ 	]+vfmaddps %xmm8,\(%rcx\),%xmm2,%xmm3
[ 	]*[a-f0-9]+:	67 c4 e3 e9 68 19 00[ 	]+vfmaddps \(%ecx\),%xmm0,%xmm2,%xmm3
[ 	]*[a-f0-9]+:	c4 c3 e9 68 18 10[ 	]+vfmaddps \(%r8\),%xmm1,%xmm2,%xmm3
[ 	]*[a-f0-9]+:	c4 c1 78 92 c8[ 	]+kmovw  %r8d,%k1
[ 	]*[a-f0-9]+:	c5 78 93 c1[ 	]+kmovw  %k1,%r8d
[ 	]*[a-f0-9]+:	62 b1 74 38 58 d0[ 	]+vaddps \{rd-sae\},%zmm16,%zmm1,%zmm2
[ 	]*[a-f0-9]+:	62 f1 74 10 58 d0[ 	]+vaddps \{rn-sae\},%zmm0,%zmm17,%zmm2
[ 	]*[a-f0-9]+:	62 e1 74 58 58 d0[ 	]+vaddps \{ru-sae\},%zmm0,%zmm1,%zmm18
[ 	]*[a-f0-9]+:	c4 e2 39 92 1c 48[ 	]+vgatherdps %xmm8,\(%rax,%xmm1,2\),%xmm3
[ 	]*[a-f0-9]+:	c4 c2 79 92 1c 48[ 	]+vgatherdps %xmm0,\(%r8,%xmm1,2\),%xmm3
[ 	]*[a-f0-9]+:	c4 a2 79 92 1c 48[ 	]+vgatherdps %xmm0,\(%rax,%xmm9,2\),%xmm3
[ 	]*[a-f0-9]+:	c4 62 79 92 1c 48[ 	]+vgatherdps %xmm0,\(%rax,%xmm1,2\),%xmm11
[ 	]*[a-f0-9]+:	62 d2 fd 0c 93 1c 48[ 	]+vgatherqpd \(%r8,%xmm1,2\),%xmm3\{%k4\}
[ 	]*[a-f0-9]+:	62 b2 fd 0c 93 1c 48[ 	]+vgatherqpd \(%rax,%xmm9,2\),%xmm3\{%k4\}
[ 	]*[a-f0-9]+:	62 f2 fd 04 93 1c 48[ 	]+vgatherqpd \(%rax,%xmm17,2\),%xmm3\{%k4\}
[ 	]*[a-f0-9]+:	62 72 fd 0c 93 1c 48[ 	]+vgatherqpd \(%rax,%xmm1,2\),%xmm11\{%k4\}
[ 	]*[a-f0-9]+:	62 e2 fd 0c 93 1c 48[ 	]+vgatherqpd \(%rax,%xmm1,2\),%xmm19\{%k4\}
#pass
