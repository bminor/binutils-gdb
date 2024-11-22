#as: -mfuture
#objdump: -dr -Mfuture
#name: Future tests

.*


Disassembly of section \.text:

0+0 <_start>:
.*:	(90 58 4c 7d|7d 4c 58 90) 	subwus  r10,r11,r12
.*:	(90 58 4c 7d|7d 4c 58 90) 	subwus  r10,r11,r12
.*:	(91 58 4c 7d|7d 4c 58 91) 	subwus\. r10,r11,r12
.*:	(91 58 4c 7d|7d 4c 58 91) 	subwus\. r10,r11,r12
.*:	(90 ac 96 7e|7e 96 ac 90) 	subdus  r20,r21,r22
.*:	(90 ac 96 7e|7e 96 ac 90) 	subdus  r20,r21,r22
.*:	(91 ac 96 7e|7e 96 ac 91) 	subdus\. r20,r21,r22
.*:	(91 ac 96 7e|7e 96 ac 91) 	subdus\. r20,r21,r22
.*:	(1b 5c 4a 7c|7c 4a 5c 1b) 	lxvrl   vs34,r10,r11
.*:	(5b 64 6a 7c|7c 6a 64 5b) 	lxvrll  vs35,r10,r12
.*:	(9a 6c aa 7c|7c aa 6c 9a) 	lxvprl  vs36,r10,r13
.*:	(da 74 ea 7c|7c ea 74 da) 	lxvprll vs38,r10,r14
.*:	(1b 7d 0a 7d|7d 0a 7d 1b) 	stxvrl  vs40,r10,r15
.*:	(5b 85 2a 7d|7d 2a 85 5b) 	stxvrll vs41,r10,r16
.*:	(9a 8d 2a 7d|7d 2a 8d 9a) 	stxvprl vs40,r10,r17
.*:	(da 95 6a 7d|7d 6a 95 da) 	stxvprll vs42,r10,r18
.*:	(c6 9d e9 f3|f3 e9 9d c6) 	xvrlw   vs31,vs41,vs51
.*:	(1e a4 ca f3|f3 ca a4 1e) 	xvadduwm vs30,vs42,vs52
.*:	(5e ac ca f3|f3 ca ac 5e) 	xvadduhm vs30,vs42,vs53
.*:	(9e b4 ab f3|f3 ab b4 9e) 	xvsubuwm vs29,vs43,vs54
.*:	(d9 f4 e0 f3|f3 e0 f4 d9) 	xvsubuhm vs63,vs0,vs30
.*:	(1e e5 c8 f3|f3 c8 e5 1e) 	xvmuluwm vs30,vs40,vs60
.*:	(5f f5 9d f3|f3 9d f5 5f) 	xvmuluhm vs60,vs61,vs62
.*:	(9f 25 43 f0|f0 43 25 9f) 	xvmulhsw vs34,vs35,vs36
.*:	(d8 1d 22 f0|f0 22 1d d8) 	xvmulhsh vs1,vs2,vs3
.*:	(90 b3 95 f2|f2 95 b3 90) 	xvmulhuw vs20,vs21,vs22
.*:	(d6 a3 8a f2|f2 8a a3 d6) 	xvmulhuh vs20,vs42,vs52
#pass
