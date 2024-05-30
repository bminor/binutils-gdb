#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+0a63b2ab[ 	]+cv.lh[ 	]+t0,t1\(t2\)
[ 	]+4:[ 	]+0bc33eab[ 	]+cv.lh[ 	]+t4,t3\(t1\)
[ 	]+8:[ 	]+0a5f3e2b[ 	]+cv.lh[ 	]+t3,t0\(t5\)
