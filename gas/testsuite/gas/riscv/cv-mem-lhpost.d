#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+8003928b[ 	]+cv.lh[ 	]+t0,\(t2\),-2048
[ 	]+4:[ 	]+00f31e8b[ 	]+cv.lh[ 	]+t4,\(t1\),15
[ 	]+8:[ 	]+7fff1e0b[ 	]+cv.lh[ 	]+t3,\(t5\),2047
