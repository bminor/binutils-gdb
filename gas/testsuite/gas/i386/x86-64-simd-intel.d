#source: x86-64-simd.s
#as: -J
#objdump: -dw -Mintel
#name: x86-64 SIMD (Intel mode)

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	f2 0f d0 0d 78 56 34 12 	addsubps xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345680 <_start\+0x12345680>
[ 	]*[a-f0-9]+:	66 0f 2f 0d 78 56 34 12 	comisd xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345688 <_start\+0x12345688>
[ 	]*[a-f0-9]+:	0f 2f 0d 78 56 34 12 	comiss xmm1,DWORD PTR \[rip\+0x12345678\]        # 1234568f <_start\+0x1234568f>
[ 	]*[a-f0-9]+:	f3 0f e6 0d 78 56 34 12 	cvtdq2pd xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345697 <_start\+0x12345697>
[ 	]*[a-f0-9]+:	f2 0f e6 0d 78 56 34 12 	cvtpd2dq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 1234569f <_start\+0x1234569f>
[ 	]*[a-f0-9]+:	0f 5a 0d 78 56 34 12 	cvtps2pd xmm1,QWORD PTR \[rip\+0x12345678\]        # 123456a6 <_start\+0x123456a6>
[ 	]*[a-f0-9]+:	f3 0f 5b 0d 78 56 34 12 	cvttps2dq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 123456ae <_start\+0x123456ae>
[ 	]*[a-f0-9]+:	f2 0f 7c 0d 78 56 34 12 	haddps xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 123456b6 <_start\+0x123456b6>
[ 	]*[a-f0-9]+:	f3 0f 7f 0d 78 56 34 12 	movdqu XMMWORD PTR \[rip\+0x12345678\],xmm1        # 123456be <_start\+0x123456be>
[ 	]*[a-f0-9]+:	f3 0f 6f 0d 78 56 34 12 	movdqu xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 123456c6 <_start\+0x123456c6>
[ 	]*[a-f0-9]+:	66 0f 17 0d 78 56 34 12 	movhpd QWORD PTR \[rip\+0x12345678\],xmm1        # 123456ce <_start\+0x123456ce>
[ 	]*[a-f0-9]+:	66 0f 16 0d 78 56 34 12 	movhpd xmm1,QWORD PTR \[rip\+0x12345678\]        # 123456d6 <_start\+0x123456d6>
[ 	]*[a-f0-9]+:	0f 17 0d 78 56 34 12 	movhps QWORD PTR \[rip\+0x12345678\],xmm1        # 123456dd <_start\+0x123456dd>
[ 	]*[a-f0-9]+:	0f 16 0d 78 56 34 12 	movhps xmm1,QWORD PTR \[rip\+0x12345678\]        # 123456e4 <_start\+0x123456e4>
[ 	]*[a-f0-9]+:	66 0f 13 0d 78 56 34 12 	movlpd QWORD PTR \[rip\+0x12345678\],xmm1        # 123456ec <_start\+0x123456ec>
[ 	]*[a-f0-9]+:	66 0f 12 0d 78 56 34 12 	movlpd xmm1,QWORD PTR \[rip\+0x12345678\]        # 123456f4 <_start\+0x123456f4>
[ 	]*[a-f0-9]+:	0f 13 0d 78 56 34 12 	movlps QWORD PTR \[rip\+0x12345678\],xmm1        # 123456fb <_start\+0x123456fb>
[ 	]*[a-f0-9]+:	0f 12 0d 78 56 34 12 	movlps xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345702 <_start\+0x12345702>
[ 	]*[a-f0-9]+:	66 0f d6 0d 78 56 34 12 	movq   QWORD PTR \[rip\+0x12345678\],xmm1        # 1234570a <_start\+0x1234570a>
[ 	]*[a-f0-9]+:	f3 0f 7e 0d 78 56 34 12 	movq   xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345712 <_start\+0x12345712>
[ 	]*[a-f0-9]+:	f3 0f 16 0d 78 56 34 12 	movshdup xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 1234571a <_start\+0x1234571a>
[ 	]*[a-f0-9]+:	f3 0f 12 0d 78 56 34 12 	movsldup xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345722 <_start\+0x12345722>
[ 	]*[a-f0-9]+:	f3 0f 70 0d 78 56 34 12 90 	pshufhw xmm1,XMMWORD PTR \[rip\+0x12345678\],0x90        # 1234572b <_start\+0x1234572b>
[ 	]*[a-f0-9]+:	f2 0f 70 0d 78 56 34 12 90 	pshuflw xmm1,XMMWORD PTR \[rip\+0x12345678\],0x90        # 12345734 <_start\+0x12345734>
[ 	]*[a-f0-9]+:	0f 60 0d 78 56 34 12 	punpcklbw mm1,DWORD PTR \[rip\+0x12345678\]        # 1234573b <_start\+0x1234573b>
[ 	]*[a-f0-9]+:	0f 62 0d 78 56 34 12 	punpckldq mm1,DWORD PTR \[rip\+0x12345678\]        # 12345742 <_start\+0x12345742>
[ 	]*[a-f0-9]+:	0f 61 0d 78 56 34 12 	punpcklwd mm1,DWORD PTR \[rip\+0x12345678\]        # 12345749 <_start\+0x12345749>
[ 	]*[a-f0-9]+:	66 0f 60 0d 78 56 34 12 	punpcklbw xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345751 <_start\+0x12345751>
[ 	]*[a-f0-9]+:	66 0f 62 0d 78 56 34 12 	punpckldq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345759 <_start\+0x12345759>
[ 	]*[a-f0-9]+:	66 0f 61 0d 78 56 34 12 	punpcklwd xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345761 <_start\+0x12345761>
[ 	]*[a-f0-9]+:	66 0f 6c 0d 78 56 34 12 	punpcklqdq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345769 <_start\+0x12345769>
[ 	]*[a-f0-9]+:	66 0f 2e 0d 78 56 34 12 	ucomisd xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345771 <_start\+0x12345771>
[ 	]*[a-f0-9]+:	0f 2e 0d 78 56 34 12 	ucomiss xmm1,DWORD PTR \[rip\+0x12345678\]        # 12345778 <_start\+0x12345778>
[ 	]*[a-f0-9]+:	f2 0f c2 00 00       	cmpeqsd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f c2 00 00       	cmpeqss xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 2a 00          	cvtpi2pd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 2a 00             	cvtpi2ps xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 2d 00             	cvtps2pi mm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 2d 00          	cvtsd2si eax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 48 0f 2d 00       	cvtsd2si rax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 2c 00          	cvttsd2si eax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 48 0f 2c 00       	cvttsd2si rax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5a 00          	cvtsd2ss xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5a 00          	cvtss2sd xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 2d 00          	cvtss2si eax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 48 0f 2d 00       	cvtss2si rax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 2c 00          	cvttss2si eax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 48 0f 2c 00       	cvttss2si rax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5e 00          	divsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5e 00          	divss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5f 00          	maxsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5f 00          	maxss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 2b 00          	movntsd QWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f3 0f 2b 00          	movntss DWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f2 0f 10 00          	movsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 11 00          	movsd  QWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f3 0f 10 00          	movss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 11 00          	movss  DWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f2 0f 59 00          	mulsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 59 00          	mulss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 53 00          	rcpss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 3a 0b 00 00    	roundsd xmm0,QWORD PTR \[rax\],0x0
[ 	]*[a-f0-9]+:	66 0f 3a 0a 00 00    	roundss xmm0,DWORD PTR \[rax\],0x0
[ 	]*[a-f0-9]+:	f3 0f 52 00          	rsqrtss xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 51 00          	sqrtsd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 51 00          	sqrtss xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5c 00          	subsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5c 00          	subss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 20 00       	pmovsxbw xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 21 00       	pmovsxbd xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 22 00       	pmovsxbq xmm0,WORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 23 00       	pmovsxwd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 24 00       	pmovsxwq xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 25 00       	pmovsxdq xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 30 00       	pmovzxbw xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 31 00       	pmovzxbd xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 32 00       	pmovzxbq xmm0,WORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 33 00       	pmovzxwd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 34 00       	pmovzxwq xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 35 00       	pmovzxdq xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 3a 21 00 00    	insertps xmm0,DWORD PTR \[rax\],0x0
[ 	]*[a-f0-9]+:	66 0f 15 08          	unpckhpd xmm1,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 15 08             	unpckhps xmm1,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 14 08          	unpcklpd xmm1,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 14 08             	unpcklps xmm1,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f c2 f7 10       	cmpss  xmm6,xmm7,0x10
[ 	]*[a-f0-9]+:	f3 0f c2 38 10       	cmpss  xmm7,DWORD PTR \[rax\],0x10
[ 	]*[a-f0-9]+:	f2 0f c2 f7 10       	cmpsd  xmm6,xmm7,0x10
[ 	]*[a-f0-9]+:	f2 0f c2 38 10       	cmpsd  xmm7,QWORD PTR \[rax\],0x10
[ 	]*[a-f0-9]+:	f2 0f d0 0d 78 56 34 12 	addsubps xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345880 <_start\+0x12345880>
[ 	]*[a-f0-9]+:	66 0f 2f 0d 78 56 34 12 	comisd xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345888 <_start\+0x12345888>
[ 	]*[a-f0-9]+:	0f 2f 0d 78 56 34 12 	comiss xmm1,DWORD PTR \[rip\+0x12345678\]        # 1234588f <_start\+0x1234588f>
[ 	]*[a-f0-9]+:	f3 0f e6 0d 78 56 34 12 	cvtdq2pd xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345897 <_start\+0x12345897>
[ 	]*[a-f0-9]+:	f2 0f e6 0d 78 56 34 12 	cvtpd2dq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 1234589f <_start\+0x1234589f>
[ 	]*[a-f0-9]+:	0f 5a 0d 78 56 34 12 	cvtps2pd xmm1,QWORD PTR \[rip\+0x12345678\]        # 123458a6 <_start\+0x123458a6>
[ 	]*[a-f0-9]+:	f3 0f 5b 0d 78 56 34 12 	cvttps2dq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 123458ae <_start\+0x123458ae>
[ 	]*[a-f0-9]+:	f2 0f 7c 0d 78 56 34 12 	haddps xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 123458b6 <_start\+0x123458b6>
[ 	]*[a-f0-9]+:	f3 0f 7f 0d 78 56 34 12 	movdqu XMMWORD PTR \[rip\+0x12345678\],xmm1        # 123458be <_start\+0x123458be>
[ 	]*[a-f0-9]+:	f3 0f 6f 0d 78 56 34 12 	movdqu xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 123458c6 <_start\+0x123458c6>
[ 	]*[a-f0-9]+:	66 0f 17 0d 78 56 34 12 	movhpd QWORD PTR \[rip\+0x12345678\],xmm1        # 123458ce <_start\+0x123458ce>
[ 	]*[a-f0-9]+:	66 0f 16 0d 78 56 34 12 	movhpd xmm1,QWORD PTR \[rip\+0x12345678\]        # 123458d6 <_start\+0x123458d6>
[ 	]*[a-f0-9]+:	0f 17 0d 78 56 34 12 	movhps QWORD PTR \[rip\+0x12345678\],xmm1        # 123458dd <_start\+0x123458dd>
[ 	]*[a-f0-9]+:	0f 16 0d 78 56 34 12 	movhps xmm1,QWORD PTR \[rip\+0x12345678\]        # 123458e4 <_start\+0x123458e4>
[ 	]*[a-f0-9]+:	66 0f 13 0d 78 56 34 12 	movlpd QWORD PTR \[rip\+0x12345678\],xmm1        # 123458ec <_start\+0x123458ec>
[ 	]*[a-f0-9]+:	66 0f 12 0d 78 56 34 12 	movlpd xmm1,QWORD PTR \[rip\+0x12345678\]        # 123458f4 <_start\+0x123458f4>
[ 	]*[a-f0-9]+:	0f 13 0d 78 56 34 12 	movlps QWORD PTR \[rip\+0x12345678\],xmm1        # 123458fb <_start\+0x123458fb>
[ 	]*[a-f0-9]+:	0f 12 0d 78 56 34 12 	movlps xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345902 <_start\+0x12345902>
[ 	]*[a-f0-9]+:	66 0f d6 0d 78 56 34 12 	movq   QWORD PTR \[rip\+0x12345678\],xmm1        # 1234590a <_start\+0x1234590a>
[ 	]*[a-f0-9]+:	f3 0f 7e 0d 78 56 34 12 	movq   xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345912 <_start\+0x12345912>
[ 	]*[a-f0-9]+:	f3 0f 16 0d 78 56 34 12 	movshdup xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 1234591a <_start\+0x1234591a>
[ 	]*[a-f0-9]+:	f3 0f 12 0d 78 56 34 12 	movsldup xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345922 <_start\+0x12345922>
[ 	]*[a-f0-9]+:	f3 0f 70 0d 78 56 34 12 90 	pshufhw xmm1,XMMWORD PTR \[rip\+0x12345678\],0x90        # 1234592b <_start\+0x1234592b>
[ 	]*[a-f0-9]+:	f2 0f 70 0d 78 56 34 12 90 	pshuflw xmm1,XMMWORD PTR \[rip\+0x12345678\],0x90        # 12345934 <_start\+0x12345934>
[ 	]*[a-f0-9]+:	0f 60 0d 78 56 34 12 	punpcklbw mm1,DWORD PTR \[rip\+0x12345678\]        # 1234593b <_start\+0x1234593b>
[ 	]*[a-f0-9]+:	0f 62 0d 78 56 34 12 	punpckldq mm1,DWORD PTR \[rip\+0x12345678\]        # 12345942 <_start\+0x12345942>
[ 	]*[a-f0-9]+:	0f 61 0d 78 56 34 12 	punpcklwd mm1,DWORD PTR \[rip\+0x12345678\]        # 12345949 <_start\+0x12345949>
[ 	]*[a-f0-9]+:	66 0f 60 0d 78 56 34 12 	punpcklbw xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345951 <_start\+0x12345951>
[ 	]*[a-f0-9]+:	66 0f 62 0d 78 56 34 12 	punpckldq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345959 <_start\+0x12345959>
[ 	]*[a-f0-9]+:	66 0f 61 0d 78 56 34 12 	punpcklwd xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345961 <_start\+0x12345961>
[ 	]*[a-f0-9]+:	66 0f 6c 0d 78 56 34 12 	punpcklqdq xmm1,XMMWORD PTR \[rip\+0x12345678\]        # 12345969 <_start\+0x12345969>
[ 	]*[a-f0-9]+:	66 0f 2e 0d 78 56 34 12 	ucomisd xmm1,QWORD PTR \[rip\+0x12345678\]        # 12345971 <_start\+0x12345971>
[ 	]*[a-f0-9]+:	0f 2e 0d 78 56 34 12 	ucomiss xmm1,DWORD PTR \[rip\+0x12345678\]        # 12345978 <_start\+0x12345978>
[ 	]*[a-f0-9]+:	f2 0f c2 00 00       	cmpeqsd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f c2 00 00       	cmpeqss xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 2a 00          	cvtpi2pd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 2a 00             	cvtpi2ps xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 2d 00             	cvtps2pi mm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 2d 00          	cvtsd2si eax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 48 0f 2d 00       	cvtsd2si rax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 2c 00          	cvttsd2si eax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 48 0f 2c 00       	cvttsd2si rax,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5a 00          	cvtsd2ss xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5a 00          	cvtss2sd xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 2d 00          	cvtss2si eax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 48 0f 2d 00       	cvtss2si rax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 2c 00          	cvttss2si eax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 48 0f 2c 00       	cvttss2si rax,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5e 00          	divsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5e 00          	divss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5f 00          	maxsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5f 00          	maxss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5d 00          	minss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 2b 00          	movntsd QWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f3 0f 2b 00          	movntss DWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f2 0f 10 00          	movsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 11 00          	movsd  QWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f3 0f 10 00          	movss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 11 00          	movss  DWORD PTR \[rax\],xmm0
[ 	]*[a-f0-9]+:	f2 0f 59 00          	mulsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 59 00          	mulss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 53 00          	rcpss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 3a 0b 00 00    	roundsd xmm0,QWORD PTR \[rax\],0x0
[ 	]*[a-f0-9]+:	66 0f 3a 0a 00 00    	roundss xmm0,DWORD PTR \[rax\],0x0
[ 	]*[a-f0-9]+:	f3 0f 52 00          	rsqrtss xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 51 00          	sqrtsd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 51 00          	sqrtss xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f2 0f 5c 00          	subsd  xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f 5c 00          	subss  xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 20 00       	pmovsxbw xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 21 00       	pmovsxbd xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 22 00       	pmovsxbq xmm0,WORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 23 00       	pmovsxwd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 24 00       	pmovsxwq xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 25 00       	pmovsxdq xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 30 00       	pmovzxbw xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 31 00       	pmovzxbd xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 32 00       	pmovzxbq xmm0,WORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 33 00       	pmovzxwd xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 34 00       	pmovzxwq xmm0,DWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 38 35 00       	pmovzxdq xmm0,QWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 3a 21 00 00    	insertps xmm0,DWORD PTR \[rax\],0x0
[ 	]*[a-f0-9]+:	66 0f 15 00          	unpckhpd xmm0,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 15 00             	unpckhps xmm0,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	66 0f 14 00          	unpcklpd xmm0,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	0f 14 00             	unpcklps xmm0,XMMWORD PTR \[rax\]
[ 	]*[a-f0-9]+:	f3 0f c2 f7 10       	cmpss  xmm6,xmm7,0x10
[ 	]*[a-f0-9]+:	f3 0f c2 38 10       	cmpss  xmm7,DWORD PTR \[rax\],0x10
[ 	]*[a-f0-9]+:	f2 0f c2 f7 10       	cmpsd  xmm6,xmm7,0x10
[ 	]*[a-f0-9]+:	f2 0f c2 38 10       	cmpsd  xmm7,QWORD PTR \[rax\],0x10
#pass
