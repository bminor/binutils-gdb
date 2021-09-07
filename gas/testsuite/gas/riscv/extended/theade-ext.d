#as: -march=rv32gcxtheade
#objdump: -dr

.*:[ 	]+file format .*


Disassembly of section .text:

00000000 <.text>:
[       ]+[0-9a-f]+:\s+cff01073          	csrw	0xcff,zero
[       ]+[0-9a-f]+:\s+02a5000b          	dcache.ipa	a0
[       ]+[0-9a-f]+:\s+0295000b          	dcache.cpa	a0
[       ]+[0-9a-f]+:\s+02b5000b          	dcache.cipa	a0
[       ]+[0-9a-f]+:\s+0225000b          	dcache.isw	a0
[       ]+[0-9a-f]+:\s+0215000b          	dcache.csw	a0
[       ]+[0-9a-f]+:\s+0235000b          	dcache.cisw	a0
[       ]+[0-9a-f]+:\s+0020000b          	dcache.iall
[       ]+[0-9a-f]+:\s+0010000b          	dcache.call
[       ]+[0-9a-f]+:\s+0030000b          	dcache.ciall
[       ]+[0-9a-f]+:\s+0100000b          	icache.iall
[       ]+[0-9a-f]+:\s+0385000b          	icache.ipa	a0
[       ]+[0-9a-f]+:\s+0180000b          	sync
[       ]+[0-9a-f]+:\s+01a0000b          	sync.i
[       ]+[0-9a-f]+:\s+02c5950b          	addsl	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+1105950b          	srri	a0,a1,16
[       ]+[0-9a-f]+:\s+20c5950b          	mula	a0,a1,a2
[       ]+[0-9a-f]+:\s+28c5950b          	mulah	a0,a1,a2
[       ]+[0-9a-f]+:\s+22c5950b          	muls	a0,a1,a2
[       ]+[0-9a-f]+:\s+2ac5950b          	mulsh	a0,a1,a2
[       ]+[0-9a-f]+:\s+40c5950b          	mveqz	a0,a1,a2
[       ]+[0-9a-f]+:\s+42c5950b          	mvnez	a0,a1,a2
[       ]+[0-9a-f]+:\s+4105a50b          	ext	a0,a1,16,16
[       ]+[0-9a-f]+:\s+4105b50b          	extu	a0,a1,16,16
[       ]+[0-9a-f]+:\s+8605950b          	ff1	a0,a1
[       ]+[0-9a-f]+:\s+8405950b          	ff0	a0,a1
[       ]+[0-9a-f]+:\s+8205950b          	rev	a0,a1
[       ]+[0-9a-f]+:\s+8905950b          	tst	a0,a1,16
[       ]+[0-9a-f]+:\s+8005950b          	tstnbz	a0,a1
[       ]+[0-9a-f]+:\s+02c5c50b          	lrb	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+22c5c50b          	lrh	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+42c5c50b          	lrw	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+82c5c50b          	lrbu	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+a2c5c50b          	lrhu	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+1af5c50b          	lbia	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+0af5c50b          	lbib	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+3af5c50b          	lhia	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+2af5c50b          	lhib	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+5af5c50b          	lwia	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+4af5c50b          	lwib	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+02c5d50b          	srb	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+22c5d50b          	srh	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+42c5d50b          	srw	a0,a1,a2,1
[       ]+[0-9a-f]+:\s+1af5d50b          	sbia	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+0af5d50b          	sbib	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+3af5d50b          	shia	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+2af5d50b          	shib	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+5ac5d50b          	swia	a0,\(a1\),12,1
[       ]+[0-9a-f]+:\s+4af5d50b          	swib	a0,\(a1\),15,1
[       ]+[0-9a-f]+:\s+c000150b          	fmv.x.hw	a0,ft0
[       ]+[0-9a-f]+:\s+a005100b          	fmv.hw.x	ft0,a0
[       ]+[0-9a-f]+:\s+0040000b          	ipush
[       ]+[0-9a-f]+:\s+0050000b          	ipop
