#as: -J
#objdump: -dw
#name: x86-64 SIMD

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	f2 0f d0 0d 78 56 34 12 	addsubps 0x12345678\(%rip\),%xmm1        # 12345680 <_start\+0x12345680>
[ 	]*[a-f0-9]+:	66 0f 2f 0d 78 56 34 12 	comisd 0x12345678\(%rip\),%xmm1        # 12345688 <_start\+0x12345688>
[ 	]*[a-f0-9]+:	0f 2f 0d 78 56 34 12 	comiss 0x12345678\(%rip\),%xmm1        # 1234568f <_start\+0x1234568f>
[ 	]*[a-f0-9]+:	f3 0f e6 0d 78 56 34 12 	cvtdq2pd 0x12345678\(%rip\),%xmm1        # 12345697 <_start\+0x12345697>
[ 	]*[a-f0-9]+:	f2 0f e6 0d 78 56 34 12 	cvtpd2dq 0x12345678\(%rip\),%xmm1        # 1234569f <_start\+0x1234569f>
[ 	]*[a-f0-9]+:	0f 5a 0d 78 56 34 12 	cvtps2pd 0x12345678\(%rip\),%xmm1        # 123456a6 <_start\+0x123456a6>
[ 	]*[a-f0-9]+:	f3 0f 5b 0d 78 56 34 12 	cvttps2dq 0x12345678\(%rip\),%xmm1        # 123456ae <_start\+0x123456ae>
[ 	]*[a-f0-9]+:	f2 0f 7c 0d 78 56 34 12 	haddps 0x12345678\(%rip\),%xmm1        # 123456b6 <_start\+0x123456b6>
[ 	]*[a-f0-9]+:	f3 0f 7f 0d 78 56 34 12 	movdqu %xmm1,0x12345678\(%rip\)        # 123456be <_start\+0x123456be>
[ 	]*[a-f0-9]+:	f3 0f 6f 0d 78 56 34 12 	movdqu 0x12345678\(%rip\),%xmm1        # 123456c6 <_start\+0x123456c6>
[ 	]*[a-f0-9]+:	66 0f 17 0d 78 56 34 12 	movhpd %xmm1,0x12345678\(%rip\)        # 123456ce <_start\+0x123456ce>
[ 	]*[a-f0-9]+:	66 0f 16 0d 78 56 34 12 	movhpd 0x12345678\(%rip\),%xmm1        # 123456d6 <_start\+0x123456d6>
[ 	]*[a-f0-9]+:	0f 17 0d 78 56 34 12 	movhps %xmm1,0x12345678\(%rip\)        # 123456dd <_start\+0x123456dd>
[ 	]*[a-f0-9]+:	0f 16 0d 78 56 34 12 	movhps 0x12345678\(%rip\),%xmm1        # 123456e4 <_start\+0x123456e4>
[ 	]*[a-f0-9]+:	66 0f 13 0d 78 56 34 12 	movlpd %xmm1,0x12345678\(%rip\)        # 123456ec <_start\+0x123456ec>
[ 	]*[a-f0-9]+:	66 0f 12 0d 78 56 34 12 	movlpd 0x12345678\(%rip\),%xmm1        # 123456f4 <_start\+0x123456f4>
[ 	]*[a-f0-9]+:	0f 13 0d 78 56 34 12 	movlps %xmm1,0x12345678\(%rip\)        # 123456fb <_start\+0x123456fb>
[ 	]*[a-f0-9]+:	0f 12 0d 78 56 34 12 	movlps 0x12345678\(%rip\),%xmm1        # 12345702 <_start\+0x12345702>
[ 	]*[a-f0-9]+:	66 0f d6 0d 78 56 34 12 	movq   %xmm1,0x12345678\(%rip\)        # 1234570a <_start\+0x1234570a>
[ 	]*[a-f0-9]+:	f3 0f 7e 0d 78 56 34 12 	movq   0x12345678\(%rip\),%xmm1        # 12345712 <_start\+0x12345712>
[ 	]*[a-f0-9]+:	f3 0f 16 0d 78 56 34 12 	movshdup 0x12345678\(%rip\),%xmm1        # 1234571a <_start\+0x1234571a>
[ 	]*[a-f0-9]+:	f3 0f 12 0d 78 56 34 12 	movsldup 0x12345678\(%rip\),%xmm1        # 12345722 <_start\+0x12345722>
[ 	]*[a-f0-9]+:	f3 0f 70 0d 78 56 34 12 90 	pshufhw \$0x90,0x12345678\(%rip\),%xmm1        # 1234572b <_start\+0x1234572b>
[ 	]*[a-f0-9]+:	f2 0f 70 0d 78 56 34 12 90 	pshuflw \$0x90,0x12345678\(%rip\),%xmm1        # 12345734 <_start\+0x12345734>
[ 	]*[a-f0-9]+:	0f 60 0d 78 56 34 12 	punpcklbw 0x12345678\(%rip\),%mm1        # 1234573b <_start\+0x1234573b>
[ 	]*[a-f0-9]+:	0f 62 0d 78 56 34 12 	punpckldq 0x12345678\(%rip\),%mm1        # 12345742 <_start\+0x12345742>
[ 	]*[a-f0-9]+:	0f 61 0d 78 56 34 12 	punpcklwd 0x12345678\(%rip\),%mm1        # 12345749 <_start\+0x12345749>
[ 	]*[a-f0-9]+:	66 0f 60 0d 78 56 34 12 	punpcklbw 0x12345678\(%rip\),%xmm1        # 12345751 <_start\+0x12345751>
[ 	]*[a-f0-9]+:	66 0f 62 0d 78 56 34 12 	punpckldq 0x12345678\(%rip\),%xmm1        # 12345759 <_start\+0x12345759>
[ 	]*[a-f0-9]+:	66 0f 61 0d 78 56 34 12 	punpcklwd 0x12345678\(%rip\),%xmm1        # 12345761 <_start\+0x12345761>
[ 	]*[a-f0-9]+:	66 0f 6c 0d 78 56 34 12 	punpcklqdq 0x12345678\(%rip\),%xmm1        # 12345769 <_start\+0x12345769>
[ 	]*[a-f0-9]+:	66 0f 2e 0d 78 56 34 12 	ucomisd 0x12345678\(%rip\),%xmm1        # 12345771 <_start\+0x12345771>
[ 	]*[a-f0-9]+:	0f 2e 0d 78 56 34 12 	ucomiss 0x12345678\(%rip\),%xmm1        # 12345778 <_start\+0x12345778>
[ 	]*[a-f0-9]+:	f2 0f c2 00 00       	cmpeqsd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f c2 00 00       	cmpeqss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 2a 00          	cvtpi2pd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	0f 2a 00             	cvtpi2ps \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	0f 2d 00             	cvtps2pi \(%rax\),%mm0
[ 	]*[a-f0-9]+:	f2 0f 2d 00          	cvtsd2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f2 48 0f 2d 00       	cvtsd2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f2 0f 2c 00          	cvttsd2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f2 48 0f 2c 00       	cvttsd2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f2 0f 5a 00          	cvtsd2ss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5a 00          	cvtss2sd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 2d 00          	cvtss2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f3 48 0f 2d 00       	cvtss2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f3 0f 2c 00          	cvttss2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f3 48 0f 2c 00       	cvttss2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f2 0f 5e 00          	divsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5e 00          	divss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 5f 00          	maxsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5f 00          	maxss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 2b 00          	movntsd %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f3 0f 2b 00          	movntss %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f2 0f 10 00          	movsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 11 00          	movsd  %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f3 0f 10 00          	movss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 11 00          	movss  %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f2 0f 59 00          	mulsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 59 00          	mulss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 53 00          	rcpss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 3a 0b 00 00    	roundsd \$0x0,\(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 3a 0a 00 00    	roundss \$0x0,\(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 52 00          	rsqrtss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 51 00          	sqrtsd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 51 00          	sqrtss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 5c 00          	subsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5c 00          	subss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 20 00       	pmovsxbw \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 21 00       	pmovsxbd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 22 00       	pmovsxbq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 23 00       	pmovsxwd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 24 00       	pmovsxwq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 25 00       	pmovsxdq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 30 00       	pmovzxbw \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 31 00       	pmovzxbd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 32 00       	pmovzxbq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 33 00       	pmovzxwd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 34 00       	pmovzxwq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 35 00       	pmovzxdq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 3a 21 00 00    	insertps \$0x0,\(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f d0 0d 78 56 34 12 	addsubps 0x12345678\(%rip\),%xmm1        # 1234585e <_start\+0x1234585e>
[ 	]*[a-f0-9]+:	66 0f 2f 0d 78 56 34 12 	comisd 0x12345678\(%rip\),%xmm1        # 12345866 <_start\+0x12345866>
[ 	]*[a-f0-9]+:	0f 2f 0d 78 56 34 12 	comiss 0x12345678\(%rip\),%xmm1        # 1234586d <_start\+0x1234586d>
[ 	]*[a-f0-9]+:	f3 0f e6 0d 78 56 34 12 	cvtdq2pd 0x12345678\(%rip\),%xmm1        # 12345875 <_start\+0x12345875>
[ 	]*[a-f0-9]+:	f2 0f e6 0d 78 56 34 12 	cvtpd2dq 0x12345678\(%rip\),%xmm1        # 1234587d <_start\+0x1234587d>
[ 	]*[a-f0-9]+:	0f 5a 0d 78 56 34 12 	cvtps2pd 0x12345678\(%rip\),%xmm1        # 12345884 <_start\+0x12345884>
[ 	]*[a-f0-9]+:	f3 0f 5b 0d 78 56 34 12 	cvttps2dq 0x12345678\(%rip\),%xmm1        # 1234588c <_start\+0x1234588c>
[ 	]*[a-f0-9]+:	f2 0f 7c 0d 78 56 34 12 	haddps 0x12345678\(%rip\),%xmm1        # 12345894 <_start\+0x12345894>
[ 	]*[a-f0-9]+:	f3 0f 7f 0d 78 56 34 12 	movdqu %xmm1,0x12345678\(%rip\)        # 1234589c <_start\+0x1234589c>
[ 	]*[a-f0-9]+:	f3 0f 6f 0d 78 56 34 12 	movdqu 0x12345678\(%rip\),%xmm1        # 123458a4 <_start\+0x123458a4>
[ 	]*[a-f0-9]+:	66 0f 17 0d 78 56 34 12 	movhpd %xmm1,0x12345678\(%rip\)        # 123458ac <_start\+0x123458ac>
[ 	]*[a-f0-9]+:	66 0f 16 0d 78 56 34 12 	movhpd 0x12345678\(%rip\),%xmm1        # 123458b4 <_start\+0x123458b4>
[ 	]*[a-f0-9]+:	0f 17 0d 78 56 34 12 	movhps %xmm1,0x12345678\(%rip\)        # 123458bb <_start\+0x123458bb>
[ 	]*[a-f0-9]+:	0f 16 0d 78 56 34 12 	movhps 0x12345678\(%rip\),%xmm1        # 123458c2 <_start\+0x123458c2>
[ 	]*[a-f0-9]+:	66 0f 13 0d 78 56 34 12 	movlpd %xmm1,0x12345678\(%rip\)        # 123458ca <_start\+0x123458ca>
[ 	]*[a-f0-9]+:	66 0f 12 0d 78 56 34 12 	movlpd 0x12345678\(%rip\),%xmm1        # 123458d2 <_start\+0x123458d2>
[ 	]*[a-f0-9]+:	0f 13 0d 78 56 34 12 	movlps %xmm1,0x12345678\(%rip\)        # 123458d9 <_start\+0x123458d9>
[ 	]*[a-f0-9]+:	0f 12 0d 78 56 34 12 	movlps 0x12345678\(%rip\),%xmm1        # 123458e0 <_start\+0x123458e0>
[ 	]*[a-f0-9]+:	66 0f d6 0d 78 56 34 12 	movq   %xmm1,0x12345678\(%rip\)        # 123458e8 <_start\+0x123458e8>
[ 	]*[a-f0-9]+:	f3 0f 7e 0d 78 56 34 12 	movq   0x12345678\(%rip\),%xmm1        # 123458f0 <_start\+0x123458f0>
[ 	]*[a-f0-9]+:	f3 0f 16 0d 78 56 34 12 	movshdup 0x12345678\(%rip\),%xmm1        # 123458f8 <_start\+0x123458f8>
[ 	]*[a-f0-9]+:	f3 0f 12 0d 78 56 34 12 	movsldup 0x12345678\(%rip\),%xmm1        # 12345900 <_start\+0x12345900>
[ 	]*[a-f0-9]+:	f3 0f 70 0d 78 56 34 12 90 	pshufhw \$0x90,0x12345678\(%rip\),%xmm1        # 12345909 <_start\+0x12345909>
[ 	]*[a-f0-9]+:	f2 0f 70 0d 78 56 34 12 90 	pshuflw \$0x90,0x12345678\(%rip\),%xmm1        # 12345912 <_start\+0x12345912>
[ 	]*[a-f0-9]+:	0f 60 0d 78 56 34 12 	punpcklbw 0x12345678\(%rip\),%mm1        # 12345919 <_start\+0x12345919>
[ 	]*[a-f0-9]+:	0f 62 0d 78 56 34 12 	punpckldq 0x12345678\(%rip\),%mm1        # 12345920 <_start\+0x12345920>
[ 	]*[a-f0-9]+:	0f 61 0d 78 56 34 12 	punpcklwd 0x12345678\(%rip\),%mm1        # 12345927 <_start\+0x12345927>
[ 	]*[a-f0-9]+:	66 0f 60 0d 78 56 34 12 	punpcklbw 0x12345678\(%rip\),%xmm1        # 1234592f <_start\+0x1234592f>
[ 	]*[a-f0-9]+:	66 0f 62 0d 78 56 34 12 	punpckldq 0x12345678\(%rip\),%xmm1        # 12345937 <_start\+0x12345937>
[ 	]*[a-f0-9]+:	66 0f 61 0d 78 56 34 12 	punpcklwd 0x12345678\(%rip\),%xmm1        # 1234593f <_start\+0x1234593f>
[ 	]*[a-f0-9]+:	66 0f 6c 0d 78 56 34 12 	punpcklqdq 0x12345678\(%rip\),%xmm1        # 12345947 <_start\+0x12345947>
[ 	]*[a-f0-9]+:	66 0f 2e 0d 78 56 34 12 	ucomisd 0x12345678\(%rip\),%xmm1        # 1234594f <_start\+0x1234594f>
[ 	]*[a-f0-9]+:	0f 2e 0d 78 56 34 12 	ucomiss 0x12345678\(%rip\),%xmm1        # 12345956 <_start\+0x12345956>
[ 	]*[a-f0-9]+:	f2 0f c2 00 00       	cmpeqsd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f c2 00 00       	cmpeqss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 2a 00          	cvtpi2pd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	0f 2a 00             	cvtpi2ps \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	0f 2d 00             	cvtps2pi \(%rax\),%mm0
[ 	]*[a-f0-9]+:	f2 0f 2d 00          	cvtsd2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f2 48 0f 2d 00       	cvtsd2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f2 0f 2c 00          	cvttsd2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f2 48 0f 2c 00       	cvttsd2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f2 0f 5a 00          	cvtsd2ss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5a 00          	cvtss2sd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 2d 00          	cvtss2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f3 48 0f 2d 00       	cvtss2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f3 0f 2c 00          	cvttss2si \(%rax\),%eax
[ 	]*[a-f0-9]+:	f3 48 0f 2c 00       	cvttss2siq \(%rax\),%rax
[ 	]*[a-f0-9]+:	f2 0f 5e 00          	divsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5e 00          	divss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 5f 00          	maxsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5f 00          	maxss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 2b 00          	movntsd %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f3 0f 2b 00          	movntss %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f2 0f 10 00          	movsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 11 00          	movsd  %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f3 0f 10 00          	movss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 11 00          	movss  %xmm0,\(%rax\)
[ 	]*[a-f0-9]+:	f2 0f 59 00          	mulsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 59 00          	mulss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 53 00          	rcpss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 3a 0b 00 00    	roundsd \$0x0,\(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 3a 0a 00 00    	roundss \$0x0,\(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 52 00          	rsqrtss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 51 00          	sqrtsd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 51 00          	sqrtss \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f2 0f 5c 00          	subsd  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	f3 0f 5c 00          	subss  \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 20 00       	pmovsxbw \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 21 00       	pmovsxbd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 22 00       	pmovsxbq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 23 00       	pmovsxwd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 24 00       	pmovsxwq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 25 00       	pmovsxdq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 30 00       	pmovzxbw \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 31 00       	pmovzxbd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 32 00       	pmovzxbq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 33 00       	pmovzxwd \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 34 00       	pmovzxwq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 38 35 00       	pmovzxdq \(%rax\),%xmm0
[ 	]*[a-f0-9]+:	66 0f 3a 21 00 00    	insertps \$0x0,\(%rax\),%xmm0
#pass
