#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+1863b2ab[ 	]+cv.lbu[ 	]+t0,t1\(t2\)
[ 	]+4:[ 	]+19c33eab[ 	]+cv.lbu[ 	]+t4,t3\(t1\)
[ 	]+8:[ 	]+185f3e2b[ 	]+cv.lbu[ 	]+t3,t0\(t5\)
