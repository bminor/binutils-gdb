#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+1a63b2ab[ 	]+cv.lhu[ 	]+t0,t1\(t2\)
[ 	]+4:[ 	]+1bc33eab[ 	]+cv.lhu[ 	]+t4,t3\(t1\)
[ 	]+8:[ 	]+1a5f3e2b[ 	]+cv.lhu[ 	]+t3,t0\(t5\)
